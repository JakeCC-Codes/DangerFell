#pragma once
#include "math.h"
#include <iostream>
#include <windows.h>
#include <vector>
using std::string;

typedef char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;
#define null NULL;
struct RenderStateBuffer { int width, height; void* memory; BITMAPINFO bitmapInfo; float aspectRatio, aspectRatioInverse; };
extern RenderStateBuffer buffer;
extern Vector2 borderOffset;
extern bool gameplay_running;
const float defaultScrollSpeed = 104.f;
const float defaultAspectRatio = 1.70925f;

inline Vector2 ShakeOffsetFX(Vector2 offsetRange, float deltaTime, float &shakeDebounce, Vector2 &shakeOffset, float delayMod = 0.f) {
    if (shakeDebounce <= 0.00f) {
        shakeDebounce = 0.06f + delayMod;
        shakeOffset = Vector2(Math::getRandomFloat(-offsetRange.x, offsetRange.x) * buffer.aspectRatioInverse, Math::getRandomFloat(-offsetRange.y, offsetRange.y));
    } else {
        shakeDebounce -= deltaTime * gameplay_running;
    }
    return shakeOffset;
}

constexpr bool AABBCollision(Vector2 posA, Vector2 sizeA, Vector2 posB, Vector2 sizeB) {
    Vector2 newSizeA = sizeA * buffer.aspectRatioInverse;
    Vector2 newSizeB = sizeB * buffer.aspectRatioInverse;
    return (abs(posA.x - posB.x) * 2 < (newSizeA.x + newSizeB.x)) &&
        (abs(posA.y - posB.y) * 2 < (newSizeA.y + newSizeB.y));
}