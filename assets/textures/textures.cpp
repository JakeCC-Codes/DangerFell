#include "../../engine/utilClasses.h"
// Make unique ptrs

const std::unique_ptr<ImageFile> font = std::make_unique<ImageFile>("assets\\textures\\font_ccsanz.png", "font_ccsanz");

const std::unique_ptr<ImageFile> screen_border = std::make_unique<ImageFile>("assets\\textures\\screen_border.png");
const std::unique_ptr<ImageFile> background_clouds = std::make_unique<ImageFile>("assets\\textures\\background_clouds.png", "clouds");
const std::unique_ptr<ImageFile> wind = std::make_unique<ImageFile>("assets\\textures\\wind_particle.png", "wind");
const std::unique_ptr<ImageFile> bullet = std::make_unique<ImageFile>("assets\\textures\\bullet.png", "bullet");