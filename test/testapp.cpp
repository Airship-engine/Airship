#include "core/application.h"

class GameClass : public Airship::Application
{
protected:
    void OnStart() override;
};

void GameClass::OnStart()
{
    std::optional<Airship::Window*> win = CreateWindow(800, 600);
    assert(win.has_value());

    std::cout << "Could render text now" << std::endl;
}

int main()
{ 
    GameClass app;
    app.Run();
}
