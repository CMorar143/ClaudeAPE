#include "Game.h"

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

    // Horizontal movement is unbounded: the world scrolls beneath the player instead.
    m_playerWorldX += dx * m_playerSpeed * deltaSeconds;
    m_playerWorldY += dy * m_playerSpeed * deltaSeconds;

    if (m_playerWorldY < 0.0f) m_playerWorldY = 0.0f;
    if (m_playerWorldY + m_playerHeight > m_windowHeight) m_playerWorldY = m_windowHeight - m_playerHeight;

    // The camera only moves once the player nears the screen edge; inside that
    // dead zone the player is free to move without the background scrolling.
    float playerScreenX = m_playerWorldX - m_cameraX;
    if (playerScreenX < m_cameraMarginX) {
        m_cameraX = m_playerWorldX - m_cameraMarginX;
    } else if (playerScreenX + m_playerWidth > m_windowWidth - m_cameraMarginX) {
        m_cameraX = m_playerWorldX + m_playerWidth - (m_windowWidth - m_cameraMarginX);
    }
}

void Game::renderBackground() {
    const int spacing = 80;
    int firstLineIndex = static_cast<int>(SDL_floorf(m_cameraX / spacing));
    int lastWorldX = static_cast<int>(m_cameraX) + m_windowWidth + spacing;

    for (int worldX = firstLineIndex * spacing; worldX < lastWorldX; worldX += spacing) {
        int screenX = static_cast<int>(worldX - m_cameraX);
        bool isOrigin = (worldX == 0);
        if (isOrigin) {
            m_renderer.drawLine(screenX, 0, screenX, m_windowHeight, 220, 90, 90);
        } else {
            m_renderer.drawLine(screenX, 0, screenX, m_windowHeight, 60, 60, 75);
        }
    }
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
    m_renderer.drawRect(playerScreenRect, 100, 200, 255);

    m_renderer.present();
}

void Game::shutdown() {
    m_renderer.shutdown();
    SDL_Quit();
}
