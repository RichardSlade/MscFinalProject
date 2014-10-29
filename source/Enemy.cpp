#include <cmath>

#include <Enemy.hpp>
#include <Constants.hpp>
#include <Utility.hpp>

void Enemy::reset()
{
    Entity::reset();
    mCollected = 0;
    mHitPoints = EnemyHitPoints;
}

void Enemy::setupSensors()
{
    mLongSensors.clear();
    mShortSensors.clear();
    mLongSensorLines.clear();
    mShortSensorLines.clear();

    double currentRotation = (mSprite.getRotation() * (3.14159265 / 180));

    double angleInc = 45.0 * (3.14159265 / 180);
    double angle = -90.0 * (3.14159265 / 180);
    sf::Vector2f pos  = mSprite.getPosition();

    for(int i = 0; i < NumSensors; i++)
    {
        sf::Vector2f sensorPoint;

        sensorPoint.x = sin(currentRotation) + pos.x;
        sensorPoint.y = -cos(currentRotation) + pos.y;
        mLongSensorLines.append(sf::Vertex(sensorPoint, sf::Color::Green));
        mShortSensorLines.append(sf::Vertex(sensorPoint, sf::Color::Blue));

        sensorPoint.x = (sin(currentRotation - angle) * (LongSensorRange)) + pos.x;
        sensorPoint.y = (-cos(currentRotation - angle) * (LongSensorRange)) + pos.y;
        mLongSensorLines.append(sf::Vertex(sensorPoint, sf::Color::Green));
        mLongSensors.push_back(sensorPoint);

        sensorPoint.x = (sin(currentRotation - angle) * ShortSensorRange) + pos.x;
        sensorPoint.y = (-cos(currentRotation - angle) * ShortSensorRange) + pos.y;
        mShortSensorLines.append(sf::Vertex(sensorPoint, sf::Color::Blue));
        mShortSensors.push_back(sensorPoint);

        angle += angleInc;
    }
}

void Enemy::testSensors(std::vector<Obstacle> obstacles,  std::vector<EnemyPtr>& enemies)
{
    mWallCollided = false;
    mMineCollided = false;

    int numCollided = 0;
    int numFrontCollided = 0;

    setupSensors();

    mLongSensorReadings.clear();
    mShortSensorReadings.clear();
    mSensorsCollision.clear();
    mMapSensorReadings.clear();

    sf::Vector2f pos = mSprite.getPosition();

    //now to check each sensor against the objects in the world
    for (int sens = 0; sens < NumSensors; sens++)
    {
        bool hit = false;
        bool sensCollided = false;
        double dist = 0;

        // Calculate long sensor readings
        for(Obstacle& ob : obstacles)
        {
            int numVert = ob.vertices.getVertexCount();
            int nextVert = 0;

            for(int i = 0; i < numVert; i++)
            {
                if(i == numVert - 1) nextVert = 0;
                else nextVert = i + 1;

                if(lineIntersection(pos, mLongSensors[sens], ob.vertices[i].position, ob.vertices[nextVert].position, dist))
                {
                    hit = true;
                    break;
                }
            }

            if(hit) break;
        }

        if(hit)
        {
            mLongSensorReadings.push_back(dist);

            if (dist < CollisionDist)
            {
                mLongSensorLines[sens * 2] = sf::Vertex(mLongSensorLines[sens * 2].position, sf::Color::Red);
                mLongSensorLines[(sens * 2) + 1] = sf::Vertex(mLongSensorLines[(sens * 2) + 1].position, sf::Color::Red);

                mWallCollided = true;
                sensCollided = true;
            }
        }
        else
        {
            mLongSensorReadings.push_back(-1);
        }

        mSensorsCollision.push_back(sensCollided);

        // Calculate map readings
        if((mLongSensors[sens].x < BorderSize) || (mLongSensors[sens].x > WorldX - BorderSize)
           || (mLongSensors[sens].y < BorderSize) || (mLongSensors[sens].y > WorldY - BorderSize))
        {
            mMapSensorReadings.push_back(1);
        }
        else
        {
            int x = mLongSensors[sens].x / CellDivider;
            int y = mLongSensors[sens].y / CellDivider;

            int ticksInBlock = mMap.getMapReading(x, y);

            if(!mSensorsCollision[sens])
            {
                if(ticksInBlock == 0)
                {
                    mMapSensorReadings.push_back(0);
                }
                else if(ticksInBlock < 10)
                {
                    mMapSensorReadings.push_back(0.1);
                }
                else if(ticksInBlock < 20)
                {
                    mMapSensorReadings.push_back(0.3);
                }
                else if(ticksInBlock < 30)
                {
                    mMapSensorReadings.push_back(0.5);
                }
                else if(ticksInBlock < 50)
                {
                    mMapSensorReadings.push_back(0.7);
                }
                else if(ticksInBlock < 80)
                {
                    mMapSensorReadings.push_back(0.8);
                }
                else
                {
                    mMapSensorReadings.push_back(0.9);
                }
            }
            else
            {
                mMapSensorReadings.push_back(1);
            }
        }
    }
}

