#pragma once
#include "../../engine/util.h"
#include "../../engine/utilClasses.h"


enum BulletTypes { customType, smallShotType, midShotType, bigShotType };
struct BulletType { float damage, speed; BulletTypes type=customType; };
const BulletType smallShot = {3.f, 350.f, smallShotType};
const BulletType midShot = {6.f, 225.f, midShotType};
const BulletType bigShot = {14.f, 225.f, bigShotType};

class Player : public Sprite {
    protected:
    void _process(float deltaTime) override;

    public:
    Vector2 playerSize = Vector2(9, 12);
    Vector2 playerPos = Vector2(50, 93.5);
    const Vector2 playerSizeOriginal = playerSize;
    float bulletSizeModifier = 0;
    float bulletMaxSize = 4.75f;
    Vector2 Velocity = 0.f;

    std::string getTypeID() const override { return "dangerfell:player"; }
    Player(Vector2 pos = Vector2(50, 93.5), Vector2 size = Vector2(9, 12));

    void Input_ButtonDown(keyCode vk_code) override;
    void Input_ButtonUp(keyCode vk_code) override;

    private:
    float resizeTimer = 0.00f;
    inline void _TimerProcess(float deltaTime);
    void _CollisionCheck();
    void _Death();
};

class Bullet : public Sprite {
    public:
    Vector2 bulletPos = Vector2(50, 100);
    Vector2 bulletSize = Vector2(2, 5);
    float bulletSpeed = 250;
    float bulletDamage = 14;
    float bulletMaxDamage = 14;
    BulletTypes bulletType = customType;

    std::string getTypeID() const override { return "dangerfell:bullet"; }
    Bullet(Vector2 pos, Vector2 size = Vector2(2, 5), float speed = 250, float damage = 14.f, BulletTypes type = customType);

    private:
    Vector2 offsetVisual;
    float offsetVTimer = 0.00f;
    void _CollisionCheck();

    protected:
    void _process(float deltaTime) override;
};


class Wall : public Sprite {
    public:
    Vector2 wallSize = Vector2(400, 11);
    Vector2 wallPos = Vector2(50, -wallSize.y);
    float wallMoveSpeed = defaultScrollSpeed;
    float wallSpacing;
    float wallOffset;
    const float barrierMaxHealth = 6.f; // 2 smallShots kill
    float barrierHealth = barrierMaxHealth;
    bool pointAwarded = false;

    // Block Health
    // Move Delay
    string getTypeID() const override { return "dangerfell:wall"; }
    string getCollisionLayer() const override { return "dangerfell:kill_layer"; }
    Wall(float moveDelay = 0.f, float blockChance = 60.f, float spaceSize = Math::clamp(2.f, Math::getRandomFloat(0.75f, 10.75f), 7.f), float moveSpeed = defaultScrollSpeed);
    float Damage(float amount);

    private:
    bool isMoving = true; // Fix
    float moveTimer = 0.00f;
    float damageTimer = 0.00f; // 0.04f seconds
    inline void _TimerProcess(float deltaTime);
    
    protected:
    void _process(float deltaTime) override;
};

class FBlock : public Sprite {
    public:
    Vector2 blockSize = Vector2(6, 6);
    Vector2 blockPos = Vector2(50, -blockSize.y);
    float blockSpeed = defaultScrollSpeed;
    const float blockMaxHealth = 3.f; // 1 smallShot kill
    float blockHealth = blockMaxHealth;
    bool pointAwarded = false;

    // Block Health
    // Move Delay
    string getTypeID() const override { return "dangerfell:falling_block"; }
    string getCollisionLayer() const override { return "dangerfell:kill_layer"; }
    FBlock(float moveSpeed = Math::getRandomFloat(defaultScrollSpeed*0.4f, defaultScrollSpeed*0.6f), Vector2 size = Vector2(6, 6), Vector2 pos = NULL);
    float Damage(float amount);

    private:
    Tween* blockTween;
    Vector2 offsetVisual;
    float offsetVTimer = 0.00f;
    float damageTimer = 0.00f; // 0.04f seconds
    float accel = 1.f;
    inline void _TimerProcess(float deltaTime);
    
    protected:
    void _process(float deltaTime) override;
};