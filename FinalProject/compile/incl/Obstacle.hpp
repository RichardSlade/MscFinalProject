#ifndef INCL_OBSTACLE_HPP
#define INCL_OBSTACLE_HPP

#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <Collidable.hpp>

struct Obstacle : public Collidable
{
    public:
                                    Obstacle(sf::PrimitiveType type)
                                    : Collidable(Collidable::Type::O)
                                    , vertices(type)
                                    {}

        sf::VertexArray             vertices;
        sf::RectangleShape          rect;

        const sf::FloatRect         getBounds();

};

#endif // INCL_OBSTACLE_HPP