void Enemy::update(sf::Time dt, std::vector<Collectable>& collectables, std::vector<Obstacle>& obstacles, std::vector<EnemyPtr>& enemies, int numIn, int numOut)
{
    std::vector<double> brainInputs;

    testSensors(obstacles, enemies);

    sf::Vector2f pos = mSprite.getPosition();

    int x = static_cast<int>(pos.x / CellDivider);
    int y = static_cast<int>(pos.y / CellDivider);

    for(double& reading : mLongSensorReadings)
    {
        brainInputs.push_back(reading);
    }

    mOldMagnitudeToTarget = mMagnitudeToTarget;
    mOldTargetID = mTargetID;
//
    bool notFound;

    brainInputs.push_back(collectableInput(collectables, obstacles, notFound));

    std::vector<double> outputs;

    if(brainInputs.size() != numIn)
        throw std::runtime_error("Error with network inputs size:\nWas: " + std::to_string(brainInputs.size()) + "\nShould be: " + std::to_string(numIn));

    outputs = mNEATBrain->Update(brainInputs);

    // Make sure there were no errors in calculating output by checking size
    if(outputs.size() != numOut)
        throw std::runtime_error("Error with network outputs size:\nWas: " + std::to_string(outputs.size()) + "\nShould be: " + std::to_string(numOut));

    double rotationAngle = (outputs[RotationLeft] - outputs[RotationRight]);

    float speed = (outputs[RotationRight] + outputs[RotationLeft]);


    // Clamp rotation
    Clamp(rotationAngle, -MaxTurnRate, MaxTurnRate);

    mSprite.rotate(rotationAngle * (180 / 3.14159265));

    float currentRotation = mSprite.getRotation() * (3.14159265 / 180);

    mVelocity.x = sin(currentRotation);
    mVelocity.y = -cos(currentRotation);

    mOldPosition = mSprite.getPosition();

    float scaledSpeed = 0.01;

    mSprite.move(mVelocity * speed);
}

