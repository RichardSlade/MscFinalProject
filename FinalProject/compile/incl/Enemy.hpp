#ifndef INCL_ENEMY_HPP
#define INCL_ENEMY_HPP

#include <vector>
#include <set>
#include <memory>

#include <Entity.hpp>
#include <Player.hpp>
#include <Collectable.hpp>
#include <Obstacle.hpp>
#include <NEATPhenotype.hpp>

class Enemy : public Entity
{
    public:
        typedef std::unique_ptr<Enemy> EnemyPtr;

    private:
        enum Outputs
        {
            RotationLeft,
            RotationRight,
            Thrust,
            Fire
        };

        enum SensorType
        {
            Left,
            DiagLeft,
            Front,
            DiagRight,
            Right,
            NotCollided
        };

        NEATNeuralNet*                  mNEATBrain;
        MemoryMap                       mMap;
        sf::FloatRect                   mShootingBounds;

        int                             mTargetID;
        int                             mOldTargetID;

        float                           mMagnitudeToTarget;
        float                           mOldMagnitudeToTarget;
        sf::Vector2f                    mOldTargetPos;

        float                           mMagnitudeToMine;

        double                          mAngle;
        double                          mFitness;
        int                             mID;
        bool                            mTargetInFront;

        bool                            mWallCollided;
        bool                            mMineCollided;
        float                           mCollisionRotation;
        bool                            mFireProjectile;

        std::vector<sf::Vector2f>       mLongSensors;
        std::vector<sf::Vector2f>       mShortSensors;
        std::vector<double>             mLongSensorReadings;
//        std::vector<double>             mMineSensorReadings;
        std::vector<double>             mMapSensorReadings;
        std::vector<double>             mShortSensorReadings;
        std::vector<bool>               mSensorsCollision;

        sf::VertexArray                 mLongSensorLines;
        sf::VertexArray                 mShortSensorLines;
        sf::VertexArray                 mCollectableLine;
        sf::VertexArray                 mMineLine;
        sf::VertexArray                 mPlayerLine;

        sf::Vector2f                    nearestCollectable(std::vector<Collectable>& items, std::vector<Obstacle>& obstacles, bool& notFound);
        double                          collectableInput(std::vector<Collectable>& items, std::vector<Obstacle>& obstacles, bool& notFound);

        sf::Vector2f                    nearestBot(std::vector<Obstacle>& obstacles, std::vector<EnemyPtr>& bot, bool& notFound);
        double                          botInput(std::vector<Obstacle>& obstacles, std::vector<EnemyPtr>& bot, bool& notFound);

        double                          playerInput(std::vector<Obstacle>& obstacles, Player::PlayerPtr&, bool&);
        double                          playerInput(std::vector<Obstacle>& obstacles, Enemy::EnemyPtr&, bool&);
        sf::Vector2f                    nearestPlayer(std::vector<Obstacle>&, sf::Vector2f playerPos, bool&);

        void                            setupSensors();
        void                            testSensors(std::vector<Obstacle> obstacles, std::vector<EnemyPtr>& enemies);

    public:
                                        Enemy(sf::Texture& sTex, EntityType type, NEATNeuralNet* net, int id, sf::SoundBuffer& soundBuf1, sf::SoundBuffer& soundBuf2, sf::FloatRect shootingBounds)
                                        : Entity(sTex, type, Collidable::Type::E, soundBuf1, soundBuf2)
                                        , mNEATBrain(net)
                                        , mMap(WorldX, WorldY, 10)
                                        , mShootingBounds(shootingBounds)
                                        , mFitness(0)
                                        , mID(id)
                                        , mOldTargetPos(sf::Vector2f(0.f, 0.f))
                                        , mOldMagnitudeToTarget(WorldX * WorldY)
                                        , mWallCollided(false)
                                        , mMineCollided(false)
                                        , mCollisionRotation(0.f)
                                        , mLongSensorLines(sf::Lines)
                                        , mShortSensorLines(sf::Lines)
                                        , mCollectableLine(sf::Lines)
                                        , mMineLine(sf::Lines)
                                        , mPlayerLine(sf::Lines)
                                        {};

        void                            reset();

        void                            incFitness(double amount) {mFitness += amount;}
        void                            setFitness(double value){mFitness = value;}
        double                          getFitness() { return mFitness; }
        int                             getID() { return mID; }
        void                            setID(int value){ mID = value; }

        void                            implantBrain(NEATNeuralNet* brain){ mNEATBrain = brain; }
        void                            resetMap(){ mMap.reset(); }

        void                            update(sf::Time dt
                                               , std::vector<Collectable>& collectables
                                               , std::vector<Obstacle>& obstacles
                                               , std::vector<EnemyPtr>& enemies
                                               , int numIn
                                               , int numOut);

        void                            update(sf::Time dt
                                               , std::vector<Collectable>& collectables
                                               , std::vector<Obstacle>& obstacles
                                               , EnemyPtr& player
                                               , std::vector<EnemyPtr>& enemies2
                                               , sf::FloatRect viewBounds
                                               , int numIn
                                               , int numOut);

        void                            update(sf::Time dt
                                               , std::vector<Collectable>& collectables
                                               , std::vector<Obstacle>& obstacles
                                               , Player::PlayerPtr& player
                                               , std::vector<EnemyPtr>& enemies
                                               , sf::FloatRect viewBounds
                                               , int numIn
                                               , int numOut);

        sf::VertexArray&                getLongSensorLines() { return mLongSensorLines; }
        sf::VertexArray&                getShortSensorLines() { return mShortSensorLines; }
        sf::VertexArray&                getCollectableLine() { return mCollectableLine; }
        sf::VertexArray&                getMineLine() { return mMineLine; }
        sf::VertexArray&                getPlayerLine(){ return mPlayerLine; }
        int                             cellsVisited(){ return mMap.cellsVisited(); }
        void                            setTexture(sf::Texture& texture){ mSprite.setTexture(texture); }

//        void                            drawNet(sf::FloatRect dim){mNEATBrain->DrawNet(dim.left, dim.left + dim.width, dim.top, dim.top + dim.height);}
        void                            drawNet(){mNEATBrain->DrawNet(200, 640, 200, 480);}
        sf::VertexArray&                 getNeuronVertex(){return mNEATBrain->networkNeurons(); }
        sf::VertexArray&                 getLinkVertex(){return mNEATBrain->networkLinks(); }
        std::vector<sf::CircleShape>&    getNeuronCircles(){return mNEATBrain->neuronCircles();}

};



#endif // INCL_ENEMY_HPP
