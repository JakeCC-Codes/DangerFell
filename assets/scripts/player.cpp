#include "sprites.h"
#include "../../engine/renderer.h"
#include "../../engine/inputHandler.h"

Vector2 playerSize = Vector2(9, 12);
Vector2 playerPos = Vector2(50, 93.5);
const Vector2 playerSizeOriginal = playerSize;
float bulletSizeModifier = 0;
float bulletMaxSize = 4.75f;

Vector2 Velocity = 0.f;
// The time it takes for the player to regen ammo is a variable,
// use it to regen ammo faster once smaller shots are shot vs
// bigger ones, except when out of ammo completely.

Player::Player(Vector2 pos, Vector2 size):playerPos(pos), playerSize(size), playerSizeOriginal(size) { Input::spriteRegistry.push_back(this); };

void Player::_process(float deltaTime) {
    float speed = 120.f;
    Input::IsKeyPressedLambda([&speed, this](u32 keyCode) {
        switch(keyCode) {
            case VK_SPACE:
            case 'Z': {
                if (playerSize.x > playerSizeOriginal.x*0.45) speed *= (bulletMaxSize - bulletSizeModifier*0.3f)/bulletMaxSize;
                resizeTimer = 0.54f;
                bulletSizeModifier += 0.15f;
                bulletSizeModifier = Math::clamp(0, bulletSizeModifier, bulletMaxSize);
            } break;
            case 'X': {speed = 240.f;} break;
        }
    });
    float hInput = Input::InputAxis(VK_LEFT, VK_RIGHT);
    Velocity += Math::lerp(Vector2(hInput, Math::clamp(-1.f, Input::InputAxis(VK_DOWN, VK_UP), 0.5f) *1.3f), Velocity, (hInput ? 3.f : 5.6f));
    playerPos += Velocity * deltaTime * speed;
    playerPos = Math::clampVector2(Vector2(getBoundaryOffset(playerSize.x), 7.5f), playerPos, Vector2(100 - getBoundaryOffset(playerSize.x), 93.5f));
    if (playerSize.x > playerSizeOriginal.x*0.45) {
        drawRect(playerPos + ShakeOffsetFX(playerSize*bulletSizeModifier*0.005f, deltaTime, Sprite::shakeDebounce, Sprite::shakeOffset, -bulletSizeModifier*0.01f), playerSize, 0xE04000, &buffer);
    } else {
        drawRect(playerPos, playerSize, 0xD94505, &buffer);
    }
    playerSize.x += Math::lerp(playerSizeOriginal.x, playerSize.x, 5.f) * (resizeTimer <= 0.00f);
    _CollisionCheck();
    _TimerProcess(deltaTime);
}
void Player::_TimerProcess(float deltaTime) {
    if (!gameplay_running) {return;}
    if (resizeTimer > 0) {
        resizeTimer -= deltaTime;
    }
}

void Player::Input_ButtonDown(keyCode vk_code) {
    switch(vk_code) {
        case VK_SPACE:
        case 'Z': {
            bulletSizeModifier = 0;
            playerSize.x *= 0.75f;
            playerSize.x = Math::clamp(playerSizeOriginal.x *0.4f, playerSize.x, playerSizeOriginal.x);
        } break;
        case 'R': {
            Sprite::registry.push_back(std::make_unique<FBlock>());
        } break;
        case 'T': {
            Sprite::registry.push_back(std::make_unique<Wall>(2.f));
        } break;
        case 'Y': {
            gameplay_running = !gameplay_running;
        } break;
    }
}

void Player::Input_ButtonUp(keyCode vk_code) { //When shoot you shake character and squash and stretch when shooting
    switch(vk_code) { // Known bug when holding down 2 keys you don't reset timer
        case VK_SPACE:
        case 'Z': {
            bulletSizeModifier = Math::round(bulletSizeModifier, bulletMaxSize*0.5f);
            BulletType bulletStats = bulletSizeModifier >= bulletMaxSize ? bigShot : (bulletSizeModifier >= bulletMaxSize*0.5f ? midShot : smallShot);
            if (playerSize.x > playerSizeOriginal.x*0.45f) {
                Sprite::registry.insert(Sprite::registry.begin(), std::make_unique<Bullet>(playerPos, Vector2(2 + bulletSizeModifier, 5 + bulletSizeModifier), bulletStats.speed, bulletStats.damage, bulletStats.type));
                playerSize.x *= 1.25f;
            } else {
                playerSize.x *= 1.1f;
            }
            bulletSizeModifier = 0;
            playerSize.x = Math::clamp(playerSizeOriginal.x *0.4f, playerSize.x, playerSizeOriginal.x);
        } break;
    }
}

void Player::_CollisionCheck() {
    for (const std::unique_ptr<Sprite>& sprite : Sprite::registry) {
        if (sprite->getCollisionLayer() != "dangerfell:kill_layer") {continue;}
        Vector2 realPlayerSize = Vector2(playerSize.x, playerSize.y *2);
        if (sprite->getTypeID() == "dangerfell:wall") {
            Wall* wall = dynamic_cast<Wall*>(sprite.get());
            Vector2 offset = Vector2(wall->wallOffset * buffer.aspectRatioInverse);

            if (AABBCollision(playerPos, realPlayerSize, wall->wallPos - offset, wall->wallSize)
                || AABBCollision(playerPos, realPlayerSize, wall->wallPos + offset, wall->wallSize)) {
                _Death();
                break;
            } else if (AABBCollision(playerPos, realPlayerSize, wall->wallPos, Vector2(wall->wallSpacing, wall->wallSize.y))) {
                if (wall->barrierHealth > 0) {
                    _Death();
                    break;
                } else if (!wall->pointAwarded) {
                    wall->pointAwarded = true;
                    GameLoop::score++;
                }
            }
            
        } else if (sprite->getTypeID() == "dangerfell:falling_block") {
            FBlock* fBlock = dynamic_cast<FBlock*>(sprite.get());
            if (AABBCollision(playerPos, Vector2(realPlayerSize.x, realPlayerSize.y *0.75f), fBlock->blockPos, Vector2(fBlock->blockSize.x * buffer.aspectRatio, fBlock->blockSize.y))) {
                _Death();
                break;
            }
        }
    }
}

void Player::_Death() {
    // Sound Effect
    gameplay_running = false;
    std::cout<<GameLoop::score<<'\n';
    Sprite::DestroyInstance(this);
    Input::DestroySpriteInstance(this);
}