#include "InputManager.h"

void InputManager::handleEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_QUIT:
            m_quitRequested = true;
            break;

        case SDL_KEYDOWN:
            if (!event.key.repeat) {
                SDL_Scancode key = event.key.keysym.scancode;
                m_down[key] = true;
                m_pressed[key] = true;
            }
            break;

        case SDL_KEYUP: {
            SDL_Scancode key = event.key.keysym.scancode;
            m_down[key] = false;
            m_released[key] = true;
            break;
        }

        default:
            break;
    }
}

void InputManager::beginNewFrame() {
    m_pressed.clear();
    m_released.clear();
}

bool InputManager::isKeyDown(SDL_Scancode key) const {
    auto it = m_down.find(key);
    return it != m_down.end() && it->second;
}

bool InputManager::wasKeyPressed(SDL_Scancode key) const {
    auto it = m_pressed.find(key);
    return it != m_pressed.end() && it->second;
}

bool InputManager::wasKeyReleased(SDL_Scancode key) const {
    auto it = m_released.find(key);
    return it != m_released.end() && it->second;
}
