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