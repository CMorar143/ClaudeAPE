#pragma once

#include "Renderer.h"
#include "InputManager.h"

class Game {
public:
    bool init(const char* title, int width, int height);
    void run();
    void shutdown();

private:
    enum class PlayerState { OnBranch, Airborne };

    void handleEvents();
    void update(float deltaSeconds);
    void updateOnBranch(float dx, float dy, float deltaSeconds);
    void updateAirborne(float deltaSeconds);
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

    // While on a branch, movement is direct (climb freely along it). Walking past its
    // touch zone, or pressing Space to jump, drops the player into free fall under
    // gravity until they land on another branch or fall past the bottom of the screen.
    PlayerState m_playerState = PlayerState::OnBranch;
    float m_velocityX = 0.0f;
    float m_velocityY = 0.0f;
    static constexpr float m_gravity = 1400.0f;  // pixels per second^2
    static constexpr float m_jumpSpeed = 600.0f; // initial upward speed when jumping

    float m_cameraX = 0.0f;
    float m_cameraMarginX = 220.0f; // how close (in screen pixels) the player can get to the edge before the camera scrolls
    int m_windowWidth = 0;
    int m_windowHeight = 0;

    // Branches are vertical bars spanning the full window height, spaced evenly in world space.
    static constexpr float m_branchSpacing = 300.0f;
    static constexpr float m_branchWidth = 16.0f;
};
