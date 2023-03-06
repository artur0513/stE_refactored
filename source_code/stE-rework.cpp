#include <iostream>
#include "Graphics.h"

int main()
{
    std::cout << "Hello World!\n";
    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode()), "STE");

    auto mng = Resource_manager<sf::Texture>::get_instance();
    mng->get_object("1.png");

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
}