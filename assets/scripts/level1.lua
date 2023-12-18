print("level1.lua loaded")

Level = {
    -- bypass. lua는 index가 1부터 시작하므로 0을 키값으로써 넣어주어 0번째부터 접근할 수 있도록 함.
    assets = {
        [0] =
        { type = "texture", id = "tank-image", file = "./assets/images/tank-panther-right.png" },
        { type = "texture", id = "truck-image",      file = "./assets/images/truck-ford-right.png" },
        { type = "texture", id = "chopper-image",    file = "./assets/images/chopper-spritesheet.png" },
        { type = "texture", id = "radar-image",      file = "./assets/images/radar.png" },
        { type = "texture", id = "bullet-image",     file = "./assets/images/bullet.png" },
        { type = "texture", id = "tree-image",       file = "./assets/images/tree.png" },
        { type = "font",    id = "arial-font-16",    file = "./assets/fonts/arial.ttf",               font_size = 16 },
        { type = "font",    id = "charriot-font-16", file = "./assets/fonts/charriot.ttf",            font_size = 16 }
    },
}
