#pragma once

#include <map>
#include <string>

#include <SDL2/SDL.h>

// one single source of truth of asset
// should be singleton
class AssetStore {
private:
    std::map<std::string, SDL_Texture*> mTextures;
    // TODO:
    // map for fonts, map for audio, etc...

public:
    AssetStore();
    virtual ~AssetStore();

    void ClearAssets();
    void AddTexture(
        SDL_Renderer* renderer,
        const std::string& assetId,
        const std::string& filePath);
    SDL_Texture* GetTexture(const std::string& assetId);
};