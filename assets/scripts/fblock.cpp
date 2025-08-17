#include "sprites.h"
#include "../../engine/renderer.h"

// They eventually start to fire bullets
Vector2 blockSize = Vector2(6, 6);
Vector2 blockPos = Vector2(50, -blockSize.y);

float blockSpeed = defaultScrollSpeed;
const float blockMaxHealth = 3.f; // 1 smallShot kill
float blockHealth = blockMaxHealth;

// bool pointAwarded = false;

// Block Health

FBlock::FBlock(float moveSpeed, Vector2 size, Vector2 pos):blockSpeed(moveSpeed), blockSize(size) {
    blockSpeed = moveSpeed; // The Deeper you go the more random it gets use chance instead of random for more fun
    blockSize = size;
    blockHealth = blockMaxHealth;
    float boundaryOffset = getBoundaryOffset(blockSize.x*2);
    blockPos = pos == NULL ? Vector2(Math::getRandomFloat(boundaryOffset, 100.f - boundaryOffset), -blockSize.y) : pos;
    if ((int)Math::getRandomFloat(0.f, 1.2f) == 1) {
        float endPos = Math::getRandomFloat(10, 30);
        float distance = endPos - blockPos.y;
        blockTween = new Tween(&blockPos.y,  endPos, distance/blockSpeed *2.f, Tween::EASEOUTSINE);
    } else {
        float endPos = 100 + blockSize.y;
        float distance = endPos - blockPos.y;
        blockTween = new Tween(&blockPos.y,  endPos, distance/blockSpeed, Tween::EASEINQUAD);
    }
}

float FBlock::Damage(float amount) {
    if (blockHealth - amount <= 0) {
        blockTween->isStopped = true;
    }
    if (damageTimer < 0.02f && blockHealth > 0) {
        float damageDrawback = blockHealth;
        blockHealth -= amount;
        damageTimer = 0.04f;
        return damageDrawback;
    }
    return 0.f;
}

// Private
// float offsetTimer = 0.04f
// float damageTimer = 0.00f; // 0.04f seconds
void FBlock::_TimerProcess(float deltaTime) {
    if (!gameplay_running) {return;}
    if (offsetVTimer > 0) {
        offsetVTimer -= deltaTime;
    }
    if (damageTimer > 0) {
        damageTimer -= deltaTime;
    }
}

// Protected
void FBlock::_process(float deltaTime) {
    if (blockPos.y >= 100 + blockSize.y) {
        //Destroy
        blockTween->isStopped = true;
        Sprite::DestroyInstance(this);
        return;
    }

    if (blockTween->isStopped && blockPos.y < 33 && blockHealth > 0) {
        float endPos = 175 + blockSize.y;
        float distance = endPos - blockPos.y;
        blockTween = new Tween(&blockPos.y, endPos,distance/blockSpeed, Tween::EASEINBACK);
    }
    
    if (offsetVTimer <= 0.00f) {
        offsetVTimer = 0.06f;
        offsetVisual = Vector2(Math::getRandomFloat(-blockSize.x*0.05, blockSize.x*0.05) * buffer.aspectRatioInverse, Math::getRandomFloat(-blockSize.y*0.075, blockSize.y*0.075));
    }
    if (blockHealth > 0 || damageTimer > 0.02f) {
        drawTexture(blockPos + Vector2(0, blockSize.y), Vector2(blockSize.x, blockSize.y*2), "wind", &buffer, {false, false}, borderOffset *4);
        drawEllipse(blockPos + offsetVisual, blockSize, damageTimer <= 0.f ? 0xDE00ED : 0xFFFFFF, &buffer); // Colour change Damamge
    } else {
        Sprite::DestroyInstance(this);
    }
    _TimerProcess(deltaTime);
}