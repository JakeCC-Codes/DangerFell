#include "inputHandler.h"

std::set<u32> Input::activeVKeyCodes;
std::vector<Sprite*> Input::spriteRegistry;

int Input::IsKeyPressed(keyCode key) {
    for (u32 entry : activeVKeyCodes) {
        if (entry == key) {
            return true;
        }
    }
    return false;
}

VKeyEventType Input::VKeyEvent(keyCode vk_code, bool is_pressed) {
    if (is_pressed) {
        if (IsKeyPressed(vk_code)) {
            Input::Input_ButtonPressed(vk_code);
            return ButtonPressed;
        } else {
            Input::Input_ButtonDown(vk_code);
            activeVKeyCodes.insert(vk_code);
            Input::Input_ButtonPressed(vk_code);
            return ButtonDown;
        }
    }
    Input::Input_ButtonUp(vk_code);
    activeVKeyCodes.erase(vk_code);
    return ButtonUp;
}

void Input::Input_ButtonDown(keyCode vk_code) {
    std::vector<Sprite*> activeSprites;
    activeSprites.reserve(Input::spriteRegistry.size());
    for (Sprite* s : Input::spriteRegistry) {
        activeSprites.push_back(s);
    }
    for (Sprite* spriteInstance : activeSprites) {
        spriteInstance->Input_ButtonDown(vk_code);
    }
}
void Input::Input_ButtonUp(keyCode vk_code) {
    std::vector<Sprite*> activeSprites;
    activeSprites.reserve(Input::spriteRegistry.size());
    for (Sprite* s : Input::spriteRegistry) {
        activeSprites.push_back(s);
    }
    for (Sprite* spriteInstance : activeSprites) {
        spriteInstance->Input_ButtonUp(vk_code);
    }
}
void Input::Input_ButtonPressed(keyCode vk_code) {
    std::vector<Sprite*> activeSprites;
    activeSprites.reserve(Input::spriteRegistry.size());
    for (Sprite* s : Input::spriteRegistry) {
        activeSprites.push_back(s);
    }
    for (Sprite* spriteInstance : activeSprites) {
        spriteInstance->Input_ButtonPressed(vk_code);
    }
}

void Input::DestroySpriteInstance(Sprite *ptr) {
    auto it = std::remove(spriteRegistry.begin(), spriteRegistry.end(), ptr);
    spriteRegistry.erase(it, spriteRegistry.end());
}

int Input::InputAxis(keyCode neg, keyCode pos) {
    int negAxis = !IsKeyPressed(neg);
    int posAxis = !IsKeyPressed(pos);

    return (negAxis - posAxis);
}