//
// Created by alireza on 10/3/21.
//

#ifndef EMUZE_MENU_H
#define EMUZE_MENU_H

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include "emulators/chip8/Chip8.h"
#include "emuze.h"

using namespace Emuze::Chip8;

class Menu final : public sf::Drawable {
    void draw(sf::RenderTarget &target,
              sf::RenderStates states) const override {
        sf::Text text;
        text.setFillColor(sf::Color::Green);
        text.setFont(Emuze::forward_font);
        text.setString("WELCOME TO EMUZE");
        text.setCharacterSize(42);
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width / 2.0f,
                       textRect.top + textRect.height / 2.0f);
        text.setPosition(sf::Vector2f(
            Emuze::SCREEN_WIDTH / 2.0f,
            Emuze::SCREEN_HEIGHT / 2.0f - Emuze::SCREEN_HEIGHT / 4.0));
        target.draw(text);

        text.setPosition(
            sf::Vector2f(Emuze::SCREEN_WIDTH / 2.0f + Emuze::SCREEN_WIDTH / 8.0,
                         Emuze::SCREEN_HEIGHT / 2.0f));
        text.setCharacterSize(24);
        text.setString(
            "[O] Open a game file\n\n"
            "[C] Change color\n\n"
            "[Q] Quit");
        target.draw(text);
    }
};

#endif  // EMUZE_MENU_H
