#include "Game.h"
#include "Menu.h"

int main(int , char**)
{
    Menu menu;

    if(!menu.Initialize())
        return EXIT_FAILURE;

    menu.RunLoop();
    menu.Shutdown();

    return EXIT_SUCCESS;
}