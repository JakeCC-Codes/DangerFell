#include "sprites.h"
#include "../../engine/renderer.h"


Vector2 bulletPos = Vector2(50, 100);
Vector2 bulletSize = Vector2(2, 5);
float bulletSpeed = 250;
float bulletDamage = 14;
float bulletMaxDamage = 14;

Bullet::Bullet(Vector2 pos, Vector2 size, float speed, float damage, BulletTypes type):bulletPos(pos), bulletSize(size), bulletSpeed(speed), bulletDamage(damage), bulletMaxDamage(damage), bulletType(type){};

void Bullet::_CollisionCheck() {
    for (const std::unique_ptr<Sprite>& sprite : Sprite::registry) {
        if (sprite->getCollisionLayer() != "dangerfell:kill_layer") {continue;}
        if (sprite->getTypeID() == "dangerfell:wall") {
            Wall* wall = dynamic_cast<Wall*>(sprite.get());
            Vector2 offset = Vector2(wall->wallOffset * buffer.aspectRatioInverse);
            if (AABBCollision(bulletPos, bulletSize, wall->wallPos, Vector2(wall->wallSpacing, wall->wallSize.y+3))) {
                float damageDrawback = wall->Damage(bulletDamage);
                if (damageDrawback > 0) {
                    bulletDamage = bulletDamage - damageDrawback;
                    break;
                }
            } else if (AABBCollision(bulletPos, bulletSize, wall->wallPos - offset, wall->wallSize)
                        || AABBCollision(bulletPos, bulletSize, wall->wallPos + offset, wall->wallSize)) {
                Sprite::DestroyInstance(this);
                break;
                // Sound Effect
            }

        } else if (sprite->getTypeID() == "dangerfell:falling_block") {
            FBlock* fBlock = dynamic_cast<FBlock*>(sprite.get());
            if (AABBCollision(bulletPos, Vector2(bulletSize.x * buffer.aspectRatio, bulletSize.y*1.75f), fBlock->blockPos, Vector2(fBlock->blockSize.x * buffer.aspectRatio, fBlock->blockSize.y))) {
                float damageDrawback = fBlock->Damage(bulletDamage);
                if (damageDrawback > 0) {
                    bulletDamage = bulletDamage - damageDrawback;
                    break;
                }
            }
        }
    }
}

void Bullet::_process(float deltaTime) {
    if (bulletPos.y <= 0 || bulletDamage <= 0) {
        //Destroy
        Sprite::DestroyInstance(this);
        return;
    }

    bulletPos += Vector2(0, -1) * bulletSpeed * deltaTime * gameplay_running;
    switch (bulletType) {
        case smallShotType:
            drawTexture(bulletPos, bulletSize, "bullet", &buffer);
        break;

        default:
            drawRect(bulletPos + ShakeOffsetFX(Vector2(bulletSize.x*0.05, bulletSize.y*0.05), deltaTime, Sprite::shakeDebounce, Sprite::shakeOffset), bulletSize * (bulletDamage/bulletMaxDamage), 0xE040FF, &buffer);
        break;
    }
    _CollisionCheck();
}