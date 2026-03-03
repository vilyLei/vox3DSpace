#include "render_object.hpp"
#include "script_loader.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <stdexcept>
#include <cstring>
#include <iostream>

namespace vkapp {

RenderObject::RenderObject(VulkanDevice* device, const RenderObjectDesc& desc)
    : device_(device)
    , position_(desc.position)
    , scale_(desc.scale)
    , scriptPaths_(desc.scriptPaths)
{
    // Create command pool for buffer copy
    QueueFamilyIndices indices = device_->getQueueFamilyIndices();
    vk::CommandPoolCreateInfo poolInfo{};
    poolInfo.flags = vk::CommandPoolCreateFlagBits::eTransient;
    poolInfo.queueFamilyIndex = indices.graphicsFamily.value();
    commandPool_ = device_->getDevice().createCommandPool(poolInfo);

    // Create GPU buffers
    createVertexBuffer(desc.vertices);
    createIndexBuffer(desc.indices);

    // Create per-object uniform buffer
    uniformBuffer_ = std::make_unique<VulkanUniformBuffer>(device_);

    // Determine script sources: file paths take precedence over inline strings
    std::string colorSrc, rotSrc, posSrc;

    if (!scriptPaths_.colorScript.empty()) {
        colorSrc = ScriptLoader::load(scriptPaths_.colorScript);
        colorMtime_ = ScriptLoader::lastModified(scriptPaths_.colorScript);
    } else {
        colorSrc = desc.colorScript;
    }

    if (!scriptPaths_.rotationScript.empty()) {
        rotSrc = ScriptLoader::load(scriptPaths_.rotationScript);
        rotMtime_ = ScriptLoader::lastModified(scriptPaths_.rotationScript);
    } else {
        rotSrc = desc.rotationScript;
    }

    if (!scriptPaths_.positionScript.empty()) {
        posSrc = ScriptLoader::load(scriptPaths_.positionScript);
        posMtime_ = ScriptLoader::lastModified(scriptPaths_.positionScript);
        hasPositionPath_ = true;
    } else if (!desc.positionScript.empty()) {
        posSrc = desc.positionScript;
    }

    // Compile mmrsl scripts
    if (!colorSrc.empty()) {
        if (!colorParser_.compile(colorSrc)) {
            throw std::runtime_error("RenderObject color script compile failed: "
                                     + colorParser_.getLastError());
        }
    }
    if (!rotSrc.empty()) {
        if (!rotParser_.compile(rotSrc)) {
            throw std::runtime_error("RenderObject rotation script compile failed: "
                                     + rotParser_.getLastError());
        }
    }
    if (!posSrc.empty()) {
        if (!posParser_.compile(posSrc)) {
            throw std::runtime_error("RenderObject position script compile failed: "
                                     + posParser_.getLastError());
        }
        hasPositionScript_ = true;
    }
}

RenderObject::~RenderObject() {
    device_->getDevice().destroyBuffer(indexBuffer_);
    device_->getDevice().freeMemory(indexBufferMemory_);
    device_->getDevice().destroyBuffer(vertexBuffer_);
    device_->getDevice().freeMemory(vertexBufferMemory_);
    device_->getDevice().destroyCommandPool(commandPool_);
}

void RenderObject::update(float t, const glm::mat4& view, const glm::mat4& proj) {
    UniformBufferObject ubo{};

    // Rotation angle from script
    float angle = rotParser_.execute({mmrsl::Value(t)}).asFloat();

    // World position: from script or fallback to static position_
    glm::vec2 pos = position_;
    if (hasPositionScript_) {
        auto pv = posParser_.execute({mmrsl::Value(t)}).asVec2();
        pos = {pv.x, pv.y};
    }

    // Model = translate * rotate * scale
    ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3(pos, 0.0f))
              * glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f))
              * glm::scale(glm::mat4(1.0f), glm::vec3(scale_, scale_, 1.0f));

    ubo.view = view;
    ubo.proj = proj;

    // Color tint from script
    auto cv = colorParser_.execute({mmrsl::Value(t)}).asVec4();
    ubo.colorTint = {cv.x, cv.y, cv.z, cv.w};

    uniformBuffer_->update(ubo);
}

void RenderObject::draw(vk::CommandBuffer cmd, vk::PipelineLayout layout) const {
    // Bind this object's descriptor set
    vk::DescriptorSet ds = uniformBuffer_->getDescriptorSet();
    cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                           layout, 0, 1, &ds, 0, nullptr);

    // Bind mesh buffers
    vk::Buffer     vertexBuffers[] = {vertexBuffer_};
    vk::DeviceSize offsets[]       = {0};
    cmd.bindVertexBuffers(0, 1, vertexBuffers, offsets);
    cmd.bindIndexBuffer(indexBuffer_, 0, vk::IndexType::eUint16);

    // Draw
    cmd.drawIndexed(indexCount_, 1, 0, 0, 0);
}

