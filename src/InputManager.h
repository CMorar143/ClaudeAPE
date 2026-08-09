#pragma once

#include <SDL.h>
#include <unordered_map>

// Tracks keyboard state per frame: down, pressed-this-frame, released-this-frame.
class InputManager {
public:
    // Feeds one SDL event into the input state. Call for every event in the poll loop.
    void handleEvent(const SDL_Event& event);

    // Call once per frame after all events for the frame have been handled,
    // to roll "pressed"/"released" edges back to steady "down" state.
    void beginNewFrame();

    bool isKeyDown(SDL_Scancode key) const;
    bool wasKeyPressed(SDL_Scancode key) const;
    bool wasKeyReleased(SDL_Scancode key) const;

    bool quitRequested() const { return m_quitRequested; }

private:
    std::unordered_map<SDL_Scancode, bool> m_down;
    std::unordered_map<SDL_Scancode, bool> m_pressed;
    std::unordered_map<SDL_Scancode, bool> m_released;
    bool m_quitRequested = false;
};
