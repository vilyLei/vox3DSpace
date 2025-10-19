#ifndef VOXOL_SYSTEM_SHOTCUT_MANAGER_H
#define VOXOL_SYSTEM_SHOTCUT_MANAGER_H

#include <glfw3.h>
#include <unordered_map>
#include <functional>
#include <string>
#include <cstdint>
#include <iostream>
#include <algorithm>

namespace Voxol::System
{

// ------------------------------
// Key 修饰符位掩码定义
// ------------------------------
enum KeyMod : uint32_t {
    MOD_CTRL  = 1 << 0,
    MOD_SHIFT = 1 << 1,
    MOD_ALT   = 1 << 2,
};

// ------------------------------
// 组合键描述结构
// ------------------------------
struct KeyCombo {
    uint32_t mods = 0; // Ctrl / Shift / Alt
    int mainKey = 0;   // 主键（GLFW key code）

    bool operator==(const KeyCombo& o) const noexcept {
        return mods == o.mods && mainKey == o.mainKey;
    }
};

// 自定义哈希
struct KeyComboHash {
    std::size_t operator()(const KeyCombo& c) const noexcept {
        return (static_cast<size_t>(c.mainKey) << 3) ^ c.mods;
    }
};

// ------------------------------
// 快捷键管理器
// ------------------------------
class ShortcutManager {
public:
    using Callback = std::function<void()>;

    void registerShortcut(const std::string& comboStr, Callback cb) {
        KeyCombo combo = parseCombo(comboStr);
        m_shortcuts[combo] = std::move(cb);
        std::cout << "[ShortcutManager] Registered: " << comboStr << " (key=" 
                  << combo.mainKey << ", mods=" << combo.mods << ")\n";
    }

    // GLFW 事件输入
    void onKeyEvent(int key, int scancode, int action, int mods) {
        if (action != GLFW_PRESS) return;

        uint32_t myMods = 0;
        if (mods & GLFW_MOD_CONTROL) myMods |= MOD_CTRL;
        if (mods & GLFW_MOD_SHIFT)   myMods |= MOD_SHIFT;
        if (mods & GLFW_MOD_ALT)     myMods |= MOD_ALT;

        KeyCombo combo{ myMods, key };
        auto it = m_shortcuts.find(combo);
        if (it != m_shortcuts.end()) {
            it->second();
        }
    }

private:
    KeyCombo parseCombo(const std::string& str) {
        KeyCombo combo{};
        std::string s = str;
        std::transform(s.begin(), s.end(), s.begin(), ::toupper);
        s.erase(std::remove_if(s.begin(), s.end(), ::isspace), s.end());

        if (s.find("CTRL+") != std::string::npos) {
            combo.mods |= MOD_CTRL;
            s.erase(s.find("CTRL+"), 5);
        }
        if (s.find("SHIFT+") != std::string::npos) {
            combo.mods |= MOD_SHIFT;
            s.erase(s.find("SHIFT+"), 6);
        }
        if (s.find("ALT+") != std::string::npos) {
            combo.mods |= MOD_ALT;
            s.erase(s.find("ALT+"), 4);
        }

        // 主键
        if (s.size() == 1 && s[0] >= 'A' && s[0] <= 'Z') {
            combo.mainKey = GLFW_KEY_A + (s[0] - 'A');
        } else if (s.size() == 1 && s[0] >= '0' && s[0] <= '9') {
            combo.mainKey = GLFW_KEY_0 + (s[0] - '0');
        } else {
            // 可扩展更多键名
            if (s == "SPACE") combo.mainKey = GLFW_KEY_SPACE;
            else if (s == "ENTER") combo.mainKey = GLFW_KEY_ENTER;
            else if (s == "TAB") combo.mainKey = GLFW_KEY_TAB;
            else if (s == "ESC" || s == "ESCAPE") combo.mainKey = GLFW_KEY_ESCAPE;
        }
        return combo;
    }

private:
    std::unordered_map<KeyCombo, Callback, KeyComboHash> m_shortcuts;
};

}
#endif