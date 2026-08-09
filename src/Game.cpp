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
    m_player.x = width / 2 - m_player.w / 2;
    m_player.y = height / 2 - m_player.h / 2;

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

    m_player.x += static_cast<int>(dx * m_playerSpeed * deltaSeconds);
    m_player.y += static_cast<int>(dy * m_playerSpeed * deltaSeconds);

    if (m_player.x < 0) m_player.x = 0;
    if (m_player.y < 0) m_player.y = 0;
    if (m_player.x + m_player.w > m_windowWidth) m_player.x = m_windowWidth - m_player.w;
    if (m_player.y + m_player.h > m_windowHeight) m_player.y = m_windowHeight - m_player.h;
}

void Game::render() {
    m_renderer.clear(30, 30, 40);
    m_renderer.drawRect(m_player, 100, 200, 255);
    m_renderer.present();
}

void Game::shutdown() {
    m_renderer.shutdown();
    SDL_Quit();
}
