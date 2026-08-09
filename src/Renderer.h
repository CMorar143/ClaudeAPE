#pragma once

#include <SDL.h>
#include <string>
#include <unordered_map>

// Wraps an SDL_Window/SDL_Renderer pair and provides basic 2D drawing
// and texture-caching helpers.
class Renderer {
public:
    Renderer() = default;
    ~Renderer();

    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool init(const char* title, int width, int height);
    void shutdown();

    void clear(Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255);
    void present();

    void drawRect(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true);

    // Loads (and caches) a texture from disk. Returns nullptr on failure.
    SDL_Texture* loadTexture(const std::string& path);

    void drawTexture(SDL_Texture* texture, const SDL_Rect* srcRect, const SDL_Rect& dstRect);

    SDL_Renderer* raw() const { return m_renderer; }

private:
    SDL_Window* m_window = nullptr;
    SDL_Renderer* m_renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*> m_textureCache;
};
