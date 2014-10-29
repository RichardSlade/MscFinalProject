#include <Obstacle.hpp>
#include <Constants.hpp>

const sf::FloatRect Obstacle::getBounds()
{
    sf::FloatRect tempRect = rect.getGlobalBounds();

    tempRect.top -= ObstacleBorder;
    tempRect.left -= ObstacleBorder;
    tempRect.width += ObstacleBorder;
    tempRect.height += ObstacleBorder;

    return tempRect;
}
