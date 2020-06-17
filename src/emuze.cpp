#include "emuze.h"
namespace Emuze {
void startEmuze() {
    spdlog::info("Test");
    Chip8::Chip8 chip8{};
    sf::RenderWindow window(sf::VideoMode(900, 600), "SFML works!",
                            sf::Style::Titlebar | sf::Style::Close);
    sf::CircleShape shape(100.f);
    shape.setFillColor(sf::Color::Green);
    window.setFramerateLimit(60);

    Chip8::Disasm diasm{};
    std::ifstream in_file("../15 Puzzle [Roger Ivie].ch8", std::ios::in | std::ios::binary);
    std::array<unsigned char, 2> word;

    while (in_file.read((char *)word.data(), 2)) {
        std::cout << diasm.disassemble(sf::Uint16(word[0]) << 8 | word[1]) << '\n';
    }
    while (window.isOpen()) {
        sf::Event event;
        window.clear();

        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::KeyPressed: {
                    break;
                }
                default:
                    break;
            }
        }
        //		if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
        //
        //
        //			//}
        //
        //		}
        const auto [x, y] = window.getSize();
        auto text = sf::Text();
        text.setString(fmt::format("X: {}, Y:{}", x, y));
        sf::Font font;
        font.loadFromFile("/run/media/alireza/Apps/Farsi Fonts/font1/ARIAL.TTF");
        text.setFont(font);
        text.setCharacterSize(26);
        text.setFillColor(sf::Color::Red);
        text.setPosition(0.f, 0.f);
        window.draw(text);
        //  window.draw(shape);
        window.display();
    }
}
}  // namespace Emuze