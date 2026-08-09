#include "Game.h"
#include <algorithm>

bool Game::init(const char* title, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return false;
    }

    if (!m_renderer.init(title, width, height)) {
        return false;
    }

    m_windowWidth = width;
    m_windowHeight = height;

    m_playerTexture = m_renderer.loadTexture("assets/orangutan.bmp");
    if (m_playerTexture) {
        int textureWidth = 0;
        int textureHeight = 0;
        SDL_QueryTexture(m_playerTexture, nullptr, nullptr, &textureWidth, &textureHeight);

        const int targetHeight = 100;
        m_playerHeight = targetHeight;
        m_playerWidth = static_cast<int>(textureWidth * (targetHeight / static_cast<float>(textureHeight)));
    }

    m_playerWorldX = 0.0f;
    m_playerWorldY = height / 2.0f - m_playerHeight / 2.0f;
    m_cameraX = m_playerWorldX + m_playerWidth / 2.0f - m_windowWidth / 2.0f;

    m_running = true;
    return true;
}

void Game::run() {
    Uint64 previousTicks = SDL_GetPerformanceCounter();
    const Uint64 frequency = SDL_GetPerformanceFrequency();

    while (m_running) {
        Uint64 currentTicks = SDL_GetPerformanceCounter();
        float deltaSeconds = static_cast<float>(currentTicks - previousTicks) / static_cast<float>(frequency);
        previousTicks = currentTicks;

        handleEvents();
        update(deltaSeconds);
        render();
    }
}

void Game::handleEvents() {
    m_input.beginNewFrame();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        m_input.handleEvent(event);
    }

    if (m_input.quitRequested() || m_input.wasKeyPressed(SDL_SCANCODE_ESCAPE)) {
        m_running = false;
    }
}

void Game::update(float deltaSeconds) {
    float dx = 0.0f;
    float dy = 0.0f;

    if (m_input.isKeyDown(SDL_SCANCODE_LEFT) || m_input.isKeyDown(SDL_SCANCODE_A)) dx -= 1.0f;
    if (m_input.isKeyDown(SDL_SCANCODE_RIGHT) || m_input.isKeyDown(SDL_SCANCODE_D)) dx += 1.0f;
    if (m_input.isKeyDown(SDL_SCANCODE_UP) || m_input.isKeyDown(SDL_SCANCODE_W)) dy -= 1.0f;
    if (m_input.isKeyDown(SDL_SCANCODE_DOWN) || m_input.isKeyDown(SDL_SCANCODE_S)) dy += 1.0f;

    if (dx != 0.0f || dy != 0.0f) {
        float length = SDL_sqrtf(dx * dx + dy * dy);
        dx /= length;
        dy /= length;
    }

    if (m_playerState == PlayerState::OnBranch) {
        updateOnBranch(dx, dy, deltaSeconds);
    } else {
        updateAirborne(deltaSeconds);
    }

    if (!m_running) {
        return;
    }

    updateBranches(deltaSeconds);

    // The camera only moves once the player nears the screen edge; inside that
    // dead zone the player is free to move without the background scrolling.
    float playerScreenX = m_playerWorldX - m_cameraX;
    if (playerScreenX < m_cameraMarginX) {
        m_cameraX = m_playerWorldX - m_cameraMarginX;
    } else if (playerScreenX + m_playerWidth > m_windowWidth - m_cameraMarginX) {
        m_cameraX = m_playerWorldX + m_playerWidth - (m_windowWidth - m_cameraMarginX);
    }
}

void Game::updateOnBranch(float dx, float dy, float deltaSeconds) {
    m_playerWorldX += dx * m_playerSpeed * deltaSeconds;
    m_playerWorldY += dy * m_playerSpeed * deltaSeconds;

    if (m_playerWorldY < 0.0f) m_playerWorldY = 0.0f;
    if (m_playerWorldY + m_playerHeight > m_windowHeight) m_playerWorldY = m_windowHeight - m_playerHeight;

    if (m_input.wasKeyPressed(SDL_SCANCODE_SPACE)) {
        m_playerState = PlayerState::Airborne;
        m_velocityX = dx * m_playerSpeed;
        m_velocityY = -m_jumpSpeed;
        return;
    }

    if (!isTouchingBranch(m_playerWorldX, m_playerWorldY + m_playerHeight / 2.0f)) {
        // Walked past the edge of the branch's touch zone: gravity takes over.
        m_playerState = PlayerState::Airborne;
        m_velocityX = dx * m_playerSpeed;
        m_velocityY = 0.0f;
    }
}

