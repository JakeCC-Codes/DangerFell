#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <memory>
#include "math.h"
#include "external/stb_image.h"
#include <map>
#include <functional>

typedef unsigned int keyCode;

class Sprite {
    public:
    static std::vector<std::unique_ptr<Sprite>> registry;
    static void fireAllProcesses(float deltaTime) {
        std::vector<Sprite*> activeSprites;
        activeSprites.reserve(registry.size());
        for (const std::unique_ptr<Sprite>& s : Sprite::registry) {
            activeSprites.push_back(s.get());
        }

        for (Sprite* spriteInstance : activeSprites) {
            spriteInstance->_process(deltaTime);
        }
    }

    virtual std::string getTypeID() const { return "dangerfell:none"; };
    virtual std::string getCollisionLayer() const { return "dangerfell:0_layer"; };
    virtual void Input_ButtonDown(keyCode vk_code) {};
    virtual void Input_ButtonUp(keyCode vk_code) {};
    virtual void Input_ButtonPressed(keyCode vk_code) {};

    static void DestroyInstance(Sprite* ptr) {
        auto it = std::remove_if(registry.begin(), registry.end(), [ptr](const std::unique_ptr<Sprite>& s) {
            return s.get() == ptr;
        });
        registry.erase(it, registry.end());
    }
    
    private:
    static bool register_entries;
    protected:
    float shakeDebounce = 0.00f;
    Vector2 shakeOffset = Vector2();
    virtual void _process(float deltaTime) {};
};

class GameLoop { // Known Bug: If you reset the score on the first Ready the Ready() will not call again
    public:
    static int score;
    static std::vector<std::unique_ptr<GameLoop>> registry;
    static void fireAllProcesses(float deltaTime) {
        std::vector<GameLoop*> activeProcesses;
        activeProcesses.reserve(registry.size());
        for (const std::unique_ptr<GameLoop>& s : GameLoop::registry) {
            activeProcesses.push_back(s.get());
        }

        for (GameLoop* levelInstance : activeProcesses) {
            std::array<int, 2> scoreRange = levelInstance->getScoreRange();
            if (abs(scoreRange[0]) <= score && score <= abs(scoreRange[1])) {
                if (cacheReadyScore-1 != scoreRange[0]) {
                    cacheReadyScore = scoreRange[0]+1;
                    levelInstance->_ready();
                }
                levelInstance->_process(deltaTime);
            }
        }
    }
    virtual std::string getTypeID() const { return "dangerfell:level0"; };
    virtual std::array<int, 2> getScoreRange() const { return {0, 0}; }

    static void DestroyInstance(GameLoop* ptr) {
        auto it = std::remove_if(registry.begin(), registry.end(), [ptr](const std::unique_ptr<GameLoop>& s) {
            return s.get() == ptr;
        });
        registry.erase(it, registry.end());
    }

    private:
    static int cacheReadyScore;
    static bool register_entries;

    protected:
    virtual void _ready() {};
    virtual void _process(float deltaTime) {};
};

class ImageFile {
    public:
    std::string name;
    std::string fileType;
    std::string path;
    int width, height, channels;
    unsigned int* data;

    ImageFile(std::string filePath, std::string fileName = "") {
        this->path = filePath;
        size_t startPos = filePath.find_last_of('\\');
        size_t endPos = filePath.find_last_of('.');
        startPos = (startPos == std::string::npos) ? 0 : startPos +1;
        this->name = (!fileName.empty()) ? fileName : filePath.substr(startPos, (endPos == std::string::npos) ? filePath.length() - startPos : endPos - startPos);
        this->fileType = filePath.substr(endPos == std::string::npos ? filePath.length() : endPos+1);
        ImageFile* prevInstance = ImageFile::get(this->path, [](ImageFile* x){return x->path;});
        if (prevInstance) {
            this->name = prevInstance->name;
            this->width = prevInstance->width;
            this->height = prevInstance->height;
            this->channels = prevInstance->channels;
            this->fileType = prevInstance->fileType;
            this->data = prevInstance->data;
        } else {
            std::stringstream ss; ss << this->name << "." << this->fileType;
            unsigned char* dataU8 = stbi_load(filePath.c_str(), &this->width, &this->height, &this->channels, 4);
            if (!dataU8) {
                std::cerr << "Error: Failed to load image [" << filePath << "]" << std::endl;
            } else {
                this->data = reinterpret_cast<unsigned int*>(dataU8);
            }

        }
        ImageFile::registry.push_back(this);
    }
    static ImageFile* get(std::string name) {
        std::vector<ImageFile*> sCopy;
        sCopy.reserve(registry.size());
        for (ImageFile* file : registry) {
            sCopy.push_back(file);
        }
        for (ImageFile* image : sCopy) {
            if (name == image->name) {
                return image;
            }
        }
        return nullptr;
    }
    template<class L> static ImageFile* get(std::string name, L lambda = [](ImageFile* image){return image->name;}) {
        std::vector<ImageFile*> sCopy;
        sCopy.reserve(registry.size());
        for (ImageFile* file : registry) {
            sCopy.push_back(file);
        }
        for (ImageFile* image : sCopy) {
            if (name == lambda(image)) {
                return image;
            }
        }
        return nullptr;
    }
    void Destroy();

