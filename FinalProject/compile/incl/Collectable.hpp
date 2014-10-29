#ifndef INCL_COLLECTABLE_HPP
#define INCL_COLLECTABLE_HPP

#include <Item.hpp>

class Collectable : public Item
{
    public:
                                Collectable(sf::Texture& texture, int id)
                                : Item(texture, id, Item::Type::Collectable, Collidable::Type::C)
                                {}
};

#endif // INCL_COLLECTABLE_HPP
