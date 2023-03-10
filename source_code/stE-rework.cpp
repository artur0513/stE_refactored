#include <iostream>
#include "Graphics.h"

int main()
{   
    std::filesystem::create_directory("logs");
    FILE* stream;
    freopen_s(&stream, "logs/std_output.txt", "w", stdout);
    freopen_s(&stream, "logs/std_error.txt", "w", stderr);

    sf::RenderWindow window(sf::VideoMode(sf::VideoMode::getDesktopMode()), "STE");
    Console *con = Console::get_instance();

    auto mng = Resource_manager<sf::Texture>::get_instance();

    std::ifstream file("load_test.json");
    json j = json::parse(file);
    gr::GraphicsEngine eng;
    eng.load_from_file(j);

    while (window.isOpen()) {
        sf::Event event;

        while (window.pollEvent(event)) {

            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color(100, 100, 100, 100));
        con->render();
        eng.render();
        window.draw(eng.get_sprite());
        window.draw(con->sprite);
        window.display();
    }
}