    private:
    static std::vector<ImageFile*> registry;
};

class Tween {
    public:
    enum TweenMode {
        LINEAR,
        EASEINSINE,
        EASEOUTSINE,
        EASEINOUTSINE,
        EASEINQUAD,
        EASEOUTQUAD,
        EASEINOUTQUAD,
        EASEINCUBIC,
        EASEOUTCUBIC,
        EASEINOUTCUBIC,
        EASEINQUART,
        EASEOUTQUART,
        EASEINOUTQUART,
        EASEINQUINT,
        EASEOUTQUINT,
        EASEINOUTQUINT,
        EASEINEXPO,
        EASEOUTEXPO,
        EASEINOUTEXPO,
        EASEINCIRC,
        EASEOUTCIRC,
        EASEINOUTCIRC,
        EASEINBACK,
        EASEOUTBACK,
        EASEINOUTBACK,
        EASEINELASTIC,
        EASEOUTELASTIC,
        EASEINOUTELASTIC,
        EASEINBOUNCE,
        EASEOUTBOUNCE,
        EASEINOUTBOUNCE,
        EASEINOUTCUSTOM1,
        EASEINCIRCH,
        EASEINOUTPEAKABOO
    };
    bool isPaused = false;
    bool isStopped = false;
    std::function<float(float)> tweenLambda;
    Tween(float* var, float final, float time, TweenMode mode = LINEAR) {
        this->progressMaxTime = time;
        this->tweenLambda = tweenMap.at(mode);
        this->lpFloat = var;
        this->initVal = *var;
        this->finalVal = final;
        this->selectedType = TpFLOAT;
        Tween::registry.push_back(this);
    }
    Tween(Vector2* var, Vector2 final, float time, TweenMode mode = LINEAR) {
        this->progressMaxTime = time;
        this->tweenLambda = tweenMap.at(mode);
        this->lpVector2 = var;
        this->initVecVal = *var;
        this->finalVecVal = final;
        this->selectedType = TpVECTOR2;
        Tween::registry.push_back(this);
    }
    ~Tween() {
        auto it = std::remove(registry.begin(), registry.end(), this);
        registry.erase(it, registry.end());
    }
    // Start
    // Pause
    // Stop

    static void fireAllProcesses(float deltaTime) {
        std::vector<Tween*> activeTweens;
        activeTweens.reserve(registry.size());
        for (Tween* s : Tween::registry) {
            activeTweens.push_back(s);
        }

        for (Tween* tweenInstance : activeTweens) {
            if (tweenInstance->isStopped) {
                delete tweenInstance;
            } else {
                tweenInstance->_process(deltaTime);
            }
        }
    }

    constexpr float getProgress() { return progress; } // read-only

    protected:
    void _process(float deltaTime);
    
    private:
    float progress = 0.f; // 0 - 1
    float progressMaxTime = 0.f;
    float progressTimer = 0.00f;

    enum AllowedTypes {TpFLOAT, TpVECTOR2};
    AllowedTypes selectedType;

    float* lpFloat;
    float initVal;
    float finalVal;
    Vector2* lpVector2;
    Vector2 initVecVal;
    Vector2 finalVecVal;
    static std::vector<Tween*> registry;
    static const std::map<TweenMode, std::function<float(float)>> tweenMap;
};