void Enemy::update(sf::Time dt, std::vector<Collectable>& collectables, std::vector<Obstacle>& obstacles, EnemyPtr& player, std::vector<EnemyPtr>& enemies, sf::FloatRect viewBounds, int numIn, int numOut)
{
    std::vector<double> brainInputs;

    testSensors(obstacles, enemies);

    sf::Vector2f pos = mSprite.getPosition();

    int x = static_cast<int>(pos.x / CellDivider);
    int y = static_cast<int>(pos.y / CellDivider);

    if(!getBounds().intersects(mShootingBounds))
    {
        if(mMap.visitCell(x, y))
            mFitness ++;
    }

    for(double& reading : mLongSensorReadings)
    {
        brainInputs.push_back(reading);
    }

    for(double& reading : mMapSensorReadings)
    {
        brainInputs.push_back(reading);
    }

    mOldMagnitudeToTarget = mMagnitudeToTarget;
    mOldTargetID = mTargetID;

    bool notFound;

    brainInputs.push_back(playerInput(obstacles, player, notFound));
    brainInputs.push_back(mTargetInFront);

    std::vector<double> outputs;

    if(brainInputs.size() < numIn)
        throw std::runtime_error("Error with network inputs size:\nWas: " + std::to_string(brainInputs.size()) + "\nShould be: " + std::to_string(numIn));

    outputs = mNEATBrain->Update(brainInputs);

    // Make sure there were no errors in calculating output by checking size
    if(outputs.size() < numOut)
        throw std::runtime_error("Error with network outputs size:\nWas: " + std::to_string(outputs.size()) + "\nShould be: " + std::to_string(numOut));

    double rotationAngle = (outputs[RotationLeft] - outputs[RotationRight]);

    float speed = (outputs[RotationRight] + outputs[RotationLeft]);

    // Clamp rotation
    Clamp(rotationAngle, -MaxTurnRate, MaxTurnRate);

    mSprite.rotate(rotationAngle * (180 / 3.14159265));

    float currentRotation = mSprite.getRotation() * (3.14159265 / 180);

    mVelocity.x = sin(currentRotation);
    mVelocity.y = -cos(currentRotation);

    mOldPosition = mSprite.getPosition();

    float scaledSpeed = 0.01;

    mSprite.move(mVelocity * speed);


    if(!getBounds().intersects(mShootingBounds))
    {
        if(!notFound
            && mTargetInFront
            && mMagnitudeToTarget < 500.f)
        {
            fire();
        }

        if(mTargetInFront
           && !notFound)
        {
            mFitness ++;
        }

        if(mMagnitudeToTarget < 500.f)
            mFitness ++;
    }

    checkProjectileLaunch(dt, viewBounds);
}

void Enemy::update(sf::Time dt, std::vector<Collectable>& collectables, std::vector<Obstacle>& obstacles, Player::PlayerPtr& player, std::vector<EnemyPtr>& enemies, sf::FloatRect viewBounds, int numIn, int numOut)
{
    std::vector<double> brainInputs;

    testSensors(obstacles, enemies);

    sf::Vector2f pos = mSprite.getPosition();

    int x = static_cast<int>(pos.x / CellDivider);
    int y = static_cast<int>(pos.y / CellDivider);

    if(!getBounds().intersects(mShootingBounds))
    {
        if(mMap.visitCell(x, y))
            mFitness ++;
    }

    for(double& reading : mLongSensorReadings)
    {
        brainInputs.push_back(reading);
    }

    for(double& reading : mMapSensorReadings)
    {
        brainInputs.push_back(reading);
    }

    mOldMagnitudeToTarget = mMagnitudeToTarget;
    mOldTargetID = mTargetID;

    bool notFound;

    brainInputs.push_back(playerInput(obstacles, player, notFound));
    brainInputs.push_back(mTargetInFront);

    std::vector<double> outputs;

    if(brainInputs.size() < numIn)
        throw std::runtime_error("Error with network inputs size:\nWas: " + std::to_string(brainInputs.size()) + "\nShould be: " + std::to_string(numIn));

    outputs = mNEATBrain->Update(brainInputs);

    // Make sure there were no errors in calculating output by checking size
    if(outputs.size() < numOut)
        throw std::runtime_error("Error with network outputs size:\nWas: " + std::to_string(outputs.size()) + "\nShould be: " + std::to_string(numOut));


    double rotationAngle = (outputs[RotationLeft] - outputs[RotationRight]);

    float speed = (outputs[RotationRight] + outputs[RotationLeft]);

    // Clamp rotation
    Clamp(rotationAngle, -MaxTurnRate, MaxTurnRate);

    mSprite.rotate(rotationAngle * (180 / 3.14159265));

    float currentRotation = mSprite.getRotation() * (3.14159265 / 180);

    mVelocity.x = sin(currentRotation);
    mVelocity.y = -cos(currentRotation);

    mOldPosition = mSprite.getPosition();

    float scaledSpeed = 0.01;

    mSprite.move(mVelocity * speed);

    if(!getBounds().intersects(mShootingBounds))
    {
        if(!notFound
            && mTargetInFront
            && mMagnitudeToTarget < 500.f)
        {
            fire();
        }

        if(mTargetInFront
           && !notFound)
        {
            mFitness ++;
        }

        if(mMagnitudeToTarget < 500.f)
            mFitness ++;
    }

    checkProjectileLaunch(dt, viewBounds);
}

