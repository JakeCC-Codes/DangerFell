#pragma once
#include "../../engine/utilClasses.h"
#include "sprites.h"

class Level1 : public GameLoop {
    public:
    std::string getTypeID() const override { return "dangerfell:level1"; };
    std::array<int, 2> getScoreRange() const override { return {0, 63};}

    protected:
    void _ready() override;
    void _process(float deltaTime) override;
    
    private:
    float normalWallTimer = 0.00f; // 0.04f seconds
    float timedWallTimer = 1.00f;
    float fBlockTimer = 0.01f; // 0.04f seconds
    void _TimerProcess(float deltaTime);
    void _CreateWall(float sec = 0.00f);
};