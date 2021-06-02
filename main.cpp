#include <cstdlib>
#include "Game.h"
#include "Menu.h"


int main(int argc, char** argv)
{
    Menu menu;
    bool success = menu.Initialize();

    if(success)
    {
        menu.RunLoop();
    }

    menu.Shutdown();
    return 0;
}