double Enemy::collectableInput(std::vector<Collectable>& items, std::vector<Obstacle>& obstacles, bool& notFound)
{
    sf::Vector2f targetVector = nearestCollectable(items, obstacles, notFound);

    double mAngle = (std::atan2(targetVector.y, targetVector.x) - std::atan2(mVelocity.y, mVelocity.x)) * 180 / 3.14159265;
    mTargetInFront = false;

    if(mAngle > -10.0
       && mAngle < 10.0)
       mTargetInFront = true;

    double dot = vecDot(mVelocity, targetVector);
    int sign = vecSign(mVelocity, targetVector);

    return dot * sign;
}

sf::Vector2f Enemy::nearestCollectable(std::vector<Collectable>& items, std::vector<Obstacle>& obstacles, bool& notFound)
{
    mCollectableLine.clear();

    mCollectableLine.append(sf::Vertex(mSprite.getPosition(), sf::Color::Green));
    mCollectableLine.append(sf::Vertex(sf::Vector2f(0.f, 0.f), sf::Color::Green));

    sf::Vector2f bestVec(0.f, 0.f);
    sf::Vector2f vecToItem(0.f, 0.f);

    mMagnitudeToTarget = WorldX * WorldY;

    float bestMagnitude = WorldX * WorldY;
    float magnitude;
    double dist = 0.0;
    bool hit = false;
    int index = 0;

    for(Collectable& item : items)
    {
        if(!item.collected)
        {
            hit = false;

            for(Obstacle& ob : obstacles)
            {
                int numVert = ob.vertices.getVertexCount();
                int nextVert = 0;

                for(int i = 0; i < numVert; i++)
                {
                    if(i == numVert - 1)
                        nextVert = 0;
                    else
                        nextVert = i + 1;

                    if(lineIntersection(mSprite.getPosition(), item.sprite.getPosition(), ob.vertices[i].position, ob.vertices[nextVert].position, dist))
                    {
                        hit = true;
                        break;
                    }
                }
            }

            if(!hit)
            {
                vecToItem = item.sprite.getPosition() - mSprite.getPosition();
                mTargetID = item.id;

                magnitude = std::sqrt((vecToItem.x * vecToItem.x) + (vecToItem.y * vecToItem.y));

                if(magnitude < bestMagnitude)
                {
                    bestVec = vecToItem;
                    bestMagnitude = magnitude;

                    mCollectableLine[0] = sf::Vertex(mSprite.getPosition(), sf::Color::Green);
                    mCollectableLine[1] = sf::Vertex(mSprite.getPosition() + vecToItem, sf::Color::Green);
                }
            }
        }

        index ++;
    }

    notFound = hit;

    return bestVec / bestMagnitude;
}

sf::Vector2f Enemy::nearestBot(std::vector<Obstacle>& obstacles, std::vector<EnemyPtr>& bots, bool& notFound)
{
    mPlayerLine.clear();

    mPlayerLine.append(sf::Vertex(mSprite.getPosition(), sf::Color::Red));
    mPlayerLine.append(sf::Vertex(mSprite.getPosition(), sf::Color::Red));

    sf::Vector2f bestVec(0.f, 0.f);
    sf::Vector2f vecToCol(0.f, 0.f);
    float magnitude;
    double dist = 0.0;
    bool hit = false;

    mMagnitudeToTarget = WorldX * WorldY;

    for(EnemyPtr& bot : bots)
    {
        hit = false;

        for(Obstacle& ob : obstacles)
        {
            int numVert = ob.vertices.getVertexCount();
            int nextVert = 0;

            for(int i = 0; i < numVert; i++)
            {
                if(i == numVert - 1)
                    nextVert = 0;
                else
                    nextVert = i + 1;

                if(lineIntersection(mSprite.getPosition(), bot->getPosition(), ob.vertices[i].position, ob.vertices[nextVert].position, dist))
                {
                    hit = true;
                    break;
                }
            }
        }

        if(!hit)
        {
            vecToCol = bot->getPosition() - mSprite.getPosition();

            magnitude = std::sqrt((vecToCol.x * vecToCol.x) + (vecToCol.y * vecToCol.y));

            if(magnitude < mMagnitudeToTarget)
            {
                bestVec = vecToCol;
                mMagnitudeToTarget = magnitude;

                mPlayerLine[1] = sf::Vertex(mSprite.getPosition() + vecToCol, sf::Color::Red);
            }
        }
    }

    notFound = hit;

    return bestVec / mMagnitudeToTarget;
}

