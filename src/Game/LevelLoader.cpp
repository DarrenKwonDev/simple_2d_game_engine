#include "LevelLoader.h"
#include "Components/AnimationComponent.h"
#include "Components/BoxColliderComponent.h"
#include "Components/CameraFollowComponent.h"
#include "Components/HealthComponent.h"
#include "Components/KeyboardControlComponent.h"
#include "Components/ProjectileEmitterComponent.h"
#include "Components/RigidBodyComponent.h"
#include "Components/SpriteComponent.h"
#include "Components/TextLabelComponent.h"
#include "Components/TransformComponent.h"
#include "sol/sol.hpp"
#include <Game/Game.h>
#include <fstream>
#include <string>

LevelLoader::LevelLoader(){};

LevelLoader::~LevelLoader(){};

void LevelLoader::LoadLevel(sol::state& lua,
                            const std::unique_ptr<Registry>& registry,
                            const std::unique_ptr<AssetStore>& assetStore,
                            SDL_Renderer* renderer,
                            int level) {

    // check lua syntax
    sol::load_result script = lua.load_file("./assets/scripts/level" + std::to_string(level) + ".lua");
    if (!script.valid()) {
        sol::error err = script;
        Logger::Err(err.what());
        exit(1);
    }

    // execute lua script
    lua.script_file("./assets/scripts/level" + std::to_string(level) + ".lua");

    sol::table levelTable = lua["Level"];
    sol::table assetsTable = levelTable["assets"];
    int i = 0;
    while (true) {
        sol::optional<sol::table> existsAssetOpt = assetsTable[i];
        if (existsAssetOpt == sol::nullopt) {
            break;
        }

        // cpp에선 type narrowing이 없다.
        sol::table existsAsset = existsAssetOpt.value();
        std::string assetType = existsAsset["type"];

        if (assetType == "texture") {
            assetStore->AddTexture(renderer, existsAsset["id"], existsAsset["file"]);
        } else if (assetType == "font") {
            assetStore->AddFont(existsAsset["id"], existsAsset["file"], existsAsset["font_size"]);
        }

        i++;
    }

    // assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    // assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    // assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
    // assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
    // assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
    // assetStore->AddTexture(renderer, "tree-image", "./assets/images/tree.png");

    // add fonts
    // assetStore->AddFont("charriot-font-16", "./assets/fonts/charriot.ttf", 16);
    // assetStore->AddFont("arial-font-16", "./assets/fonts/arial.ttf", 16);

    // tilemap
    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
    int tileSize = 32; // 32 pixel
    double tileScale = 2.0;
    int mapNumCols = 25;
    int mapNumRows = 20;

    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");
    if (!mapFile) {
        // TODO: error handling
        exit(1);
    }

    for (int y = 0; y < mapNumRows; y++) {
        for (int x = 0; x < mapNumCols; x++) {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore(); // ','는 버려야 하므로.

            Entity tile = registry->CreateEntity();
            tile.Group("tiles");
            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
                                                  glm::vec2(tileScale, tileScale),
                                                  0.0);

            // tilemap-image의 x, y 좌표에 의해 렌더링 함.
            // tilemap-image의 배치는 다음과 같음.
            // 0 1 2 3 4 5 6 7 8 9
            // 10 11 12 13 14 15 16 17 18 19
            // 20 21 22 23 24 25 26 27 28 29
            tile.AddComponent<SpriteComponent>(
                "tilemap-image", tileSize, tileSize, ZIndex::Background, false, srcRectX, srcRectY);
        }
    }

    mapFile.close();

    Game::mMapWidth = mapNumCols * tileSize * tileScale;
    Game::mMapHeight = mapNumRows * tileSize * tileScale;

    // create entity and add component
    Entity chopper = registry->CreateEntity();
    chopper.Tag("player");
    chopper.AddComponent<TransformComponent>(glm::vec2(50.0, 50.0), glm::vec2(1.0, 1.0), 0.0);
    chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, ZIndex::Player);
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<AnimationComponent>(2, 15, true); // it has 2 frame and render five frame per sec
    chopper.AddComponent<KeyboardControlComponent>(glm::vec2(0, -200),
                                                   glm::vec2(200, 0),
                                                   glm::vec2(0, 200),
                                                   glm::vec2(-200, 0));
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<HealthComponent>(100);
    // mRepeatFrequencyMS가 0인 것은, 발사를 막는 효과를 내도록 처리함.
    chopper.AddComponent<ProjectileEmitterComponent>(
        glm::vec2(250.0, 250.0), DO_NOT_AUTO_RE_EMIT_PROJECTILE_FREQ, 10000, 10, true);

    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(glm::vec2(Game::mWindowWidth - 74, 32), glm::vec2(1.0, 1.0), 0.0);
    radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
    radar.AddComponent<SpriteComponent>("radar-image", 64, 64, ZIndex::UI, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    Entity tank = registry->CreateEntity();
    tank.Group("enemies");
    tank.AddComponent<TransformComponent>(glm::vec2(400.0, 550.0), glm::vec2(1.0, 1.0), 0.0);
    tank.AddComponent<RigidBodyComponent>(glm::vec2(-30.0, 00.0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, ZIndex::Enemy);
    tank.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 1000, 10000, 10, false);
    tank.AddComponent<HealthComponent>(100);

    Entity truck = registry->CreateEntity();
    truck.Group("enemies");
    truck.AddComponent<TransformComponent>(glm::vec2(100.0, 350.0), glm::vec2(1.0, 1.0), 0.0);
    truck.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 00.0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, ZIndex::Enemy);
    truck.AddComponent<BoxColliderComponent>(32, 32, glm::vec2(0));
    truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 1000, 2000, 10, false);
    truck.AddComponent<HealthComponent>(100);

    Entity treeA = registry->CreateEntity();
    treeA.Group("obstacles");
    treeA.AddComponent<TransformComponent>(glm::vec2(300.0, 350.0), glm::vec2(1.0, 1.0), 0.0);
    treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, ZIndex::Enemy);
    treeA.AddComponent<BoxColliderComponent>(16, 32, glm::vec2(0));

    Entity treeB = registry->CreateEntity();
    treeB.Group("obstacles");
    treeB.AddComponent<TransformComponent>(glm::vec2(50.0, 350.0), glm::vec2(1.0, 1.0), 0.0);
    treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, ZIndex::Enemy);
    treeB.AddComponent<BoxColliderComponent>(16, 32, glm::vec2(0));

    Entity label = registry->CreateEntity();
    SDL_Color green = {0, 255, 0};
    label.AddComponent<TextLabelComponent>(glm::vec2(100, 100), "CHOPPER 1.0", "arial-font-16", green, true);
}