#include "Renderer.h"
#include <cstdio>

Renderer::~Renderer() {
    shutdown();
}

bool Renderer::init(const char* title, int width, int height) {
    m_window = SDL_CreateWindow(
        title,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_SHOWN
    );
    if (!m_window) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!m_renderer) {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());
        return false;
    }

    return true;
}

void Renderer::shutdown() {
    for (auto& [path, texture] : m_textureCache) {
        SDL_DestroyTexture(texture);
    }
    m_textureCache.clear();

    if (m_renderer) {
        SDL_DestroyRenderer(m_renderer);
        m_renderer = nullptr;
    }
    if (m_window) {
        SDL_DestroyWindow(m_window);
        m_window = nullptr;
    }
}

void Renderer::clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    SDL_RenderClear(m_renderer);
}

void Renderer::present() {
    SDL_RenderPresent(m_renderer);
}

void Renderer::drawRect(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled) {
    SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    if (filled) {
        SDL_RenderFillRect(m_renderer, &rect);
    } else {
        SDL_RenderDrawRect(m_renderer, &rect);
    }
}

void Renderer::drawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    SDL_SetRenderDrawColor(m_renderer, r, g, b, a);
    SDL_RenderDrawLine(m_renderer, x1, y1, x2, y2);
}

void Renderer::drawThickLine(float x1, float y1, float x2, float y2, float thickness, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float length = SDL_sqrtf(dx * dx + dy * dy);
    if (length < 0.0001f) {
        return;
    }

    float halfThickness = thickness / 2.0f;
    float perpX = -dy / length * halfThickness;
    float perpY = dx / length * halfThickness;

    SDL_Color color{r, g, b, a};
    SDL_Vertex vertices[4] = {
        {{x1 + perpX, y1 + perpY}, color, {0, 0}},
        {{x1 - perpX, y1 - perpY}, color, {0, 0}},
        {{x2 - perpX, y2 - perpY}, color, {0, 0}},
        {{x2 + perpX, y2 + perpY}, color, {0, 0}},
    };
    int indices[6] = {0, 1, 2, 0, 2, 3};

    SDL_RenderGeometry(m_renderer, nullptr, vertices, 4, indices, 6);
}

SDL_Texture* Renderer::loadTexture(const std::string& path) {
    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end()) {
        return it->second;
    }

    SDL_Surface* surface = SDL_LoadBMP(path.c_str());
    if (!surface) {
        SDL_Log("Failed to load image '%s': %s", path.c_str(), SDL_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        SDL_Log("Failed to create texture from '%s': %s", path.c_str(), SDL_GetError());
        return nullptr;
    }

    m_textureCache[path] = texture;
    return texture;
}

void Renderer::drawTexture(SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect& dstRect) {
    SDL_RenderCopy(m_renderer, texture, srcRect, &dstRect);
}
