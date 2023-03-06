#include <iostream>
#include <SFML/Graphics.hpp>

int main()
{
    std::cout << "Hello World!\n";
    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode()), "STE");

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
        }
    }
}