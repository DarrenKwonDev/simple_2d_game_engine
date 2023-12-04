#pragma once

#include <map>
#include <string>

#include <SDL2/SDL.h>

////////////////////////////////////////
// texture, audio 등을 저장하는 singleton
////////////////////////////////////////
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