#include "OglResUtils.h"
namespace Voxol::Test
{
namespace ResUtils
{
const char* vertShaderSource = R"(#version 330 core
precision highp float;

layout(location = 0) in vec2 a_pos;

uniform mat3 u_matrix;

void main() {
    vec3 pos = u_matrix * vec3(a_pos, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
})";

const char* fragShaderSource = R"(#version 330 core
precision mediump float;
uniform vec4 u_color;
out vec4 outColor;
void main() {
    outColor = u_color;
})";


const char* vertTexSource          = R"(#version 330 core
precision highp float;

layout(location = 0) in vec4 a_pos;

uniform mat3 u_matrix;
out vec2 v_uv;
void main() {
    v_uv = a_pos.zw;
    mat3 trans = u_matrix;
    vec3 pos = trans * vec3(a_pos.xy, 1.0);
    gl_Position = vec4(pos.xy, 0.0, 1.0);
}
)";
const char* fragTexSource          = R"(#version 330 core
    precision     mediump float;
uniform vec4      u_color;
in vec2           v_uv;
uniform sampler2D u_tex0;
out vec4          fragColor;
void              main()
{
    vec4 texColor = texture(u_tex0, v_uv);
    fragColor = texColor * u_color;
}
)";
const char* fragRedFormatTexSource = R"(#version 330 core
    precision     mediump float;
uniform vec4      u_color;
in vec2           v_uv;
uniform sampler2D u_tex0;
out vec4          fragColor;
void              main()
{
    vec4 texColor = u_color;
    texColor.a *= texture(u_tex0, v_uv).r;
    fragColor = texColor;
}
)";

const char* fragRGBGlyphFormatTexSource = R"(#version 330 core
    precision     mediump float;
uniform vec4      u_color;
in vec2           v_uv;
uniform sampler2D u_tex0;
out vec4          fragColor;
void              main()
{
    vec4 texColor = u_color;
    vec3 glyColor = texture(u_tex0, v_uv).rgb;
    texColor.rgb *= glyColor;
    texColor.a = (glyColor.r + glyColor.g + glyColor.b)/3.0;
    fragColor = texColor;
}
)";

GLuint compileShader(GLenum type, const char* source)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint result = GL_FALSE;
    int   infoLogLength;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        std::vector<char> shaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(shader, infoLogLength, NULL, &shaderErrorMessage[0]);
        printf("%s\n", &shaderErrorMessage[0]);
    }
    return shader;
}

GLuint createSahderProgram(const char* vertSource, const char* fragSource)
{
    GLint result = GL_FALSE;
    int   infoLogLength;

    GLuint vs        = ResUtils::compileShader(GL_VERTEX_SHADER, vertSource);
    GLuint fs        = ResUtils::compileShader(GL_FRAGMENT_SHADER, fragSource);
    GLuint programID = glCreateProgram();
    glAttachShader(programID, vs);
    glAttachShader(programID, fs);
    glLinkProgram(programID);

    // Check the program
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
        printf("%s\n", &programErrorMessage[0]);
    }

    glDetachShader(programID, vs);
    glDetachShader(programID, fs);

    glDeleteShader(vs);
    glDeleteShader(fs);

    return programID;
}


std::vector<float> getVertsWithVEOSegN(int n)
{

    float              x = 0, y = 0, w = 1, h = 1;
    std::vector<float> verts{};
    if (n > 1)
    {
        return verts;
    }
    else
    {
        verts = {
            x, y,
            x + w, y,
            x + w, y + h,
            x, y + h};
        return verts;
    }
}
std::vector<float> getVertsWithUVVEOSegN(int n)
{

    float              x = 0, y = 0, w = 1, h = 1;
    std::vector<float> verts{};
    if (n > 1)
    {
        return verts;
    }
    else
    {
        verts = {
            x, y, 0, 0,
            x + w, y, 1, 0,
            x + w, y + h, 1, 1,
            x, y + h, 0, 1};
        return verts;
    }
}

std::vector<float> getVertsWithUVVEO(float u0, float v0, float u1, float v1)
{
    float              x = 0, y = 0, w = 1, h = 1;
    std::vector<float> verts = {
        x, y, u0, v0,
        x + w, y, u1, v0,
        x + w, y + h, u1, v1,
        x, y + h, u0, v1};
    return verts;
}

std::vector<float> getVertsWithUVVEOFlipY(float u0, float v0, float u1, float v1)
{
    float              x = 0, y = 0, w = 1, h = 1;
    std::vector<float> verts = {
        x, y, u0, 1.0f - v0,
        x + w, y, u1, 1.0f - v0,
        x + w, y + h, u1, 1.0f - v1,
        x, y + h, u0, 1.0f - v1};
    return verts;
}

