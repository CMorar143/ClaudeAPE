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

    // Branches aren't anchored anywhere: the whole branch shifts left/right as a rigid
    // body, spring-damped toward a target driven by which side the orangutan is on.
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
    void renderSky();
    void renderForest();
    void renderMountainLayer(SDL_Texture* texture, float parallax, float baselineFraction, Uint8 alpha);
    void renderTreeLayer(SDL_Texture* variantA, SDL_Texture* variantB, float parallax, float slotSpacing, float baseHeight, int seed, float minAnchorFraction, float maxAnchorFraction);
    void renderBackground();
    bool isTouchingBranch(float playerLeftX) const;
    int nearestBranchIndex(float worldX) const;
    float branchLeanAt(int branchIndex) const;
    void visibleBranchIndexRange(int& firstIndex, int& lastIndex) const;
    static float hashToUnitFloat(int seed);

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

    // Branches are spaced evenly in world space. Each one is a free-floating vertical
    // bar (not pinned at either end) that drifts gently left/right, damped heavily
    // enough that it settles toward the orangutan's side without overshooting past it --
    // a subtle nudge that follows the climber, not something that swings wildly on its own.
    static constexpr float m_branchSpacing = 300.0f;
    static constexpr float m_branchWidth = 16.0f;
    static constexpr float m_maxBranchLean = 75.0f; // max shift (pixels) when the orangutan is fully to one side
    static constexpr float m_branchSpringStiffness = 25.0f;
    static constexpr float m_branchDamping = 10.0f; // near-critical: settles without overshoot
    std::unordered_map<int, BranchState> m_branches;

    // Decorative forest background: a handful of tree/mountain textures, tiled
    // and scattered across a few parallax layers. Placement within each layer
    // is deterministic (hashed from slot index), so the same scenery reappears
    // consistently as the camera revisits a stretch of world.
    SDL_Texture* m_treeCanopyTexture = nullptr;
    SDL_Texture* m_treePalmTexture = nullptr;
    SDL_Texture* m_mountainFarTexture = nullptr;
    SDL_Texture* m_mountainNearTexture = nullptr;
};
