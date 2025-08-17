#pragma once
#include "util.h"
#include "utilClasses.h"
#include <set>

enum VKeyEventType {
    ButtonDown,
    ButtonUp,
    ButtonPressed
};

class Input {
    public:
    static std::vector<Sprite*> spriteRegistry;
    static VKeyEventType VKeyEvent(keyCode vk_code, bool is_pressed);

    static void Input_ButtonDown(keyCode vk_code);
    static void Input_ButtonUp(keyCode vk_code);
    static void Input_ButtonPressed(keyCode vk_code);

    static int InputAxis(keyCode neg, keyCode pos);
    static int IsKeyPressed(keyCode key);
    template <class L> inline static void IsKeyPressedLambda(L lambda) {
        for (u32 entry : activeVKeyCodes) {
            lambda(entry);
        }
    }

    static void DestroySpriteInstance(Sprite* ptr);

    private:
    static std::set<u32> activeVKeyCodes;
    static bool register_entries;
};