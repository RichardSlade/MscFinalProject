#ifndef INCL_ENTITY_HPP
#define INCL_ENTITY_HPP

#include <array>

//#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Audio/Sound.hpp>

#include <Obstacle.hpp>
#include <Projectile.hpp>
#include <Collidable.hpp>
#include <Constants.hpp>
#include <Enum.hpp>
#include <MemoryMap.hpp>

class Entity : public Collidable
{
    public:

        enum EntityType
        {
            Player,
            PlayerBot,
            Enemy
        };

    protected:
        EntityType                      mType;
        sf::Vector2f                    mVelocity;
        sf::Vector2f                    mOldPosition;
        sf::Sprite                      mSprite;

        sf::Sound                       mLaserSound;
        sf::Sound                       mDeathSound;

        int                             mHitPoints;
        int                             mCollected;
        bool                            mFireProjectile;

        std::vector<Projectile>         mProjectiles;
        sf::Time                        mFireCountdown;

        void                            checkProjectileLaunch(sf::Time dt, sf::FloatRect viewBounds);
        void                            fireProjectile(sf::FloatRect viewBounds);
        void                            updateProjectiles(sf::Time dt);

    public:
                                        Entity(sf::Texture& sTexture, EntityType type, Collidable::Type colType, sf::SoundBuffer& soundBuf1, sf::SoundBuffer& soundBuf2)
                                        : Collidable(colType)
                                        , mSprite(sTexture)
                                        , mDeathSound(soundBuf1)
                                        , mLaserSound(soundBuf2)
                                        , mType(type)
                                        , mCollected(0)
                                        , mFireProjectile(false)
                                        , mFireCountdown(sf::Time::Zero)
                                        {
                                            sf::FloatRect bounds = mSprite.getLocalBounds();
                                            mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

                                            switch(type)
                                            {
                                                case EntityType::Player: mHitPoints = PlayerHitPoints; break;
                                                case EntityType::Enemy: mHitPoints = EnemyHitPoints; break;
                                                default: break;
                                            }

                                            mLaserSound.setVolume(75.f);
                                            mDeathSound.setVolume(75.f);
                                        };

        virtual void                    accelerate(float accleration, sf::Time dt){};
        virtual void                    update(sf::Time dt){};
        virtual void                    destroy();
        virtual void                    reset();

//        void                            setLaserSound(sf::SoundBuffer& sound){ mLaserSound.setBuffer(sound); mLaserSound.setVolume(50.f);}
        void                            playDeathSound(){ mDeathSound.play(); }
        void                            fire() { mFireProjectile = true; };

        EntityType                      getType(){ return mType; }
        int                             getHitPoints() { return mHitPoints; }
        void                            changeHitPoints(int value) { mHitPoints += value; }

        float                           getRotation(){ return mSprite.getRotation(); }
        void                            setRotation(float rotation){ mSprite.setRotation(rotation); }
        void                            rotate(float rotation){ mSprite.rotate(rotation); }

        void                            setPosition(sf::Vector2f pos){ mSprite.setPosition(pos); }
        const sf::Vector2f              getPosition(){ return mSprite.getPosition(); }

        void                            setVelocity(sf::Vector2f vel){ mVelocity = vel; }
        void                            setVelocity(float x, float y){ mVelocity = sf::Vector2f(x, y); }
        sf::Vector2f                    getVelocity(){ return mVelocity; }
        void                            reverseMove();

        void                            incCollected(){ mCollected ++; }
        void                            setCollected(int value){ mCollected = value; }
        int                             getCollected(){ return mCollected; }

        const sf::FloatRect             getBounds();
        const sf::FloatRect             getSpriteBounds() { return mSprite.getGlobalBounds(); }
        const sf::Sprite&               getSprite(){ return mSprite; }
        sf::Sprite&                     getNonConstSprite(){ return mSprite; }

        std::vector<Projectile>&        getProjectiles() { return mProjectiles; }
};


#endif // INCL_ENTITY_HPP
