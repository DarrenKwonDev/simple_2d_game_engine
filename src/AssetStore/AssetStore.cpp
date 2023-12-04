

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../Logger/Logger.h"
#include "AssetStore.h"

AssetStore::AssetStore() {
    Logger::Log("asset store constructor called");
}
AssetStore::~AssetStore() {
    AssetStore::ClearAssets();
    Logger::Log("asset store deconstructor called");
}

void AssetStore::ClearAssets() {
    // ptr를 든 stl container는 내부 원소를 삭제한 뒤, container를 삭제해야 메모리 leak이 없음에 주의.
    for (auto& [_, texture] : mTextures) {
        SDL_DestroyTexture(texture);
    }
    mTextures.clear();
}

void AssetStore::AddTexture(
    SDL_Renderer* renderer,
    const std::string& assetId,
    const std::string& filePath) {

    SDL_Surface* surface = IMG_Load(filePath.c_str());
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // add texture to the map
    mTextures.emplace(assetId, texture);

    Logger::Log("new texture added. assetId : " + assetId);
}

SDL_Texture* AssetStore::GetTexture(const std::string& assetId) {
    // cpp에선 map의 operator[]는 없는 요소면 생성되는 동작을 함.
    // 따라서 아래와 같이 조회할 필요가 있음.
    return mTextures.find(assetId)->second;
}
