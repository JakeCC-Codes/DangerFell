#include "utilClasses.h"
#include "inputHandler.h"
#include "../assets/scripts/sprites.h"
#include "../assets/scripts/levels.h"

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