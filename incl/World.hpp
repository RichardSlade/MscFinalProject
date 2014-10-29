#ifndef INCL_WORLD_HPP
#define INCL_WORLD_HPP

#include <string>
#include <vector>
#include <memory>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

#include <CommandQueue.hpp>
#include <Command.hpp>
#include <Enemy.hpp>
#include <Player.hpp>
#include <Collectable.hpp>
#include <Constants.hpp>
#include <Obstacle.hpp>
#include <MemoryMap.hpp>
#include <QuadTree.hpp>
#include <Particle.hpp>

class GameState;

class World
{
    private:
        sf::RenderWindow&                           mWindow;
        sf::View                                    mWorldView;
        sf::FloatRect                               mWorldBounds;
        sf::Vector2f                                mPlayerSpawnPosition;
        sf::Vector2f                                mEnemySpawnPosition;
        sf::FloatRect                               mEnemySpawnBounds;
        sf::FloatRect                               mPlayerSpawnBounds;
        sf::FloatRect                               mShootingBounds;

        GameState*                                  mGame;
        QuadTree                                    mQuadTree;
        CommandQueue                                mCommandQueue;

        Application::Type                           mAppType;
        Ai::Type                                    mAiType;
        Evolution::Type                             mEvoType;

        sf::VertexArray                             mEnemySpawnBox;
        sf::Time                                    mLevelTime;
        sf::Time                                    mCollectableCountdown;
        bool                                        mSpawnCollectable;

        bool                                        mPlayerWin;
        bool                                        mPlayerDead;
        bool                                        mMinTicksPassed;

        double                                      mAvgFitness;
        double                                      mLastAvgFitness;
        double                                      mChangeInFitness;

        int                                         mNumTicks;
        int                                         mPlayerParticleIndex;
        int                                         mEnemyHitMine;
        int                                         mRealTimeSpawned;

        int                                         mPopCount;
        int                                         mIterCount;
        int                                         mGenCount;
        int                                         mLevelCount;
        int                                         mEnemyCollected;

        int                                         mRecorded;
        int                                         mCollectableNum;
        int                                         mMineNum;
        int                                         mCollectablePool;

        std::array<sf::Texture,
                    Textures::TextureCount>&        mTextures;

        std::array<sf::SoundBuffer
                    , Sounds::Type::SoundCount>&    mSounds;

        sf::Sprite                                  mBackground;

        Player::PlayerPtr                           mPlayer;
        Enemy::EnemyPtr                             mPlayerBot;
        std::vector<NEATNeuralNet*>                 mPlayerBotBrains;
        std::vector<Enemy::EnemyPtr>                mHarvesters;
        std::vector<Enemy::EnemyPtr>                mHunters;
        std::vector<Collectable>                    mCollectables;
        std::vector<Obstacle>                       mObstacles;
        std::vector<ParticleSystem>                 mParticleSystems;


        void                                        createParticleSystem(sf::Vector2f startPos, sf::Color pColour, bool isPlayer = false);
        void                                        adaptPlayerVelocity();
        void                                        handleProjectileCollisions();
        void                                        setupQuadTree();
        void                                        handleCollisions();

//        void                                        handleCollectableCollisions();
//        void                                        handleObstacleCollisions();

        void                                        setupText(std::string brainName);
        void                                        buildWorld();
        void                                        resetLevel();
        void                                        createObstacles();
        void                                        spawnCollectable();

        void                                        positionPlayer(){ mPlayer->setPosition(mPlayerSpawnPosition); }
        void                                        positionEnemy(Enemy::EnemyPtr&);
        void                                        positionCollectable(Collectable&);

        Obstacle                                    generateQuad(std::array<sf::Color, 5> colours);
        std::vector<double>                         getFitnessScores(Ai::Type aiType);

        void                                        checkEnemyStatus();

        void                                        trainingEpoch();
        void                                        rtEpoch();
        void                                        updateGame(sf::Time dt);
        void                                        updateTraining(sf::Time dt);
        void                                        updateRtNEAT();
        void                                        gameEpoch();
        void                                        increaseEnemies();

    public:
                                                    World(sf::RenderWindow& window
                                                          , GameState* game
                                                          , std::array<sf::Texture, Textures::Type::TextureCount>& textures
                                                          , Application::Type appType
                                                          , Ai::Type aiType
                                                          , Evolution::Type evoType
                                                          , std::array<sf::SoundBuffer, Sounds::Type::SoundCount>& sounds);

        void                                        update(sf::Time timeElapsed);
        void                                        render();


        CommandQueue&                               getCommandQueue() {return mCommandQueue;}
        sf::FloatRect                               getViewBounds() const;

        int                                         getGenCount() {return mGenCount;}
        int                                         getIterCount(){ return mIterCount; }
        int                                         getLevelCount(){ return mLevelCount; }
        int                                         getCollectablePool(){ return mCollectablePool; }

        double                                      getFitness(){ return mHarvesters[0]->getFitness(); }
        int                                         getPlayerLives(){ return mPlayer->getLives(); }
        int                                         getPlayerHitPoints(){ return mPlayer->getHitPoints(); }
        int                                         getPlayerScore(){ return mPlayer->getScore(); }
        int                                         getPlayerCollected(){ return mPlayer->getCollected(); }
        int                                         getHarvesterCollected(){ return mHarvesters[0]->getCollected(); }
        int                                         getCellsVisited(){ return mHarvesters[0]->cellsVisited(); }
        int                                         getEnemyCollected(){ return mEnemyCollected; }
        int                                         getPlayerKillCount(){ return mPlayer->getKillCount(); }
        int                                         getMineHitNum(){ return mEnemyHitMine; }
        double                                      getAvgFitness(){ return mAvgFitness; }
        double                                      getLastAvgFitness(){ return mLastAvgFitness; }
        double                                      getChangeInFitness(){ return mChangeInFitness; }
        int                                         getPopCount(){ return mPopCount; }
        int                                         getRecord(){ return mRecorded; }
        int                                         getRealTimeSpawned(){ return mRealTimeSpawned; }

        void                                        drawEnemyNet(){mHarvesters[0]->drawNet();}
        sf::VertexArray&                             getNeuronVertex(){ return mHarvesters[0]->getNeuronVertex();}
        sf::VertexArray&                             getLinkVertex(){ return mHarvesters[0]->getLinkVertex();}

        std::vector<sf::CircleShape>&               getNeuronCircles(){return mHarvesters[0]->getNeuronCircles();}

        void                                        incPlayerScore(int value){ mPlayer->incScore(value); }
};

#endif // INCL_WORLD_HPP