double Enemy::botInput(std::vector<Obstacle>& obstacles, std::vector<EnemyPtr>& bots, bool& notFound)
{
    sf::Vector2f vecToPlayer = nearestBot(obstacles, bots, notFound);

    if(notFound)
        return 0;

    double mAngle = (std::atan2(vecToPlayer.y, vecToPlayer.x) - std::atan2(mVelocity.y, mVelocity.x)) * 180 / 3.14159265;
    mTargetInFront = false;

    if(mAngle > -5.0
       && mAngle < 5.0)
       mTargetInFront = true;

    double dot = vecDot(mVelocity, vecToPlayer);
    int sign = vecSign(mVelocity, vecToPlayer);

    return dot * sign;
}

double Enemy::playerInput(std::vector<Obstacle>& obstacles, Enemy::EnemyPtr& player, bool& notFound)
{

    sf::Vector2f vecToPlayer = nearestPlayer(obstacles, player->getPosition(), notFound);

    double mAngle = (std::atan2(vecToPlayer.y, vecToPlayer.x) - std::atan2(mVelocity.y, mVelocity.x)) * 180 / 3.14159265;
    mTargetInFront = false;

    if(mAngle > -10.0
       && mAngle < 10.0)
       mTargetInFront = true;

    double dot = vecDot(mVelocity, vecToPlayer);
    int sign = vecSign(mVelocity, vecToPlayer);

    return dot * sign;
}

double Enemy::playerInput(std::vector<Obstacle>& obstacles, Player::PlayerPtr& player, bool& notFound)
{
    sf::Vector2f vecToPlayer = nearestPlayer(obstacles, player->getPosition(), notFound);

    double mAngle = (std::atan2(vecToPlayer.y, vecToPlayer.x) - std::atan2(mVelocity.y, mVelocity.x)) * 180 / 3.14159265;
    mTargetInFront = false;

    if(mAngle > -10.0
       && mAngle < 10.0)
       mTargetInFront = true;

    double dot = vecDot(mVelocity, vecToPlayer);
    int sign = vecSign(mVelocity, vecToPlayer);

    return dot * sign;
}

sf::Vector2f Enemy::nearestPlayer(std::vector<Obstacle>& obstacles, sf::Vector2f playerPos, bool& notFound)
{
    mPlayerLine.clear();

    mPlayerLine.append(sf::Vertex(mSprite.getPosition(), sf::Color::Red));
    mPlayerLine.append(sf::Vertex(mSprite.getPosition(), sf::Color::Red));

    sf::Vector2f vecToCol(0.f, 0.f);
    double dist = 0.0;
    bool hit = false;

    mOldMagnitudeToTarget = mMagnitudeToTarget;

    mMagnitudeToTarget = WorldX * WorldY;
    hit = false;

    for(Obstacle& ob : obstacles)
    {
        int numVert = ob.vertices.getVertexCount();
        int nextVert = 0;

        for(int i = 0; i < numVert; i++)
        {
            if(i == numVert - 1)
                nextVert = 0;
            else
                nextVert = i + 1;

            if(lineIntersection(mSprite.getPosition(), playerPos, ob.vertices[i].position, ob.vertices[nextVert].position, dist))
            {
                hit = true;
                break;
            }
        }
    }

    if(!hit)
    {
        vecToCol = playerPos - mSprite.getPosition();
        mOldTargetPos = playerPos;
        mPlayerLine[1] = sf::Vertex(playerPos, sf::Color::Red);
    }
    else
    {
        vecToCol = mOldTargetPos - mSprite.getPosition();
        mPlayerLine[1] = sf::Vertex(mOldTargetPos, sf::Color::Red);
    }

    notFound = hit;
    mMagnitudeToTarget = std::sqrt((vecToCol.x * vecToCol.x) + (vecToCol.y * vecToCol.y));

    return vecToCol / mMagnitudeToTarget;
}