std::vector<unsigned short> getIndicesWithSegN(int n)
{

    std::vector<unsigned short> indices(n * 6);
    auto                        k = 0;
    for (auto i = 0; i < n; ++i)
    {
        auto baseI     = i * 4;
        indices[k]     = baseI;
        indices[k + 1] = baseI + 1;
        indices[k + 2] = baseI + 2;
        indices[k + 3] = baseI + 2;
        indices[k + 4] = baseI + 3;
        indices[k + 5] = baseI;
        k += 6;
    }
    return indices;
}

GLuint createTextureFromImageBytes(int imageWidth, int imageHeight, const std::vector<unsigned char>& buffer, GLint internalformat, GLint format, GLint alignment)
{

    GLuint tex = GL_ZERO;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);

    glTexImage2D(
        GL_TEXTURE_2D, 0, internalformat, imageWidth, imageHeight, 0, format,
        GL_UNSIGNED_BYTE, buffer.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    auto mipmapFlag = (imageWidth & (imageWidth - 1)) == 0 &&
        (imageHeight & (imageHeight - 1)) == 0;
    if (mipmapFlag)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    return tex;
}
std::vector<unsigned char> createRGBAImgBytes(int imageWidth, int imageHeight)
{
    std::vector<unsigned char> data(imageWidth * imageHeight * 4);
    for (auto i = 0; i < imageWidth; i++)
    {
        for (auto j = 0; j < imageHeight; j++)
        {
            float         t = float(j) / float(imageWidth - 1);
            unsigned char r = (unsigned char)((1.0f - t) * 255);
            unsigned char g = (unsigned char)(t * 255);
            unsigned char b = 0;
            unsigned char a = 255;
            int           k = (i * imageWidth + j) * 4;

            data[k]     = r;
            data[k + 1] = g;
            data[k + 2] = b;
            data[k + 3] = r;

            k += 4;
        }
    }
    return data;
}
// 单通道渐变图像 (16x16, GL_RED)
std::vector<unsigned char> createGrayGradientImage(int w, int h)
{
    std::vector<unsigned char> data(w * h);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float         t     = float(x) / float(w - 1); // 左黑 -> 右白
            unsigned char value = (unsigned char)(t * 255);
            data[y * w + x]     = value;
        }
    }
    return data;
}
std::vector<unsigned char> createRGBGradientImage(int w, int h)
{
    std::vector<unsigned char> data(w * h * 3);
    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            float         t     = float(x) / float(w - 1); // 左黑 -> 右白
            unsigned char value = (unsigned char)(t * 255);

            auto k      = (y * w + x) * 3;
            data[k]     = value;
            data[k + 1] = value;
            data[k + 2] = value;
        }
    }
    return data;
}

} // namespace ResUtils

namespace RawData
{
void MSDFAtlas::reset() {
    glyphs.clear();
}
}

namespace Gpu
{

void ShdNode::buildGPURes()
{
}
void ShdNode::bindGPU()
{

    glUseProgram(program);

    for (auto i = 0; i < textures.size(); ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        glUniform1i(texLocs[i], i);
    }
}

void VertNode::buildBaseRes()
{
    auto&&     vs     = ResUtils::getVertsWithVEOSegN(1);
    VertVSNode vsNode = {2, 2, vs};
    vsNodes.push_back(vsNode);
    indices = ResUtils::getIndicesWithSegN(1);
    buildGPURes();
}

void VertNode::buildTexRes()
{
    auto&&     vs     = ResUtils::getVertsWithUVVEOSegN(1);
    VertVSNode vsNode = {4, 4, vs};
    vsNodes.push_back(vsNode);
    indices = ResUtils::getIndicesWithSegN(1);
    buildGPURes();
}

void VertNode::buildGPURes()
{

    if (vsNodes.empty())
        return;

    auto& node = vsNodes[0];
    auto& vs   = node.vs;

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vs[0]) * vs.size(), vs.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glVertexAttribPointer(0, node.componentSize, GL_FLOAT, GL_FALSE, node.componentStride * sizeof(vs[0]), nullptr);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &veo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

void VertNode::bindGPU()
{
    glBindVertexArray(vao);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, veo);
}

GLsizei VertNode::indicesSize() const
{
    return static_cast<GLsizei>(indices.size());
}
void VertNode::draw()
{
    auto isize = indicesSize();
    glDrawElements(GL_TRIANGLES, isize, GL_UNSIGNED_SHORT, 0);
}

void DrawingUnit::bindGPU()
{

    shader.bindGPU();
    vertex.bindGPU();
}

