#include "emuze.h"

#include <chrono>
namespace Emuze {
void startEmuze() {
    spdlog::info("Test");
    Chip8::Chip8 chip8{};
    sf::RenderWindow window(sf::VideoMode(900, 600), "SFML works!",
                            sf::Style::Titlebar | sf::Style::Close);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    window.setFramerateLimit(160);

    // Chip8::Disasm diasm{};
    chip8.openRom("/home/alireza/Downloads/PONG");

    while (window.isOpen()) {
        sf::Event event;
        window.clear();
        //        const auto [x, y] = window.getSize();
        //        auto text = sf::Text();
        //        text.setString(fmt::format("X: {}, Y:{}", x, y));
        //        sf::Font font;
        //        font.loadFromFile(
        //            "/run/media/alireza/Apps/Farsi Fonts/font1/ARIAL.TTF");
        //        text.setFont(font);
        //        text.setCharacterSize(26);
        //        text.setFillColor(sf::Color::Red);
        //        text.setPosition(0.f, 0.f);
        //        window.draw(text);
        window.draw(chip8);
        chip8.step();
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed: {
                    spdlog::info(event.key.code);
                    //                    chip8.play();
                    //                    using namespace std::chrono_literals;
                    //                    std::this_thread::sleep_for(200ms);
                    //                    chip8.stop();

                    break;
                }
                default:
                    break;
            }
        }
        //		if
        //(sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
        //{
        //
        //
        //			//}
        //
        //		}

        //  window.draw(shape);
        window.display();
    }
}
}  // namespace Emuze
