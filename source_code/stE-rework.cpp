#include <iostream>
#include "Graphics.h"

int main()
{   
    std::filesystem::create_directory("logs");
    FILE* stream;
    freopen_s(&stream, "logs/std_output.txt", "w", stdout);
    freopen_s(&stream, "logs/std_error.txt", "w", stderr);

    std::cout << "Hello World!\n";
    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode()), "STE");
    Console *con = Console::get_instance();

    auto mng = Resource_manager<sf::Texture>::get_instance();

    con->log(get_message_prefix(mng));
    con->log(get_message_prefix(&window));

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(100, 100, 100, 100));
        con->render();
        window.draw(con->sprite);
        window.display();
    }
}