#include "emuze.h"

#include "../libs/nativefiledialog/src/include/nfd.h"
#include "menu.h"

namespace Emuze {
int startEmuze() {
    Chip8::Chip8 chip8{};
    Menu menu{};
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Emuze",
                            sf::Style::Titlebar | sf::Style::Close);

    // Chip8 runs about 1000 instructions per second.
    // This should change depending on what hardware Emuze is emulating
    window.setFramerateLimit(60);
    bool runningGame = false;

    while (window.isOpen()) {
        sf::Event event{};

        window.clear();

        runningGame = !chip8.isFinished();

        if (runningGame) {
            for (int i = 0; i < STEPS_PER_FRAME; i++) {
                chip8.step();
                window.draw(chip8);
            }
        } else {
            window.draw(menu);
        }
        window.display();

        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    chip8.finish();
                    window.close();
                    break;
                case sf::Event::KeyPressed: {
                    if (!runningGame) {
                        switch (event.key.code) {
                            case sf::Keyboard::Key::O: {
                                nfdchar_t *outPath = nullptr;
                                nfdresult_t result =
                                    NFD_OpenDialog(nullptr, nullptr, &outPath);

                                if (result == NFD_OKAY) {
                                    puts("Success!");
                                    chip8.openRom(outPath);
                                    free(outPath);
                                    runningGame = true;
                                } else if (result == NFD_CANCEL) {
                                    puts("User pressed cancel.");
                                } else {
                                    printf("Error: %s\n", NFD_GetError());
                                }
                                break;
                            }
                            case sf::Keyboard::Key::Q:
                                return 0;
                            default:
                                break;
                        }
                    } else {
                        chip8.setPressedKey(event.key.code);
                    }
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
    return 0;
}
}  // namespace Emuze
