#include "levels.h"
#include "../../engine/util.h"

float normalWallTimer = 0.00f; // 0.04f seconds
float timedWallTimer = 1.00f;
float fBlockTimer = 0.01f; // 0.04f seconds

void Level1::_ready() {
}

void Level1::_process(float deltaTime) {
    float offset = score * 0.01f;
    if (normalWallTimer <= 0.00f) {
        normalWallTimer = Math::getRandomFloat(Math::clamp(0.3f, 1.15f - offset, 2.f), 1.7f - offset);
        _CreateWall();
    }
    if (timedWallTimer <= 0.00f) {
        timedWallTimer = Math::getRandomFloat(4.f - offset, 14.f - offset);
        _CreateWall(Math::getRandomFloat(3.f, 3.5f));
    }
    if (fBlockTimer <= 0.00f) {
        fBlockTimer = Math::getRandomFloat(4.f - offset*2, 7.f - offset*2) * (int)((Math::getRandomFloat(0.f, 1.f) >= 0.4f || score < 20) + 0.3f);
        Sprite::registry.push_back(std::make_unique<FBlock>());
    }
    _TimerProcess(deltaTime);
}

void Level1::_TimerProcess(float deltaTime) {
    if (!gameplay_running) {return;}
    if (normalWallTimer > 0) {
        normalWallTimer -= deltaTime;
    }
    if (score > 33 && timedWallTimer > 0) {
        timedWallTimer -= deltaTime;
    }
    if (score > 9 && fBlockTimer > 0) {
        fBlockTimer -= deltaTime;
    }
}

void Level1::_CreateWall(float sec) {
    Sprite::registry.push_back(std::make_unique<Wall>(sec, 60.f * (score > 6)));
}