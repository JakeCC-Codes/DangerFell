#include "sprites.h"
#include "../../engine/renderer.h"

Vector2 wallSize = Vector2(400, 11);
Vector2 wallPos = Vector2(50, -wallSize.y);
float wallMoveSpeed = defaultScrollSpeed;
float wallSpacing;
float wallOffset;

const float barrierMaxHealth = 6.f; // 2 smallShots kill
float barrierHealth = barrierMaxHealth;

// bool pointAwarded = false;

// Block Health
// Move Delay

Wall::Wall(float moveDelay, float blockChance, float spaceSize, float moveSpeed) {
    wallMoveSpeed = moveSpeed; // The Deeper you go the more random it gets use chance instead of random for more fun
    wallOffset = wallSize.x*(29.25f + spaceSize) * defaultAspectRatio * 0.01f;
    wallSpacing = 1.995f*wallOffset - wallSize.x;
    float boundaryOffset = getBoundaryOffset(wallSpacing);
    wallPos.x = Math::getRandomFloat(boundaryOffset, 100.f - boundaryOffset);
    barrierHealth = spaceSize < 4 && Math::getRandomFloat(0.f, 100.f) < blockChance ? barrierMaxHealth : 0.f; //30% Chance for a size <4 block to have a barrier
    if (moveDelay > 0.f) {
        moveTimer = moveDelay;
        wallMoveSpeed *= 3.f;
        wallPos.y = wallSize.y;
    } else {
        float endPos = 100 + wallSize.y;
        float distance = endPos - wallPos.y;
        new Tween(&wallPos.y,  endPos, (distance/wallMoveSpeed), Tween::EASEINOUTCUSTOM1);// Change me lol
        isMoving = false;
    }
}

float Wall::Damage(float amount) {
    if (damageTimer < 0.02f && barrierHealth > 0) {
        float damageDrawback = barrierHealth;
        barrierHealth -= amount;
        damageTimer = 0.04f;
        return damageDrawback;
    }
    return 0.f;
}

// Private
// float moveTimer = 0.00f;
// float damageTimer = 0.00f; // 0.04f seconds
void Wall::_TimerProcess(float deltaTime) {
    if (!gameplay_running) {return;}
    if (moveTimer > 0) {
        moveTimer -= deltaTime;
    }
    if (damageTimer > 0) {
        damageTimer -= deltaTime;
    }
}

// Protected
void Wall::_process(float deltaTime) {
    if (wallPos.y >= 100 + wallSize.y) {
        //Destroy
        Sprite::DestroyInstance(this);
        return;
    }
    
    wallPos.y += wallMoveSpeed * deltaTime * (moveTimer <= 0.f && gameplay_running && isMoving);
    drawRect(wallPos - Vector2(wallOffset * buffer.aspectRatioInverse), wallSize, 0x600026, &buffer);
    drawRect(wallPos + Vector2(wallOffset * buffer.aspectRatioInverse), wallSize, 0x600026, &buffer);
    if (barrierHealth > 0 || damageTimer > 0.02f) {
        drawRect(wallPos, Vector2(wallSpacing, wallSize.y+3), damageTimer <= 0.f ? 0xDE00ED : 0xFFFFFF, &buffer); // Colour change Damamge
    }
    _TimerProcess(deltaTime);
}