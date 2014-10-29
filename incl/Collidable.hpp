#ifndef INCL_COLLIDABLE_HPP
#define INCL_COLLIDABLE_HPP

#include <memory>

#include <SFML/Graphics/Rect.hpp>

struct Collidable
{
    public:
        typedef std::unique_ptr<Collidable> CollidablePtr;

        enum Type
        {
            C,
            M,
            P,
            E,
            O
        };

                            Collidable(Collidable::Type type)
                            : collidableType(type)
                            , moved(false)
                            , collected(false)
                            {};

                            Collidable(Collidable::Type type, sf::FloatRect bounds)
                            : collidableType(type)
                            , moved(false)
                            , collected(false)
                            , collideBounds(bounds)
                            {};

        Collidable::Type    collidableType;
        sf::FloatRect       collideBounds;
        bool                moved;
        bool                collected;
};

#endif // INCL_COLLIDABL_HPP
