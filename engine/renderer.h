#pragma once
#include "math.h"
#include "util.h"
#include "utilClasses.h"
#include "textRendererUtil.h"
#include <array>
#include <wingdi.h>

namespace Renderer
{
    inline float getBoundaryOffset(float sizeX) {
        return (7.f + sizeX*buffer.aspectRatioInverse)/2;
    }

    inline void renderBackgroundColour(u32 colour, RenderStateBuffer* lpRenderStateBuffer) {
        u32* pixel = (u32*)lpRenderStateBuffer->memory;
        int totalPixels = lpRenderStateBuffer->width * lpRenderStateBuffer->height;

        for (int i = 0; i < totalPixels; i++) {
            if (pixel[i] != colour) {
                pixel[i] = colour;
            }
        }
    }

    inline void drawTextureInPixels(Vector2 pos1, Vector2 pos2, ImageFile *image, RenderStateBuffer* lpRenderStateBuffer, std::array<bool, 2> flipAxis, Vector2 offset = Vector2()) {
        Vector2 bufferSize = Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height);
        if (pos1.x > pos2.x) std::swap(pos1.x, pos2.x); // Flip if negative
        if (pos1.y > pos2.y) std::swap(pos1.y, pos2.y); // Flip if negative

        Vector2 UcPos1 = pos1.asInt();
        Vector2 UcPos2 = pos2.asInt();
        float width = (UcPos2.x - UcPos1.x);
        float height = (UcPos2.y - UcPos1.y);
        pos1 = Math::clampVector2(Vector2(), pos1.asInt(), bufferSize);
        pos2 = Math::clampVector2(Vector2(), pos2.asInt(), bufferSize);

