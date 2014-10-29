#ifndef INCL_PROJECTILE_HPP
#define INCL_PROJECTILE_HPP

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Color.hpp>

#include <Collidable.hpp>

struct Projectile// : public Collidable
{
    public:
                                Projectile(sf::Vector2f spawnPos, float rotation, sf::Vector2f vel)
                                //: Collidable(Collidable::Type::Projectile)
                                : projectile(sf::Vector2f(4.f, 5.f))
                                , velocity(vel)
                                , isActive(true)
                                {
                                    sf::FloatRect bounds = projectile.getLocalBounds();
                                    projectile.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                                    projectile.setFillColor(sf::Color::Yellow);
                                    projectile.setPosition(spawnPos);
                                    projectile.rotate(rotation);
                                }

                                Projectile()
//                                : Collidable(Collidable::Type::Projectile)
                                : isActive(false)
                                {
                                    sf::FloatRect bounds = projectile.getLocalBounds();
                                    projectile.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                                    projectile.setFillColor(sf::Color::Yellow);
                                };

        sf::Vector2f            velocity;
        sf::RectangleShape      projectile;
        bool                    isActive;

        sf::FloatRect           getBounds() {
                                                sf::FloatRect bounds = projectile.getGlobalBounds();

                                                float border = 4.f;

                                                bounds.left -= border;
                                                bounds.top -= border;
                                                bounds.width += border;
                                                bounds.height += border;

                                                return bounds;
                                            }

};

#endif // INCL_PROJECTILE_HPP
