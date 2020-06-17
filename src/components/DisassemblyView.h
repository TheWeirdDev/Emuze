//
// Created by alireza on 5/24/20.
//

#ifndef EMUZE_DISASSEMBLYVIEW_H
#define EMUZE_DISASSEMBLYVIEW_H

#include <SFML/Graphics/Drawable.hpp>

class DisassemblyView : public sf::Drawable {
   protected:
    void draw(sf::RenderTarget &target, sf::RenderStates states) const override = 0;
};

#endif  //EMUZE_DISASSEMBLYVIEW_H
