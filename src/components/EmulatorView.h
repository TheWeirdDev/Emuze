//
// Created by alireza on 5/24/20.
//

#ifndef EMUZE_EMULATORVIEW_H
#define EMUZE_EMULATORVIEW_H
#include <SFML/Graphics/Drawable.hpp>

class EmulatorView : public sf::Drawable {
   protected:
    void draw(sf::RenderTarget &target,
              sf::RenderStates states) const override = 0;
};

#endif  // EMUZE_EMULATORVIEW_H
