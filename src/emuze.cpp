#include "emuze.h"

namespace Emuze {
void startEmuze(const std::string &rom) {
    Chip8::Chip8 chip8{};
    sf::RenderWindow window(sf::VideoMode(900, 600), "Emuze",
                            sf::Style::Titlebar | sf::Style::Close);

    // Chip8 runs about 1000 instructions per second.
    // This should change depending on what hardware Emuze is emulating
    window.setFramerateLimit(1000);
    chip8.openRom(rom);

    while (window.isOpen()) {
        sf::Event event{};

        window.clear();
        window.draw(chip8);
        window.display();
        chip8.step();

        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    chip8.finish();
                    window.close();
                    break;
                case sf::Event::KeyPressed: {
                    chip8.setPressedKey(event.key.code);
                    break;
                }
                case sf::Event::KeyReleased: {
                    chip8.setReleasedKey();
                    break;
                }
                default:
                    break;
            }
        }
    }
}
}  // namespace Emuze
