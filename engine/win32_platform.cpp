#include <windows.h>
#include <wingdi.h>
#include <memoryapi.h>
#include <winuser.h>
#include "math.h"
#include "util.h"
#include "utilClasses.h"
#include "inputHandler.h"
#include "renderer.h"

static char window_title[12] = "DangerFell";
enum WindowMetrics {positionX = CW_USEDEFAULT, positionY = CW_USEDEFAULT, sizeX = 1024, sizeY = 768, fpsCap = 420};
RenderStateBuffer buffer;

bool gameplay_running = true; // Freeze Time LOL
static bool window_running = true;
static const float window_fps = 500.f/fpsCap;
float deltaTime, oldDT, newDT = 1.f/fpsCap, lastRoundFPS = 0.f, fps = 0.f;

Vector2 borderOffset = Vector2();

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    LRESULT result = 0;
    switch (uMsg) {
        case WM_CLOSE:
        case WM_DESTROY: {window_running = false;} break;
        case WM_SIZE: {
            RECT viewport_rect;
            GetClientRect(hWnd, &viewport_rect);
            buffer.width = viewport_rect.right - viewport_rect.left;
            buffer.height = viewport_rect.bottom - viewport_rect.top;

            int buffer_size = buffer.width * buffer.height * sizeof(u32);

            if (buffer.memory) {VirtualFree(buffer.memory, 0, MEM_RELEASE); buffer.memory = nullptr;}
            buffer.memory = VirtualAlloc(nullptr, buffer_size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            buffer.bitmapInfo.bmiHeader = {.biSize = sizeof(buffer.bitmapInfo.bmiHeader), .biWidth = buffer.width, .biHeight = buffer.height, .biPlanes = 1, .biBitCount = 32, .biCompression = BI_RGB};
            buffer.aspectRatio = (float)buffer.width / buffer.height;
            buffer.aspectRatioInverse = (float)buffer.height / buffer.width;
        } break;

        default: {result = DefWindowProc(hWnd, uMsg, wParam, lParam);}
    }

    return result;
};

int WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    // Create Window Class
    WNDCLASS window_class = {.style= CS_HREDRAW | CS_VREDRAW, .lpfnWndProc= WindowProc, .hInstance= hInstance, .lpszClassName= window_title};
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hIcon = (HICON)LoadImage(hInstance, TEXT("assets\\textures\\icon.ico"), IMAGE_ICON, 32, 32, LR_LOADFROMFILE | LR_DEFAULTCOLOR);
    // Register Window Class
    RegisterClass(&window_class);
    // Create Window
    HWND game_window = CreateWindow(window_class.lpszClassName, window_class.lpszClassName, WS_OVERLAPPEDWINDOW | WS_VISIBLE, positionX, positionY, sizeX, sizeY, NULL, NULL, hInstance, NULL);

    LARGE_INTEGER frame_time_begin;
    QueryPerformanceCounter(&frame_time_begin);

    float performance_frequency;
    {
        LARGE_INTEGER perf;
        QueryPerformanceFrequency(&perf);
        performance_frequency = (float)perf.QuadPart;
    }
    //PlaySound(TEXT("assets\\music\\DangerFell.wav"), NULL, SND_LOOP | SND_ASYNC); // Music
    
    //Game Loop
    while (window_running) {
        Sleep(window_fps);
        //Input
        MSG message;
        while (PeekMessage(&message, game_window, 0, 0, PM_REMOVE)) {
            switch(message.message) {
                case WM_KEYUP:
                case WM_KEYDOWN: {
                    Input::VKeyEvent((keyCode)message.wParam, ((message.lParam & (1 << 31)) == 0));
                } break;
                default: {
                    TranslateMessage(&message);
                    DispatchMessage(&message);
                }
            }
        }
        

        if (buffer.memory) {
            //Process - Simulate
            renderBackgroundColour(0xFF8F00, &buffer);
            GameLoop::fireAllProcesses(deltaTime);
            Sprite::fireAllProcesses(deltaTime);
            Tween::fireAllProcesses(deltaTime);
            drawTexture(Vector2(0, 50), Vector2(7*buffer.aspectRatio, 103), "screen_border", &buffer, {false, false}, Vector2(borderOffset.x, borderOffset.y + 103)); //0xC05700
            drawTexture(Vector2(100, 50), Vector2(7*buffer.aspectRatio, 103), "screen_border", &buffer, {true, false}, borderOffset); //0xC05700
            if ((int)(1.f/deltaTime *0.05f)*20 != lastRoundFPS ) {
                lastRoundFPS = (int)(1.f/deltaTime *0.05f)*20;
                fps = 1.f/deltaTime;
            }
            drawText(Vector2(16, 94), Vector2(40, 5), "FPS: " + std::to_string((int)fps), 0xEEFFEE, "font_ccsanz", &buffer);
            drawText(Vector2(16, 88), Vector2(40, 5), "SCORE: " + std::to_string(GameLoop::score), 0xEEFFEE, "font_ccsanz", &buffer);
            // drawText(Vector2(50, 50), Vector2(30, 15), "Hello World and friends!", 0xEEFFEE, "font_ccsanz", &buffer);
            borderOffset.y += defaultScrollSpeed * 1.5f * deltaTime * gameplay_running;
            borderOffset = Math::wrapVector2(Vector2(), borderOffset, Vector2(1.f, 200.f));
            //Make Draw Order, Sprites and Query

        //Output - Render
            HDC hdc = GetDC(game_window);
            StretchDIBits(hdc, 0, 0, buffer.width, buffer.height, 0, 0, buffer.width, buffer.height, buffer.memory, &buffer.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
            ReleaseDC(game_window, hdc); // Memory Leak Fix
        }
        
        LARGE_INTEGER frame_time_end;
        QueryPerformanceCounter(&frame_time_end);
        newDT = ((float)(frame_time_end.QuadPart - frame_time_begin.QuadPart) / performance_frequency)/2;
        if (oldDT != newDT)
            deltaTime = newDT;
        frame_time_begin = frame_time_end;
        oldDT = deltaTime;
    }
}