#ifndef INCL_PLAYER_HPP
#define INCL_PLAYER_HPP

#include <memory>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>

#include <Entity.hpp>
#include <Command.hpp>
#include <Obstacle.hpp>

class Player : public Entity
{
    public:
        typedef std::unique_ptr<Player> PlayerPtr;

    private:
        int                             mKillCount;
        int                             mHarvesterKillCount;
        int                             mHunterKillCount;
        int                             mLives;
        int                             mScore;
        sf::Sound                       mCollectSound;
        bool                            mHasMoved;

    public:
                                        Player(sf::Texture& sTexture, EntityType type, sf::SoundBuffer& soundBuf1, sf::SoundBuffer& soundBuf2, sf::SoundBuffer& soundBuf3)
                                        : Entity(sTexture, type, Collidable::Type::P, soundBuf2, soundBuf3)
                                        , mLives(PlayerLives)
                                        , mScore(0)
                                        , mKillCount(0)
                                        , mHarvesterKillCount(0)
                                        , mHunterKillCount(0)
                                        , mCollectSound(soundBuf1)
                                        , mHasMoved(false)
                                        {
                                            mDeathSound.setVolume(50.f);
                                            mCollectSound.setVolume(50.f);
                                        };

        int                             getKillCount(){ return mKillCount; }
        void                            setKillCount(int value){ mKillCount = value; }
        void                            incKillCount(){ mKillCount ++; }

        int                             getHarvesterKillCount(){ return mHarvesterKillCount; }
        void                            setHarvesterKillCount(int value){ mHarvesterKillCount = value; }
        void                            incHarvesterKillCount(){ mHarvesterKillCount ++; }

        int                             getHunterKillCount(){ return mHunterKillCount; }
        void                            setHunterKillCount(int value){ mHunterKillCount = value; }
        void                            incHunterKillCount(){ mHunterKillCount ++; }

        int                             getLives(){ return mLives; }
        void                            incLives(){ mLives ++; }
        void                            decLives(){ mLives --; }

        void                            incScore(int value){ mScore += value; if(mScore < 0) mScore = 0; }
        void                            setScore(int value){ mScore = value; }
        int                             getScore(){ return mScore; }

        void                            onCommand(const Command& command, sf::Time dt);
        void                            accelerate(float accleration, sf::Time dt);

        void                            update(sf::Time dt);

        void                            playCollect(){ mCollectSound.play(); }
        void                            reset();

};

#endif // INCL_PLAYER_HPP