void Game::updateAirborne(float deltaSeconds) {
    m_velocityY += m_gravity * deltaSeconds;
    m_playerWorldX += m_velocityX * deltaSeconds;
    m_playerWorldY += m_velocityY * deltaSeconds;

    if (isTouchingBranch(m_playerWorldX, m_playerWorldY + m_playerHeight / 2.0f)) {
        m_playerState = PlayerState::OnBranch;
        m_velocityX = 0.0f;
        m_velocityY = 0.0f;
        if (m_playerWorldY < 0.0f) m_playerWorldY = 0.0f;
        if (m_playerWorldY + m_playerHeight > m_windowHeight) m_playerWorldY = m_windowHeight - m_playerHeight;
        return;
    }

    if (m_playerWorldY > m_windowHeight) {
        SDL_Log("Orangutan fell off screen. Game over.");
        m_running = false;
    }
}

void Game::renderBackground() {
    int firstIndex, lastIndex;
    visibleBranchIndexRange(firstIndex, lastIndex);

    for (int i = firstIndex; i <= lastIndex; ++i) {
        float worldX = i * m_branchSpacing;
        float lean = branchLeanAt(i);

        float baseScreenX = worldX - m_cameraX;
        float tipScreenX = worldX + lean - m_cameraX;
        m_renderer.drawThickLine(baseScreenX, static_cast<float>(m_windowHeight), tipScreenX, 0.0f, m_branchWidth, 90, 55, 25);
    }
}

int Game::nearestBranchIndex(float worldX) const {
    return static_cast<int>(SDL_roundf(worldX / m_branchSpacing));
}

float Game::branchLeanAt(int branchIndex) const {
    auto it = m_branches.find(branchIndex);
    return it != m_branches.end() ? it->second.leanOffset : 0.0f;
}

void Game::visibleBranchIndexRange(int& firstIndex, int& lastIndex) const {
    firstIndex = static_cast<int>(SDL_floorf(m_cameraX / m_branchSpacing)) - 1;
    lastIndex = static_cast<int>(SDL_floorf((m_cameraX + m_windowWidth) / m_branchSpacing)) + 1;
}

void Game::updateBranches(float deltaSeconds) {
    int firstIndex, lastIndex;
    visibleBranchIndexRange(firstIndex, lastIndex);

    bool hasGrippedBranch = (m_playerState == PlayerState::OnBranch);
    int grippedIndex = hasGrippedBranch ? nearestBranchIndex(m_playerWorldX + m_playerWidth / 2.0f) : 0;

    for (int i = firstIndex; i <= lastIndex; ++i) {
        BranchState& state = m_branches[i];

        float targetLean = 0.0f;
        if (hasGrippedBranch && i == grippedIndex) {
            float heightFraction = 1.0f - std::clamp(m_playerWorldY / m_windowHeight, 0.0f, 1.0f);
            float playerCenterX = m_playerWorldX + m_playerWidth / 2.0f;
            float branchNominalX = i * m_branchSpacing;
            float normalizedOffset = std::clamp((playerCenterX - branchNominalX) / (m_playerWidth / 2.0f), -1.0f, 1.0f);
            targetLean = m_maxBranchLean * heightFraction * normalizedOffset;
        }

        // Spring-damper: pulls leanOffset toward targetLean, opposed by velocity-proportional
        // damping, so the branch overshoots and settles instead of snapping straight to it.
        float springForce = m_branchSpringStiffness * (targetLean - state.leanOffset) - m_branchDamping * state.leanVelocity;
        state.leanVelocity += springForce * deltaSeconds;
        state.leanOffset += state.leanVelocity * deltaSeconds;
    }
}

bool Game::isTouchingBranch(float playerLeftX, float playerCenterY) const {
    float playerRightX = playerLeftX + m_playerWidth;
    int centerIndex = nearestBranchIndex((playerLeftX + playerRightX) / 2.0f);
    float heightFraction = 1.0f - std::clamp(playerCenterY / m_windowHeight, 0.0f, 1.0f);

    for (int i = centerIndex - 1; i <= centerIndex + 1; ++i) {
        float branchCenter = i * m_branchSpacing + branchLeanAt(i) * heightFraction;
        float branchLeft = branchCenter - m_branchWidth / 2.0f;
        float branchRight = branchCenter + m_branchWidth / 2.0f;
        if (playerRightX > branchLeft && playerLeftX < branchRight) {
            return true;
        }
    }
    return false;
}

void Game::render() {
    m_renderer.clear(30, 30, 40);
    renderBackground();

    SDL_Rect playerScreenRect{
        static_cast<int>(m_playerWorldX - m_cameraX),
        static_cast<int>(m_playerWorldY),
        m_playerWidth,
        m_playerHeight
    };
    if (m_playerTexture) {
        m_renderer.drawTexture(m_playerTexture, nullptr, playerScreenRect);
    } else {
        m_renderer.drawRect(playerScreenRect, 100, 200, 255);
    }

    m_renderer.present();
}

void Game::shutdown() {
    m_renderer.shutdown();
    SDL_Quit();
}
