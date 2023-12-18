#pragma once

#include "AssetStore/AssetStore.h"
#include "ECS/ECS.h"
#include "SDL2/SDL.h"
#include "sol/sol.hpp"
#include <memory>

class LevelLoader {
public:
    LevelLoader();
    virtual ~LevelLoader();

    void LoadLevel(sol::state& lua,
                   const std::unique_ptr<Registry>& registry,
                   const std::unique_ptr<AssetStore>& assetStore,
                   SDL_Renderer* renderer,
                   int level);
};