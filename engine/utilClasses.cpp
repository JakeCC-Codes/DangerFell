#include "utilClasses.h"
#include "inputHandler.h"
#include "../assets/scripts/sprites.h"
#include "../assets/scripts/levels.h"

std::vector<Tween*> Tween::registry;
std::vector<ImageFile*> ImageFile::registry;
std::vector<std::unique_ptr<Sprite>> Sprite::registry;
std::vector<std::unique_ptr<GameLoop>> GameLoop::registry;
int GameLoop::score = 0;
int GameLoop::cacheReadyScore = 0;

void ImageFile::Destroy() {
    stbi_image_free(this->data);
    auto it = std::remove(ImageFile::registry.begin(), ImageFile::registry.end(), this);
    ImageFile::registry.erase(it, ImageFile::registry.end());
}

bool Sprite::register_entries = []() {
    // Put registries here
    Sprite::registry.push_back(std::make_unique<Player>());

    
    return true;
}();

bool Input::register_entries = []() {
    // Put init registries here

    
    return true;
}();

bool GameLoop::register_entries = []() {
    // Put registries here
    GameLoop::registry.push_back(std::make_unique<Level1>());
    
    return true;
}();

const std::map<Tween::TweenMode, std::function<float(float)>> Tween::tweenMap = {
    //-- https://easings.net/ --//
    { Tween::LINEAR, [](float x) {return x;} },
    { Tween::EASEINSINE, [](float x) {return 1 - std::cos((x * Math::PI) /2);} },
    { Tween::EASEOUTSINE, [](float x) {return std::sin((x * Math::PI) /2);} },
    { Tween::EASEINOUTSINE, [](float x) {return -(std::cos(Math::PI * x) - 1) /2;} },
    { Tween::EASEINQUAD, [](float x) {return x * x;} },
    { Tween::EASEOUTQUAD, [] (float x) {return 1 - (1 - x) * (1 - x);} },
    { Tween::EASEINOUTQUAD, [](float x) {return x < 0.5f ? 2 * x * x : 1 - Math::powLITE(-2*x + 2, 2) /2;} },
    { Tween::EASEINCUBIC, [](float x) {return x * x * x;} },
    { Tween::EASEOUTCUBIC, [](float x) {return 1 - Math::powLITE(1 - x, 3);} },
    { Tween::EASEINOUTCUBIC, [](float x) {return x < 0.5f ? 4 * x * x * x : 1 - Math::powLITE(-2*x + 2, 3) /2;} },
    { Tween::EASEINQUART, [](float x) {return x * x * x * x;} },
    { Tween::EASEOUTQUART, [](float x) {return 1 - Math::powLITE(1 - x, 4);} },
    { Tween::EASEINOUTQUART, [](float x) {return x < 0.5f ? 8 * x * x * x * x : 1 - Math::powLITE(-2*x + 2, 4) /2;} },
    { Tween::EASEINQUINT, [](float x) {return x * x * x * x * x;} },
    { Tween::EASEOUTQUINT, [](float x) {return 1 - Math::powLITE(1 - x, 5);} },
    { Tween::EASEINOUTQUINT, [](float x) {return x < 0.5f ? 16 * x * x * x * x * x : 1 - Math::powLITE(-2*x + 2, 5) /2;} },
    { Tween::EASEINEXPO, [](float x) {return x == 0 ? 0 : Math::powLITE(2, 10*x - 10);} },
    { Tween::EASEOUTEXPO, [](float x) {return x == 1 ? 1 : 1 - Math::powLITE(2, -10*x);} },
    { Tween::EASEINOUTEXPO, [] (float x) {return (x == 0 ? 0 : (x == 1 ? 1 : (x < 0.5f ? std::pow(2, 20*x - 10) /2 : (2 - std::pow(2, -20*x + 10)) /2)));} },
    { Tween::EASEINCIRC, [](float x) {return 1 - std::sqrt(1 - x*x);} },
    { Tween::EASEOUTCIRC, [](float x) {return std::sqrt(1 - Math::powLITE(x - 1, 2));} },
    { Tween::EASEINOUTCIRC, [](float x) {return x < 0.5f ? (1 - std::sqrt(1 - Math::powLITE(2*x, 2))) /2 : (std::sqrt(1 - Math::powLITE(-2*x + 2, 2)) + 1) /2;} },
    { Tween::EASEINBACK, [](float x) {const float c1 = 1.70158f; return (c1 + 1.f) * x * x * x - c1 * x * x;} },
    { Tween::EASEOUTBACK, [](float x) {const float c1 = 1.70158f; return 1 + (c1 + 1.f) * Math::powLITE(x - 1, 3) + c1 * Math::powLITE(x - 1, 2);} },
    { Tween::EASEINOUTBACK, [](float x) {const float c1 = 1.70158f; const float c2 = c1 * 1.525f;
        return x < 0.5f ? (Math::powLITE(2*x, 2) * ((c2 + 1) * 2 * x - c2)) /2 : (Math::powLITE(2*x - 2, 2) * ((c2 + 1) * (x * 2 - 2) + c2) + 2) /2;}
    },
    { Tween::EASEINELASTIC, [](float x) {return x == 0 ? 0 : x == 1 ? 1 : -Math::powLITE(2, 10*x - 10) * std::sin((x * 10 - 10.75f) * ((2*Math::PI)/3));} },
    { Tween::EASEOUTELASTIC, [](float x) {return x == 0 ? 0 : x == 1 ? 1 : Math::powLITE(2, -10*x) * std::sin((x * 10 - 0.75f) * ((2*Math::PI)/3)) + 1;} },
    { Tween::EASEINOUTELASTIC, [](float x) {const float c5 = (2*Math::PI)/4.5f; 
        return x == 0 ? 0 : x == 1 ? 1 : x < 0.5f ? -(Math::powLITE(2, 20*x - 10) * std::sin((20*x - 11.125f)*c5))/2 : (Math::powLITE(2, -20*x + 10) * std::sin((20*x - 11.125f)*c5))/2 + 1;}
    },
    { Tween::EASEINBOUNCE, [](float x) {return 1 - tweenMap.at(Tween::EASEOUTBOUNCE)(1 - x);} },
    { Tween::EASEOUTBOUNCE, [](float x) {
        const float n1 = 7.5625f;
        const float d1 = 2.75f;
        if (x < 1 / d1) {
            return n1 * x * x;
        } else if (x < 2 / d1) {
            x -= 1.5f / d1;
            return n1 * x * x + 0.75f;
        } else if (x < 2.5 / d1) {
            x -= 2.25f / d1;
            return n1 * x * x + 0.9375f;
        } else {
            x -= 2.625f / d1;
            return n1 * x * x + 0.984375f;
        }}
    },
    { Tween::EASEINOUTBOUNCE, [](float x) {return x < 0.5f ? (1 - tweenMap.at(Tween::EASEOUTBOUNCE)(1 - 2*x) ) /2 : (1 + tweenMap.at(Tween::EASEOUTBOUNCE)(2*x - 1)) /2;} },
    { Tween::EASEINCIRCH, [](float x) {return 1.25f-std::sqrt(1-std::sqrt(x*x*0.9216f))*1.25f;} },
    { Tween::EASEINOUTPEAKABOO, [](float x) {const float t = 1-x;return x < 0.5f ? x - (2 * x * x) : 1 - (t - (2 * t * t));} },
    { Tween::EASEINOUTCUSTOM1, [](float x) {return 3*std::pow(1 - x, 2) * x * 0.19f + 4 * (1 - x) * std::pow(x, 2) * 0.57f + std::pow(x, 3);} },
};

void Tween::_process(float deltaTime) {
    if (isPaused || isStopped || !gameplay_running)
        return;

    if (progress < 1) {
        progressTimer += deltaTime;
        progressTimer = Math::clamp(0.f, progressTimer, progressMaxTime);
        progress = progressTimer/progressMaxTime;
        switch(selectedType) {
            case TpFLOAT: {
                *lpFloat = initVal + (finalVal - initVal) * tweenLambda(progress);
            } break;
            case TpVECTOR2: {
                *lpVector2 = initVecVal + (finalVecVal - initVecVal) * tweenLambda(progress);
            } break;
        }
    } else if (!isStopped) {
        isStopped = true;
    }
}