        for (int y = pos1.y; y < pos2.y; y++) {
            u32* pixel = (u32*)lpRenderStateBuffer->memory + y * (int)bufferSize.x + (int)pos1.x;
            for (int x = pos1.x; x < pos2.x; x++) {
                // compute uv (0-1)
                float u = (x - UcPos1.x) / width;
                float v = (y - UcPos1.y) / height;

                // Flip Axis
                u = flipAxis[0] ? (1 - u) : u;
                v = flipAxis[1] ? v : (1 - v);

                // Offset
                u += offset.x;
                v += offset.y - 0.001f;

                // Wrap UVs (screen-wrap)
                u = Math::wrap(0.f, u, 1.f);
                v = Math::wrap(0.f, v, 1.f);

                // convert to image coordinates
                int srcX = u * image->width;
                int srcY = v * image->height;

                // sample pixel
                u32 rgba = image->data[srcY * image->width + srcX];
                uint8_t* c = (uint8_t*)&rgba;
                u32 bgra = (c[2] << 0) | (c[1] << 8) | (c[0] << 16) | (c[3] << 24);

                // write to framebuffer
                if (rgba != 0 && *pixel != bgra) *pixel = bgra;
                pixel++;
            }
        }
    }
    inline void drawRectInPixels(Vector2 pos1, Vector2 pos2, u32 colour, RenderStateBuffer* lpRenderStateBuffer) {
        Vector2 bufferSize = Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height);
        if (pos1.x > pos2.x) std::swap(pos1.x, pos2.x); // Flip if negative
        if (pos1.y > pos2.y) std::swap(pos1.y, pos2.y); // Flip if negative

        pos1 = Math::clampVector2(Vector2(), pos1.asInt(), bufferSize);
        pos2 = Math::clampVector2(Vector2(), pos2.asInt(), bufferSize);

        for (int y = pos1.y; y < pos2.y; y++) {
            u32* pixel = (u32*)lpRenderStateBuffer->memory + y * (int)bufferSize.x + (int)pos1.x;
            for (int x = pos1.x; x < pos2.x; x++) {
                if (*pixel != colour) *pixel = colour;
                pixel++;
            }
        }
    }
    inline void drawEllipseInPixels(Vector2 pos1, Vector2 pos2, u32 colour, RenderStateBuffer* lpRenderStateBuffer) {
        Vector2 bufferSize = Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height);
        if (pos1.x > pos2.x) std::swap(pos1.x, pos2.x); // Flip if negative
        if (pos1.y > pos2.y) std::swap(pos1.y, pos2.y); // Flip if negative
        Vector2 centrePos = (pos1 + pos2)*0.5f;
        float rx = 2.f/(pos2.x - pos1.x);
        float ry = 2.f/(pos2.y - pos1.y);

        pos1 = Math::clampVector2(Vector2(), pos1.asInt(), bufferSize);
        pos2 = Math::clampVector2(Vector2(), pos2.asInt(), bufferSize);

        for (int y = pos1.y; y < pos2.y; y++) {
            u32* pixel = (u32*)lpRenderStateBuffer->memory + y * (int)bufferSize.x + (int)pos1.x;
            for (int x = pos1.x; x < pos2.x; x++) {
                float dx = (x - centrePos.x) * rx;
                float dy = (y - centrePos.y) * ry;
                if (*pixel != colour && dx*dx + dy*dy <= 1.0f) *pixel = colour;
                pixel++;
            }
        }
    }
    inline void drawTextInPixels(Vector2 pos1, Vector2 pos2, string prompt, u32 colour, string fontName, RenderStateBuffer* lpRenderStateBuffer) {
        ImageFile* fontImage = ImageFile::get(fontName);
        if (!fontImage) {std::cerr << "Error: Font file not found [" << fontName << "]" << '\n'; return;}
        Vector2 bufferSize = Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height);
        const int promptSize = (int)prompt.size();
        if (pos1.x > pos2.x) std::swap(pos1.x, pos2.x); // Flip if negative
        if (pos1.y > pos2.y) std::swap(pos1.y, pos2.y); // Flip if negative
        Vector2 boxSize = Vector2(pos2.x - pos1.x, pos2.y - pos1.y);
        Vector2 letterSize = Vector2(boxSize.y*defaultLetterAspect, boxSize.y);
        Vector2 letterSizeInverse = 1.f/letterSize;
        int columns = Math::maxLITE(1, std::floor(boxSize.x * letterSizeInverse.x));
        int rows = std::ceil((float)promptSize/columns);
        
        letterSizeInverse *= Math::maxLITE(1, std::floor(rows * 0.5f));
        columns = Math::maxLITE(1, std::floor(boxSize.x * letterSizeInverse.x));
        rows = std::ceil((float)promptSize/columns);

        Vector2 Upos3 = Vector2(static_cast<int>(pos1.x), static_cast<int>(pos2.y));
        pos1 = Math::clampVector2(Vector2(), pos1.asInt(), bufferSize);
        pos2 = Math::clampVector2(Vector2(), pos2.asInt(), bufferSize);
        

        std::vector<Vector2> promptMap;
        prompt.reserve(promptSize);
        for (int i=0; i<promptSize; i++) {
            promptMap.push_back(defaultFontUVMap.find(prompt[i])->second[0]);
        }

        for (int y = pos1.y; y < pos2.y; y++) {
            u32* pixel = (u32*)lpRenderStateBuffer->memory + y * (int)bufferSize.x + (int)pos1.x;
            float k = (Upos3.y - y)*letterSizeInverse.y;

            for (int x = pos1.x; x < pos2.x; x++) {
                float j = (x - Upos3.x)*letterSizeInverse.x;
                int i = std::floor(k)*columns + std::floor(j);
                if (i < promptSize && j < columns) {
                    float u = promptMap[i].x + Math::floorDecimal(j) * defaultLetterUVWidth;
                    float v = promptMap[i].y + Math::floorDecimal(k) * defaultLetterUVHeight;

                    int srcX = u * fontImage->width;
                    int srcY = v * fontImage->height;
                    if ((u32)fontImage->data[srcY * fontImage->width + srcX] != 0 && *pixel != colour) *pixel = colour;
                    pixel++;
                }

            }
        }
    }

    inline void drawTexture(Vector2 position, Vector2 size, string imageName, RenderStateBuffer *lpRenderStateBuffer, std::array<bool, 2> flipAxis = {false, false}, Vector2 textureOffset = Vector2()) {
        const float scale = 0.01f;
        position *= Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height) * scale;
        size *= Vector2(lpRenderStateBuffer->height, lpRenderStateBuffer->height) * scale;
        textureOffset = textureOffset * scale;

        ImageFile* image = ImageFile::get(imageName);
        if (!image) {std::cerr << "Error: Image not found [" << imageName << "]" << '\n'; return;}

        drawTextureInPixels(position - size/2, position + size/2, image, lpRenderStateBuffer, flipAxis, textureOffset);
    }
    inline void drawTexture(Vector2 position, Vector2 size, ImageFile image, RenderStateBuffer *lpRenderStateBuffer, std::array<bool, 2> flipAxis = {false, false}, Vector2 textureOffset = Vector2()) {
        const float scale = 0.01f;
        position *= Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height) * scale;
        size *= Vector2(lpRenderStateBuffer->height, lpRenderStateBuffer->height) * scale;
        textureOffset = textureOffset * scale;

        drawTextureInPixels(position - size/2, position + size/2, &image, lpRenderStateBuffer, flipAxis, textureOffset);
    }
    inline void drawRect(Vector2 position, Vector2 size, u32 colour, RenderStateBuffer *lpRenderStateBuffer) {
        const float scale = 0.01f;
        position *= Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height) * scale;
        size *= Vector2(lpRenderStateBuffer->height, lpRenderStateBuffer->height) * scale;

        drawRectInPixels(position - size/2, position + size/2, colour, lpRenderStateBuffer);
    }
    inline void drawEllipse(Vector2 position, Vector2 size, u32 colour, RenderStateBuffer *lpRenderStateBuffer) {
        const float scale = 0.01f;
        position *= Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height) * scale;
        size *= Vector2(lpRenderStateBuffer->height, lpRenderStateBuffer->height) * scale;

        drawEllipseInPixels(position - size/2, position + size/2, colour, lpRenderStateBuffer);
    }
    inline void drawText(Vector2 position, Vector2 size, string prompt, u32 colour = 0xFFFFFF, string fontName = "font_ccsanz", RenderStateBuffer *lpRenderStateBuffer = &buffer) { // Finish and fix
        const float scale = 0.01f;
        position *= Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height) * scale;
        size *= Vector2(lpRenderStateBuffer->height, lpRenderStateBuffer->height) * scale;

        drawTextInPixels(position - size/2, position + size/2, prompt, colour, fontName, lpRenderStateBuffer);
    }
}
using namespace Renderer;