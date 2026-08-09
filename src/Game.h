#pragma once

#include "Renderer.h"
#include "InputManager.h"

class Game {
public:
    bool init(const char* title, int width, int height);
    void run();
    void shutdown();

private:
    void handleEvents();
    void update(float deltaSeconds);
    void render();

    Renderer m_renderer;
    InputManager m_input;
    bool m_running = false;

    // Example game state: a rectangle the player moves with arrow keys / WASD.
    SDL_Rect m_player{0, 0, 40, 40};
    float m_playerSpeed = 300.0f; // pixels per second
    int m_windowWidth = 0;
    int m_windowHeight = 0;
};
