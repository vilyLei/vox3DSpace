#ifndef VOXOL_SYSTEM_SHOTCUT_MANAGER_H
#define VOXOL_SYSTEM_SHOTCUT_MANAGER_H

#include <glfw3.h>
#include <unordered_map>
#include <functional>
#include <string>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <chrono>
#include <set>
#include <vector>

namespace Voxol::System
{

struct KeyCombo
{
    std::vector<int> keys;

    KeyCombo() = default;
    KeyCombo(std::initializer_list<int> ks) :
        keys(ks)
    {
        std::sort(keys.begin(), keys.end());
    }
    KeyCombo(const std::vector<int>& ks) :
        keys(ks) {}

    bool operator==(const KeyCombo& other) const noexcept
    {
        return keys == other.keys;
    }
};

struct KeyComboHash
{
    std::size_t operator()(const KeyCombo& combo) const noexcept
    {
        std::size_t h = 0;
        for (int k : combo.keys)
            h ^= std::hash<int>()(k) + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
        return h;
    }
};

using Callback = std::function<void()>;

class ShortcutManager
{
public:
    enum class TriggerType
    {
        Press,
        LongPress,
        Release
    };

    struct Shortcut
    {
        KeyCombo                              combo;
        Callback                              callback;
        TriggerType                           trigger;
        bool                                  active = false;
        std::chrono::steady_clock::time_point pressTime;
    };

    using Listener = std::function<void(const KeyCombo&, TriggerType)>;

    void registerShortcut(const KeyCombo& combo, Callback cb, TriggerType trigger = TriggerType::Press)
    {
        Shortcut s{combo, std::move(cb), trigger};
        m_shortcuts[combo] = std::move(s);
    }

    void addListener(Listener listener)
    {
        m_listeners.push_back(std::move(listener));
    }

    void handleKeyEvent(int key, int scancode, int action, int mods)
    {
        using namespace std::chrono;

        if (action == GLFW_PRESS)
        {
            m_pressed.insert(key);
            checkPressTrigger();
        }
        else if (action == GLFW_RELEASE)
        {
            m_pressed.erase(key);
            checkReleaseTrigger();
        }
    }

    void update()
    {
        using namespace std::chrono;

        auto now = steady_clock::now();

        for (auto& [combo, shortcut] : m_shortcuts)
        {
            if (shortcut.trigger == TriggerType::LongPress && shortcut.active)
            {
                auto elapsed = duration_cast<milliseconds>(now - shortcut.pressTime).count();
                if (elapsed >= m_longPressThreshold && !m_longPressTriggered[combo])
                {
                    shortcut.callback();
                    notify(combo, TriggerType::LongPress);
                    m_longPressTriggered[combo] = true;
                }
            }
        }
    }

    void setLongPressThreshold(int ms) { m_longPressThreshold = ms; }

private:
    void checkPressTrigger()
    {
        std::vector<int> sortedKeys(m_pressed.begin(), m_pressed.end());
        std::sort(sortedKeys.begin(), sortedKeys.end());
        KeyCombo combo{sortedKeys};

        auto it = m_shortcuts.find(combo);
        if (it != m_shortcuts.end())
        {
            auto& shortcut              = it->second;
            shortcut.active             = true;
            shortcut.pressTime          = std::chrono::steady_clock::now();
            m_longPressTriggered[combo] = false;

            if (shortcut.trigger == TriggerType::Press)
            {
                shortcut.callback();
                notify(combo, TriggerType::Press);
            }
        }
    }

    void checkReleaseTrigger()
    {
        std::vector<int> sortedKeys(m_pressed.begin(), m_pressed.end());
        std::sort(sortedKeys.begin(), sortedKeys.end());
        KeyCombo combo{sortedKeys};

        for (auto& [k, shortcut] : m_shortcuts)
        {
            if (shortcut.active && shortcut.trigger == TriggerType::Release)
            {
                shortcut.active = false;
                shortcut.callback();
                notify(k, TriggerType::Release);
            }
        }
    }

    void notify(const KeyCombo& combo, TriggerType type)
    {
        for (auto& listener : m_listeners)
        {
            listener(combo, type);
        }
    }

private:
    std::unordered_map<KeyCombo, Shortcut, KeyComboHash> m_shortcuts;
    std::unordered_map<KeyCombo, bool, KeyComboHash>     m_longPressTriggered;
    std::vector<Listener>                                m_listeners;
    std::set<int>                                        m_pressed;
    int                                                  m_longPressThreshold = 600; // ms
};


}
#endif