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
    void renderBackground();
    bool isTouchingBranch(float playerLeftX) const;

    Renderer m_renderer;
    InputManager m_input;
    bool m_running = false;

    // The player, drawn from assets/orangutan.bmp, moved with arrow keys / WASD.
    // Position is tracked in world space, which extends infinitely left/right;
    // the camera follows the player horizontally so the world scrolls beneath it.
    SDL_Texture* m_playerTexture = nullptr;
    float m_playerWorldX = 0.0f;
    float m_playerWorldY = 0.0f;
    int m_playerWidth = 40;
    int m_playerHeight = 40;
    float m_playerSpeed = 300.0f; // pixels per second

    float m_cameraX = 0.0f;
    float m_cameraMarginX = 220.0f; // how close (in screen pixels) the player can get to the edge before the camera scrolls
    int m_windowWidth = 0;
    int m_windowHeight = 0;

    // Branches are vertical bars spanning the full window height, spaced evenly in world space.
    // The player can only move (in any direction) while overlapping one.
    static constexpr float m_branchSpacing = 300.0f;
    static constexpr float m_branchWidth = 16.0f;
};