void DrawingUnit::draw()
{
    if (shader.program <= GL_ZERO)
        return;

    bindGPU();

    glEnable(GL_BLEND);
    glBlendFuncSeparate(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA,
        GL_ONE,
        GL_ONE_MINUS_SRC_ALPHA);

    mvp.append(objMat);

    glUniformMatrix3fv(shader.matrixLoc, 1, GL_FALSE, mvp.ptr());
    glUniform4fv(shader.colorLoc, 1, color.data());
    vertex.draw();
}


void buildBaseDrawUnit(DrawingUnit& unit)
{
    auto& shader     = unit.shader;
    shader.program   = ResUtils::createSahderProgram(ResUtils::vertShaderSource, ResUtils::fragShaderSource);
    shader.matrixLoc = glGetUniformLocation(shader.program, "u_matrix");
    shader.colorLoc  = glGetUniformLocation(shader.program, "u_color");

    auto& vert = unit.vertex;
    vert.buildBaseRes();
}

void buildTexDrawUnit(DrawingUnit& unit, const RawData::Image2DBytesData& imgData)
{
    auto& shader = unit.shader;

    shader.program   = ResUtils::createSahderProgram(ResUtils::vertTexSource, ResUtils::fragTexSource);
    shader.matrixLoc = glGetUniformLocation(shader.program, "u_matrix");
    shader.colorLoc  = glGetUniformLocation(shader.program, "u_color");
    auto texLoc      = glGetUniformLocation(shader.program, "u_tex0");
    shader.texLocs.push_back(texLoc);

    if (imgData.width < 1 || imgData.height < 1 || imgData.buffer.empty())
    {
        auto imgW = 4;
        auto imgH = 4;
        auto data = ResUtils::createRGBAImgBytes(imgW, imgH);
        auto tex  = ResUtils::createTextureFromImageBytes(imgW, imgH, data);
        shader.textures.push_back(tex);
    }
    else
    {
        auto tex = ResUtils::createTextureFromImageBytes(imgData.width, imgData.height, imgData.buffer);
        shader.textures.push_back(tex);
    }

    auto& vert = unit.vertex;
    vert.buildTexRes();
}

void buildRedFormatTexDrawUnit(DrawingUnit& unit, const RawData::Image2DBytesData& imgData)
{
    auto& shader = unit.shader;

    shader.program   = ResUtils::createSahderProgram(ResUtils::vertTexSource, ResUtils::fragRedFormatTexSource);
    shader.matrixLoc = glGetUniformLocation(shader.program, "u_matrix");
    shader.colorLoc  = glGetUniformLocation(shader.program, "u_color");
    auto texLoc      = glGetUniformLocation(shader.program, "u_tex0");
    shader.texLocs.push_back(texLoc);

    if (imgData.width < 1 || imgData.height < 1 || imgData.buffer.empty())
    {
        auto imgW         = 16;
        auto imgH         = 16;
        auto imgBytesData = ResUtils::createGrayGradientImage(imgW, imgH);
        auto tex          = ResUtils::createTextureFromImageBytes(imgW, imgH, imgBytesData, GL_RED, GL_RED, 1);
        shader.textures.push_back(tex);
    }
    else
    {
        auto tex = ResUtils::createTextureFromImageBytes(imgData.width, imgData.height, imgData.buffer, GL_RED, GL_RED, 1);
        shader.textures.push_back(tex);
    }

    auto& vert = unit.vertex;
    vert.buildTexRes();
}

void buildGlyphTexDrawUnit(DrawingUnit& unit, const RawData::TextGlyphData& glyphData)
{
    auto& imgData = glyphData.image;
    if (!glyphData.useSubpixel)
    {
        buildRedFormatTexDrawUnit(unit, imgData);
        return;
    }
    auto& shader = unit.shader;

    shader.program   = ResUtils::createSahderProgram(ResUtils::vertTexSource, ResUtils::fragRGBGlyphFormatTexSource);
    shader.matrixLoc = glGetUniformLocation(shader.program, "u_matrix");
    shader.colorLoc  = glGetUniformLocation(shader.program, "u_color");
    auto texLoc      = glGetUniformLocation(shader.program, "u_tex0");
    shader.texLocs.push_back(texLoc);
    if (imgData.width < 1 || imgData.height < 1 || imgData.buffer.empty())
    {
        auto imgW         = 16;
        auto imgH         = 16;
        auto imgBytesData = ResUtils::createRGBGradientImage(imgW, imgH);
        auto tex          = ResUtils::createTextureFromImageBytes(imgW, imgH, imgBytesData, GL_RGB, GL_RGB, 3);
        shader.textures.push_back(tex);
    }
    else
    {
        auto tex = ResUtils::createTextureFromImageBytes(imgData.width, imgData.height, imgData.buffer, GL_RGB, GL_RGB, 3);
        shader.textures.push_back(tex);
    }

    auto& vert = unit.vertex;
    vert.buildTexRes();
}
} // namespace Gpu
} // namespace Voxol::Test