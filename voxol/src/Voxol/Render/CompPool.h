#ifndef VOXOL_RENDER_COMP_POOL_H
#define VOXOL_RENDER_COMP_POOL_H

#include <vector>
#include <cstdint>
#include <type_traits>
#include <cassert>

namespace Voxol::Render
{
// 只需要获取到 index, 外部引用也只能记录对应的index
template <typename T>
class CompPool
{
    static_assert(std::is_trivially_copyable_v<T> && std::is_standard_layout_v<T>,
              "T must be a trivially copyable, standard layout type");

public:
    CompPool() = default;
    ~CompPool() = default;

    void initialize(size_t initialCapacity = 512)
    {
        reserve(initialCapacity);
    }

    // 禁止拷贝，但允许移动
    CompPool(const CompPool&)                = delete;
    CompPool& operator=(const CompPool&)     = delete;
    CompPool(CompPool&&) noexcept            = default;
    CompPool& operator=(CompPool&&) noexcept = default;

    // 预分配空间
    void reserve(size_t capacity)
    {
        if (capacity > comps.size())
        {
            size_t oldSize = comps.size();
            comps.resize(capacity);
            m_usedList.resize(capacity);
            m_freeList.reserve(capacity);
            for (size_t i = oldSize; i < capacity; ++i)
                m_freeList.push_back(static_cast<uint32_t>(i));
        }
    }

    // 分配对象（未初始化）
    int32_t allocate()
    {
        if (m_freeList.empty())
            reserve(comps.size() * 2 + 1);

        uint32_t idx = m_freeList.back();
        m_freeList.pop_back();
        m_usedList[idx] = true;
        ++m_activeCount;

        return idx;
    }
    template <typename Fn>
    void forEach(Fn&& fn) noexcept
    {
        for (auto& obj : comps)
        {
            fn(obj);
        }
    }
    // 分配并赋值（构造）
    template <typename... Args>
    int32_t emplace(Args&&... args)
    {
        auto index           = allocate();
        comps[index] = T{std::forward<Args>(args)...};
        return index;
    }
    T& operator[](int32_t index)
    {
        return comps[index];
    }
    const T& operator[](int32_t index) const
    {
        return comps[index];
    }
    // 通过句柄访问对象（返回指针或nullptr）
    T& get(int32_t index)
    {
        return comps[index];
    }
    const T& get(int32_t index) const
    {
        return comps[index];
    }

    int32_t acquire() {

        if (m_freeList.empty())
            return -1;

        uint32_t idx = m_freeList.back();
        m_freeList.pop_back();
        m_usedList[idx] = true;
        return idx;
    }

    void release(int32_t index) {

        if (!isValid(index) || !m_usedList[index])
            return;
        m_usedList[index] = true;
        m_freeList.push_back(index);
    }
    // 清空所有对象
    void reset()
    {
        m_freeList.clear();
        for (uint32_t i = 0; i < comps.size(); ++i)
        {
            m_freeList.push_back(i);
            m_usedList[i] = false;
        }
        m_activeCount = 0;
    }

    [[nodiscard]] bool hasFree(int32_t index) const noexcept
    {
        return !m_freeList.empty();
    }
    // 判断句柄是否有效
    [[nodiscard]] bool isValid(int32_t index) const noexcept
    {
        return index >= 0 && index < comps.size();
    }

    [[nodiscard]] size_t activeCount() const noexcept { return m_activeCount; }
    [[nodiscard]] size_t capacity() const noexcept { return comps.size(); }

private:
    std::vector<T>        comps;
    std::vector<bool>    m_usedList;
    std::vector<uint32_t> m_freeList;
    size_t                m_activeCount = 0;
};
}
#endif