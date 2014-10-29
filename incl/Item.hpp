#ifndef INCL_ITEM_HPP
#define INCL_ITEM_HPP

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <Collidable.hpp>
#include <Constants.hpp>

struct Item : public Collidable
{
    public:
        enum Type
        {
            Collectable,
            Mine
        };

                                Item(sf::Texture& texture, int id, Item::Type type, Collidable::Type colType)
                                : Collidable(colType)
                                , sprite(texture)
                                , type(type)
                                , id(id)
                                {
                                    sf::FloatRect bounds = sprite.getLocalBounds();
                                    sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                                };

        sf::Sprite              sprite;
        Type                    type;
        int                     id;

//        void                    setOrigin() {
//                                                sf::FloatRect bounds = sprite.getLocalBounds();
//                                                sprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
//                                            }

//        sf::FloatRect           getVertexRect() {
//                                                    sf::FloatRect sprRect = getBounds();
//                                                    sf::Vector2f origin = sprite.getOrigin();
//                                                    sf::FloatRect rect( origin.x - sprRect.width / 2.f,
//                                                                        origin.y - sprRect.height / 2.f,
//                                                                        sprRect.width,
//                                                                        sprRect.height);
//                                                }
        const sf::Sprite&       getSprite(){ return sprite; };
        const sf::FloatRect     getBounds()
                                {
                                    sf::FloatRect tempRect = sprite.getGlobalBounds();

                                    tempRect.top -= ObstacleBorder;
                                    tempRect.left -= ObstacleBorder;
                                    tempRect.width += ObstacleBorder;
                                    tempRect.height += ObstacleBorder;

                                    return tempRect;
                                }
};

#endif // INCL_ITEM_HPP
