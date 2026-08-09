#pragma once

#include "Renderer.h"
#include "InputManager.h"
#include <unordered_map>

class Game {
public:
    bool init(const char* title, int width, int height);
    void run();
    void shutdown();

private:
    enum class PlayerState { OnBranch, Airborne };

    // A branch pivots at its base (the bottom of the screen) like a spring-loaded pole:
    // leanOffset is how far its tip has shifted horizontally from resting (vertical).
    struct BranchState {
        float leanOffset = 0.0f;
        float leanVelocity = 0.0f;
    };

    void handleEvents();
    void update(float deltaSeconds);
    void updateOnBranch(float dx, float dy, float deltaSeconds);
    void updateAirborne(float deltaSeconds);
    void updateBranches(float deltaSeconds);
    void render();
    void renderBackground();
    bool isTouchingBranch(float playerLeftX, float playerCenterY) const;
    int nearestBranchIndex(float worldX) const;
    float branchLeanAt(int branchIndex) const;
    void visibleBranchIndexRange(int& firstIndex, int& lastIndex) const;

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

    // Branches are spaced evenly in world space and pivot at the bottom of the screen.
    // Each one wobbles like a spring, leaning toward whichever side carries most of the
    // orangutan's weight, more so the higher up it climbs -- which can fling the tip far
    // enough sideways to bridge the gap to the next branch.
    static constexpr float m_branchSpacing = 300.0f;
    static constexpr float m_branchWidth = 16.0f;
    static constexpr float m_maxBranchLean = 140.0f;    // max tip offset (pixels) at full weight, top of branch
    static constexpr float m_branchSpringStiffness = 40.0f;
    static constexpr float m_branchDamping = 3.0f;
    std::unordered_map<int, BranchState> m_branches;
};
