#pragma once

#include "EventBus/Event.h"
#include "SDL2/SDL_keycode.h"
#include <SDL2/SDL.h>

class KeyPressedEvent : public Event {
private:
public:
    SDL_KeyCode mSymbol;
    KeyPressedEvent(int symbolAsNum) : mSymbol(static_cast<SDL_KeyCode>(symbolAsNum)){};
    KeyPressedEvent(SDL_KeyCode symbol) : mSymbol(symbol){};
};