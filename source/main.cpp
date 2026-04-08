// Description: Main driver file for the casino game UI application.
// Related Files: MainUI.h, MainUI.cpp
// Date Created: 3/29/2026
// Last Edited: 4/7/2026

#include <SFML/Graphics.hpp>
#include "UIheaders/MainUI.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode({ 1920, 1080 }), "Congo Clash Casino");
    window.setFramerateLimit(144);

    MainUI ui;
    ui.run(window);

    return 0;
}