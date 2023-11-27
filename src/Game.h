#pragma once

class Game {
private:
    //

public:
    Game();
    virtual ~Game();

    void Initialize();
    void Run();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
};
