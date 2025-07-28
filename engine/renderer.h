#pragma once
#include "math.h"
#include "util.h"
#include "utilClasses.h"

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

        Vector2 UcPos1 = pos1;
        Vector2 UcPos2 = pos2;

        pos1 = Math::clampVector2(Vector2(), pos1, bufferSize);
        pos2 = Math::clampVector2(Vector2(), pos2, bufferSize);

        for (int y = pos1.y; y < pos2.y; y++) {
            u32* pixel = (u32*)lpRenderStateBuffer->memory + y * (int)bufferSize.x + (int)pos1.x;
            for (int x = pos1.x; x < pos2.x; x++) {
                // compute uv (0-1)
                float u = (x - UcPos1.x) / (UcPos2.x - UcPos1.x);
                float v = (y - UcPos1.y) / (UcPos2.y - UcPos1.y);

                // Flip Axis
                u = flipAxis[0] ? (1 - u) : u;
                v = flipAxis[1] ? v : (1 - v);

                // Offset
                u += offset.x;
                v += offset.y;

                // Wrap UVs (screen-wrap)
                u = Math::wrap(0.f, u, 1.f);
                v = Math::wrap(0.f, v, 1.f);

                // convert to image coordinates
                int srcX = u * (image->width - 0.1f);
                int srcY = v * (image->height - 0.1f);

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

        pos1 = Math::clampVector2(Vector2(), pos1, bufferSize);
        pos2 = Math::clampVector2(Vector2(), pos2, bufferSize);

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

        pos1 = Math::clampVector2(Vector2(), pos1, bufferSize);
        pos2 = Math::clampVector2(Vector2(), pos2, bufferSize);

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

    inline void drawTexture(Vector2 position, Vector2 size, string imageName, RenderStateBuffer *lpRenderStateBuffer, std::array<bool, 2> flipAxis = {false, false}, Vector2 textureOffset = Vector2()) {
        const float scale = 0.01f;
        position *= Vector2(lpRenderStateBuffer->width, lpRenderStateBuffer->height) * scale;
        size *= Vector2(lpRenderStateBuffer->height, lpRenderStateBuffer->height) * scale;
        textureOffset = textureOffset * scale;

        ImageFile* image = ImageFile::get(imageName);
        if (!image) {cout << "Error: Image not found [" << imageName << "]" << std::endl; return;}

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
}
using namespace Renderer;