bool RenderObject::reloadIfChanged() {
    bool reloaded = false;

    // Check color script
    if (!scriptPaths_.colorScript.empty()) {
        auto mtime = ScriptLoader::lastModified(scriptPaths_.colorScript);
        if (mtime != colorMtime_) {
            std::string src = ScriptLoader::load(scriptPaths_.colorScript);
            if (colorParser_.compile(src)) {
                colorMtime_ = mtime;
                reloaded = true;
            } else {
                std::cerr << "[HotReload] Color script compile failed (keeping old): "
                          << colorParser_.getLastError() << std::endl;
            }
        }
    }

    // Check rotation script
    if (!scriptPaths_.rotationScript.empty()) {
        auto mtime = ScriptLoader::lastModified(scriptPaths_.rotationScript);
        if (mtime != rotMtime_) {
            std::string src = ScriptLoader::load(scriptPaths_.rotationScript);
            if (rotParser_.compile(src)) {
                rotMtime_ = mtime;
                reloaded = true;
            } else {
                std::cerr << "[HotReload] Rotation script compile failed (keeping old): "
                          << rotParser_.getLastError() << std::endl;
            }
        }
    }

    // Check position script
    if (hasPositionPath_) {
        auto mtime = ScriptLoader::lastModified(scriptPaths_.positionScript);
        if (mtime != posMtime_) {
            std::string src = ScriptLoader::load(scriptPaths_.positionScript);
            if (posParser_.compile(src)) {
                posMtime_ = mtime;
                reloaded = true;
            } else {
                std::cerr << "[HotReload] Position script compile failed (keeping old): "
                          << posParser_.getLastError() << std::endl;
            }
        }
    }

    return reloaded;
}

// ---------------------------------------------------------------------------
// Private helpers
// ---------------------------------------------------------------------------

void RenderObject::createVertexBuffer(const std::vector<Vertex>& vertices) {
    vk::DeviceSize size = sizeof(vertices[0]) * vertices.size();

    vk::Buffer       staging;
    vk::DeviceMemory stagingMem;
    device_->createBuffer(size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        staging, stagingMem);

    void* data = device_->getDevice().mapMemory(stagingMem, 0, size);
    memcpy(data, vertices.data(), static_cast<size_t>(size));
    device_->getDevice().unmapMemory(stagingMem);

    device_->createBuffer(size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vertexBuffer_, vertexBufferMemory_);

    copyBuffer(staging, vertexBuffer_, size);

    device_->getDevice().destroyBuffer(staging);
    device_->getDevice().freeMemory(stagingMem);
}

void RenderObject::createIndexBuffer(const std::vector<uint16_t>& indices) {
    vk::DeviceSize size = sizeof(indices[0]) * indices.size();
    indexCount_ = static_cast<uint32_t>(indices.size());

    vk::Buffer       staging;
    vk::DeviceMemory stagingMem;
    device_->createBuffer(size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        staging, stagingMem);

    void* data = device_->getDevice().mapMemory(stagingMem, 0, size);
    memcpy(data, indices.data(), static_cast<size_t>(size));
    device_->getDevice().unmapMemory(stagingMem);

    device_->createBuffer(size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        indexBuffer_, indexBufferMemory_);

    copyBuffer(staging, indexBuffer_, size);

    device_->getDevice().destroyBuffer(staging);
    device_->getDevice().freeMemory(stagingMem);
}

void RenderObject::copyBuffer(vk::Buffer src, vk::Buffer dst, vk::DeviceSize size) {
    vk::CommandBufferAllocateInfo allocInfo{};
    allocInfo.level              = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandPool        = commandPool_;
    allocInfo.commandBufferCount = 1;

    vk::CommandBuffer cmd = device_->getDevice().allocateCommandBuffers(allocInfo)[0];

    vk::CommandBufferBeginInfo beginInfo{};
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    cmd.begin(beginInfo);

    vk::BufferCopy region{};
    region.size = size;
    cmd.copyBuffer(src, dst, region);
    cmd.end();

    vk::SubmitInfo submitInfo{};
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &cmd;
    device_->getGraphicsQueue().submit(submitInfo);
    device_->getGraphicsQueue().waitIdle();

    device_->getDevice().freeCommandBuffers(commandPool_, cmd);
}

} // namespace vkapp
