#include "core/logging.h"
#include "game.h"

int main() 
{
    SHIPLOG_INFO("Hello, World!");

    Game game;
    game.Run();

}
