#include <cstdlib>
#include <utility>

#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Color.hpp>

#include <World.hpp>
#include <GameState.hpp>
#include <Utility.hpp>

World::World(sf::RenderWindow& window, GameState* game, std::array<sf::Texture, Textures::Type::TextureCount>& textures, Application::Type appType, Ai::Type aiType, Evolution::Type evoType, std::array<sf::SoundBuffer, Sounds::Type::SoundCount>& sounds)
: mWindow(window)
, mWorldView(window.getDefaultView())
, mWorldBounds(0.f, 0.f, WorldX, WorldY)
, mPlayerSpawnPosition(mWorldBounds.width / 2.f, mWorldBounds.height / 8.f)
, mEnemySpawnPosition(mWorldBounds.width / 2.f, mWorldBounds.height / 2.f)
, mEnemySpawnBounds((mWorldBounds.width / 2) - 80.f, (mWorldBounds.height / 2) - 80.f, 80.f, 80.f)
, mPlayerSpawnBounds(mPlayerSpawnPosition.x - 50.f, mPlayerSpawnPosition.y -50.f, 100.f, 100.f)
, mShootingBounds(mEnemySpawnBounds.left - (mEnemySpawnBounds.width / 4.f), mEnemySpawnBounds.top - (mEnemySpawnBounds.height / 4.f), mEnemySpawnBounds.width * 2.f, mEnemySpawnBounds.height * 2.f)
, mEnemySpawnBox(sf::Quads)
, mLevelTime(sf::Time::Zero)
, mGame(game)
, mQuadTree(0, mWorldBounds)
, mTextures(textures)
, mSounds(sounds)
, mAppType(appType)
, mAiType(aiType)
, mEvoType(evoType)
, mSpawnCollectable(false)
, mPlayerWin(false)
, mPlayerDead(false)
, mMinTicksPassed(false)
, mAvgFitness(0)
, mLastAvgFitness(0)
, mChangeInFitness(0)
, mPopCount(1)
, mIterCount(0)
, mGenCount(0)
, mLevelCount(1)
, mEnemyCollected(0)
, mCollectablePool(0)
, mEnemyHitMine(0)
, mRecorded(0)
, mPlayer(nullptr)
, mNumTicks(MinTicks / (getAiPopNum(mAiType) * 0.5))
, mRealTimeSpawned(0)
{
    buildWorld();
}

void World::buildWorld()
{
    // Create world walls and obstacles
    createObstacles();

    mEnemySpawnBox.append(sf::Vertex(sf::Vector2f(mShootingBounds.left, mShootingBounds.top), sf::Color::Red));
    mEnemySpawnBox.append(sf::Vertex(sf::Vector2f(mShootingBounds.left + mShootingBounds.width, mShootingBounds.top), sf::Color::Blue));
    mEnemySpawnBox.append(sf::Vertex(sf::Vector2f(mShootingBounds.left + mShootingBounds.width, mShootingBounds.top + mShootingBounds.height), sf::Color::Red));
    mEnemySpawnBox.append(sf::Vertex(sf::Vector2f(mShootingBounds.left, mShootingBounds.top + mShootingBounds.height), sf::Color::Blue));

    // Initialize Collectables
    mCollectableNum = mGame->getCollectableNum();

    for (int i = 0; i < mCollectableNum; i++)
    {
        Collectable col(mTextures[Textures::Collectable], i);
        positionCollectable(col);
        mCollectables.push_back(std::move(col));
    }

    // Add PLayer
    Player::PlayerPtr player(new Player(mTextures[Textures::Player], Entity::EntityType::Player, mSounds[Sounds::Collected], mSounds[Sounds::PlayerDeath], mSounds[Sounds::PlayerLaser]));
    mPlayer = std::move(player);
    positionPlayer();

    Sounds::Type deathSound;

    int count = 0;
    std::vector<NEATNeuralNet*> brains;

    switch(mAppType)
    {
        case Application::Type::Training:
        {
//            brains = mGame->getPhenotypes(Ai::Type::Harvester);

            if(mAiType == Ai::Type::Harvester)
            {
                brains = mGame->getPhenotypes(Ai::Type::Harvester);

                for(NEATNeuralNet* net : brains)
                {
                    if(randFloat() < 0.5)
                        deathSound = Sounds::Type::EnemyDeath1;
                    else
                        deathSound = Sounds::Type::EnemyDeath2;

                    Enemy::EnemyPtr harvester(new Enemy(mTextures[Textures::Harvester], Entity::EntityType::Enemy, net, count++, mSounds[deathSound], mSounds[Sounds::EnemyLaser], mShootingBounds));
                    positionEnemy(harvester);
                   mHarvesters.push_back(std::move(harvester));
                }

//                mHarvesters[0]->setTexture(mTextures[Textures::Best]);
            }
            else if(mAiType == Ai::Type::Hunter)
            {
                if(mEvoType == Evolution::Train)
                {
                    mPlayerBotBrains = mGame->getPhenotypes(Ai::Type::Harvester);

                    Enemy::EnemyPtr playerBot(new Enemy(mTextures[Textures::Harvester], Entity::EntityType::PlayerBot, mPlayerBotBrains[0], count++, mSounds[Sounds::Type::EnemyDeath1], mSounds[Sounds::EnemyLaser], mShootingBounds));
                    positionEnemy(playerBot);

                    mPlayerBot = std::move(playerBot);
                }

                brains.clear();
                brains = mGame->getPhenotypes(Ai::Type::Hunter);

                for(NEATNeuralNet* net : brains)
                {
                    if(randFloat() < 0.5)
                        deathSound = Sounds::Type::EnemyDeath1;
                    else
                        deathSound = Sounds::Type::EnemyDeath2;

                    Enemy::EnemyPtr hunter(new Enemy(mTextures[Textures::Hunter], Entity::EntityType::Enemy, net, count++, mSounds[deathSound], mSounds[Sounds::EnemyLaser], mShootingBounds));
                    positionEnemy(hunter);
                    mHunters.push_back(std::move(hunter));
                }

//                mHunters[0]->setTexture(mTextures[Textures::Best]);
            }

            break;
        }
        case Application::Type::Game:
        {
            std::pair<int, int> enemyNums = mGame->getEnemyNumbers();

            if(mEvoType != Evolution::NoEvolution)
                brains = mGame->getPhenotypes(Ai::Type::Harvester);
            else
                brains = mGame->getPhenotypes(Ai::Type::Harvester, enemyNums.first);

            for(NEATNeuralNet* net : brains)
            {
                if(randFloat() < 0.5)
                    deathSound = Sounds::Type::EnemyDeath1;
                else
                    deathSound = Sounds::Type::EnemyDeath2;

                Enemy::EnemyPtr harvester(new Enemy(mTextures[Textures::Harvester], Entity::EntityType::Enemy, net, count++, mSounds[deathSound], mSounds[Sounds::EnemyLaser], mShootingBounds));
                positionEnemy(harvester);
                mHarvesters.push_back(std::move(harvester));
            }

            count = 0;
            brains.clear();

            if(mEvoType != Evolution::NoEvolution)
                brains = mGame->getPhenotypes(Ai::Type::Hunter);
            else
                brains = mGame->getPhenotypes(Ai::Type::Hunter, enemyNums.second);

            for(NEATNeuralNet* net : brains)
            {
                if(randFloat() < 0.5)
                    deathSound = Sounds::Type::EnemyDeath1;
                else
                    deathSound = Sounds::Type::EnemyDeath2;

                Enemy::EnemyPtr hunter(new Enemy(mTextures[Textures::Hunter], Entity::EntityType::Enemy, net, count++, mSounds[deathSound], mSounds[Sounds::EnemyLaser], mShootingBounds));
                positionEnemy(hunter);
                mHunters.push_back(std::move(hunter));
            }

            break;
        }
    }

    setupQuadTree();
}

void World::resetLevel()
{
    mParticleSystems.clear();

    mPlayerWin = false;
    mPlayer->setKillCount(0);
    mPlayer->setCollected(0);

    mPlayer->reset();
    mPlayer->setVelocity(sf::Vector2f(0.f, 0.f));
    positionPlayer();

    mEnemyCollected = 0;

    mObstacles.clear();
    createObstacles();

    if(mAiType == Ai::Hunter
       && mEvoType == Evolution::Train)
        positionEnemy(mPlayerBot);

    mCollectablePool = 0;

    // Reposition collectabes
    for(Collectable& col : mCollectables)
    {
        positionCollectable(col);
    }

    setupQuadTree();
}

void World::spawnCollectable()
{
    for(Collectable& col : mCollectables)
    {
        if(col.collected)
        {
            positionCollectable(col);
            mCollectablePool --;

            col.collideBounds = col.sprite.getGlobalBounds();
            Collidable* colPtr = &col;

            mQuadTree.insert(colPtr);

            break;
        }
    }
}

void World::createObstacles()
{
    std::array<sf::Color, 6> colours;

    colours[0] = sf::Color::Red;
    colours[1] = sf::Color::Magenta;
    colours[2] = sf::Color::Green;
    colours[3] = sf::Color::Cyan;
    colours[4] = sf::Color::Yellow;
    colours[5] = sf::Color::Blue;


    // Define border walls
    Obstacle leftWall(sf::Quads);
    leftWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.left, mWorldBounds.top), sf::Color::Blue));
    leftWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.left + BorderSize, mWorldBounds.top), sf::Color::Blue));
    leftWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.left + BorderSize, mWorldBounds.height), sf::Color::Blue));
    leftWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.left, mWorldBounds.height), sf::Color::Blue));

    sf::RectangleShape rect(sf::Vector2f(BorderSize, mWorldBounds.height));
    rect.setPosition(mWorldBounds.left, mWorldBounds.top);

    leftWall.rect = rect;
    mObstacles.push_back(leftWall);

    Obstacle topWall(sf::Quads);
    topWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.left, mWorldBounds.top), sf::Color::Blue));
    topWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.width, mWorldBounds.top), sf::Color::Blue));
    topWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.width, mWorldBounds.top + BorderSize), sf::Color::Blue));
    topWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.left, mWorldBounds.top + BorderSize), sf::Color::Blue));

    rect.setPosition(mWorldBounds.left, mWorldBounds.top);
    rect.setSize(sf::Vector2f(mWorldBounds.width, BorderSize));

    topWall.rect = rect;
    mObstacles.push_back(topWall);

    Obstacle rightWall(sf::Quads);
    rightWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.width - BorderSize, mWorldBounds.top), sf::Color::Blue));
    rightWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.width, mWorldBounds.top), sf::Color::Blue));
    rightWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.width, mWorldBounds.height), sf::Color::Blue));
    rightWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.width - BorderSize, mWorldBounds.height), sf::Color::Blue));

    rect.setPosition(mWorldBounds.width - BorderSize, mWorldBounds.top);
    rect.setSize(sf::Vector2f(BorderSize, mWorldBounds.height));

    rightWall.rect = rect;
    mObstacles.push_back(rightWall);

    Obstacle bottomWall(sf::Quads);
    bottomWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.left, mWorldBounds.height - BorderSize), sf::Color::Blue));
    bottomWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.width, mWorldBounds.height - BorderSize), sf::Color::Blue));
    bottomWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.width, mWorldBounds.height), sf::Color::Blue));
    bottomWall.vertices.append(sf::Vertex(sf::Vector2f(mWorldBounds.left, mWorldBounds.height), sf::Color::Blue));

    rect.setPosition(mWorldBounds.left, mWorldBounds.height - BorderSize);
    rect.setSize(sf::Vector2f(mWorldBounds.width, BorderSize));

    bottomWall.rect = rect;
    mObstacles.push_back(bottomWall);

    sf::Color colour1 = colours[rangedInt(0, colours.size() - 1)];
    sf::Color colour2 = colours[rangedInt(0, colours.size() - 1)];

     // Define Obstacles
    Obstacle leftTop(sf::Quads);
    leftTop.vertices.append(sf::Vertex(sf::Vector2f(WorldX / 8, WorldY / 8), colour1));
    leftTop.vertices.append(sf::Vertex(sf::Vector2f((WorldX / 2) - (WorldX / 8), WorldY / 8), colour2));
    leftTop.vertices.append(sf::Vertex(sf::Vector2f((WorldX / 2) - (WorldX / 8), (WorldY / 2) - (WorldY / 8)), colour1));
    leftTop.vertices.append(sf::Vertex(sf::Vector2f(WorldX / 8, (WorldY / 2) - (WorldY / 8)), colour2));

    rect.setPosition(sf::Vector2f(WorldX / 8, WorldY / 8));
    rect.setSize(sf::Vector2f(WorldX / 4, WorldY / 4));

    leftTop.rect = rect;
    mObstacles.push_back(leftTop);

    colour1 = colours[rangedInt(0, colours.size() - 1)];
    colour2 = colours[rangedInt(0, colours.size() - 1)];

    Obstacle rightTop(sf::Quads);
    rightTop.vertices.append(sf::Vertex(sf::Vector2f((WorldX / 2) + WorldX / 8, WorldY / 8), colour1));
    rightTop.vertices.append(sf::Vertex(sf::Vector2f(WorldX - (WorldX / 8), WorldY / 8), colour2));
    rightTop.vertices.append(sf::Vertex(sf::Vector2f(WorldX - (WorldX / 8), (WorldY / 2) - (WorldY / 8)), colour1));
    rightTop.vertices.append(sf::Vertex(sf::Vector2f((WorldX / 2) + WorldX / 8, (WorldY / 2) - (WorldY / 8)), colour2));

    rect.setPosition(sf::Vector2f((WorldX / 2) + WorldX / 8, WorldY / 8));
    rect.setSize(sf::Vector2f(WorldX / 4, WorldY / 4));

    rightTop.rect = rect;
    mObstacles.push_back(rightTop);

    colour1 = colours[rangedInt(0, colours.size() - 1)];
    colour2 = colours[rangedInt(0, colours.size() - 1)];

    Obstacle leftBottom(sf::Quads);
    leftBottom.vertices.append(sf::Vertex(sf::Vector2f((WorldX / 2) + WorldX / 8, (WorldY / 2) + WorldY / 8), colour1));
    leftBottom.vertices.append(sf::Vertex(sf::Vector2f(WorldX - (WorldX / 8), (WorldY / 2) + WorldY / 8), colour2));
    leftBottom.vertices.append(sf::Vertex(sf::Vector2f(WorldX - (WorldX / 8), WorldY - (WorldY / 8)), colour1));
    leftBottom.vertices.append(sf::Vertex(sf::Vector2f((WorldX / 2) + WorldX / 8, WorldY - (WorldY / 8)), colour2));

    rect.setPosition(sf::Vector2f((WorldX / 2) + WorldX / 8, (WorldY / 2) + WorldY / 8));
    rect.setSize(sf::Vector2f(WorldX / 4, WorldY / 4));

    leftBottom.rect = rect;
    mObstacles.push_back(leftBottom);

    colour1 = colours[rangedInt(0, colours.size() - 1)];
    colour2 = colours[rangedInt(0, colours.size() - 1)];

    Obstacle rightBottom(sf::Quads);
    rightBottom.vertices.append(sf::Vertex(sf::Vector2f(WorldX / 8, (WorldY / 2) + WorldY / 8), colour1));
    rightBottom.vertices.append(sf::Vertex(sf::Vector2f((WorldX / 2) - WorldX / 8, (WorldY / 2) + WorldY / 8), colour2));
    rightBottom.vertices.append(sf::Vertex(sf::Vector2f((WorldX / 2) - WorldX / 8, WorldY - (WorldY / 8)), colour1));
    rightBottom.vertices.append(sf::Vertex(sf::Vector2f((WorldX / 8), WorldY - (WorldY / 8)), colour2));

    rect.setPosition(sf::Vector2f(WorldX / 8, (WorldY / 2) + WorldY / 8));
    rect.setSize(sf::Vector2f(WorldX / 4, WorldY / 4));

    rightBottom.rect = rect;
    mObstacles.push_back(rightBottom);
//
//    int numObstacles = rangedInt(3, 5);
//
//    for(int i = 0; i < numObstacles; i++)
//        mObstacles.push_back(generateQuad(colours));
}

Obstacle World::generateQuad(std::array<sf::Color, 5> colours)
{
    int rectX = rangedInt(BorderSize, WorldX - BorderSize);
    int rectY = rangedInt(BorderSize, WorldY - BorderSize);
    float rectWidth = rangedInt(WorldX / 8, WorldX / 4);
    float rectHeight = rangedInt(WorldY / 8, WorldY / 4);

    sf::RectangleShape rect(sf::Vector2f(rectWidth, rectHeight));
    rect.setPosition(rectX, rectY);
    sf::FloatRect rectBounds = rect.getGlobalBounds();

    while(true)
    {
        if((!rectBounds.intersects(mShootingBounds))
           && (!rectBounds.intersects(mPlayerSpawnBounds))
           && (rectBounds.left + rectBounds.width < WorldX - BorderSize)
           && (rectBounds.top + rectBounds.height < WorldY - BorderSize))
        {
            break;
        }

        rectX = rangedInt(BorderSize, WorldX - BorderSize);
        rectY = rangedInt(BorderSize, WorldY - BorderSize);
        rectWidth = rangedInt(WorldX / 8, WorldX / 4);
        rectHeight = rangedInt(WorldY / 8, WorldY / 4);

        rect.setSize(sf::Vector2f(rectWidth, rectHeight));
        rect.setPosition(rectX, rectY);
        rectBounds = rect.getGlobalBounds();
    }

    Obstacle ob(sf::Quads);

    int colourIndex = rangedInt(0, 3);

    ob.vertices.append(sf::Vertex(sf::Vector2f(rectBounds.left, rectBounds.top), colours[colourIndex]));
    ob.vertices.append(sf::Vertex(sf::Vector2f(rectBounds.left + rectBounds.width, rectBounds.top), colours[colourIndex]));
    ob.vertices.append(sf::Vertex(sf::Vector2f(rectBounds.left + rectBounds.width, rectBounds.top + rectBounds.height), colours[colourIndex]));
    ob.vertices.append(sf::Vertex(sf::Vector2f(rectBounds.left, rectBounds.top + rectBounds.height), colours[colourIndex]));

    ob.rect = rect;

    return ob;
}

void World::positionEnemy(Enemy::EnemyPtr& enemy)
{
    float x;
    float y;

    if(enemy->getType() == Entity::EntityType::PlayerBot)
    {
        x = mPlayerSpawnPosition.x;
        y = mPlayerSpawnPosition.y;
    }
    else
    {
        while(true)
        {
            x = rangedFloat(mEnemySpawnBounds.left, mEnemySpawnBounds.left + mEnemySpawnBounds.width);
            y = rangedFloat(mEnemySpawnBounds.top, mEnemySpawnBounds.top + mEnemySpawnBounds.height);

            if(mEnemySpawnBounds.contains(x, y))
                break;
        }

        x = mEnemySpawnPosition.x;
        y = mEnemySpawnPosition.y;
    }

    enemy->setPosition(sf::Vector2f(x, y));
    enemy->setRotation(rangedInt(0, 360));

}

void World::positionCollectable(Collectable& col)
{
    float x, y;
    bool skip = false;

    while(true)
    {
        x = rangedFloat(BorderSize + 20.f, mWorldBounds.width - (BorderSize + 20.f));
        y = rangedFloat(BorderSize + 20.f, mWorldBounds.height - (BorderSize + 20.f));

        col.sprite.setPosition(sf::Vector2f(x, y));
//        sf::FloatRect colBounds(x - 10.f, y - 10.f, 40.f, 40.f);

        skip = false;

        if(mShootingBounds.intersects(col.getBounds()))
        {
            skip = true;
//            continue;
        }

        for(Obstacle& ob : mObstacles)
        {
            if(ob.getBounds().intersects(col.getBounds()))
            {
                skip = true;
                break;
//                    spaceFound = true;
            }
        }

        if(skip)
            continue;
        else
            break;
    }

//    collectable.sprite.setPosition(x, y);
    col.collected = false;
}

void World::createParticleSystem(sf::Vector2f startPos, sf::Color pColour, bool isPlayer)
{
    ParticleSystem particles(startPos);
    particles.fuel(25, pColour);

    mParticleSystems.push_back(particles);

    if(isPlayer)
        mPlayerParticleIndex = mParticleSystems.size() - 1;
}

void World::adaptPlayerVelocity()
{
	sf::Vector2f velocity = mPlayer->getVelocity();

    if(velocity.x != 0.f && velocity.y != 0.f)
        mPlayer->setVelocity(velocity - (velocity / 64.f));
}

void World::handleProjectileCollisions()
{
    // Player projectiles
    std::vector<Projectile>& projectiles = mPlayer->getProjectiles();

    for(Projectile& proj : projectiles)
    {
        if(proj.isActive)
        {
            sf::Vector2f pos = proj.projectile.getPosition();

            // Check for projectile being out of world bounds
            if((pos.x < mWorldBounds.left) || (pos.x > mWorldBounds.width - BorderSize)
                || (pos.y < mWorldBounds.top + BorderSize) || (pos.y > mWorldBounds.height + - BorderSize))
            {
                proj.isActive = false;
                continue;
            }

            sf::FloatRect projBounds = proj.getBounds();

            // Check if projectile is in enemy spawn
            if(projBounds.intersects(mShootingBounds))
            {
                proj.isActive =  false;
                continue;
            }

            // Check for projectiles hitting enemies
            for(Enemy::EnemyPtr& e : mHarvesters)
            {
                sf::FloatRect enemyBounds = e->getBounds();

                if(enemyBounds.intersects(projBounds))
                {
                    proj.isActive = false;
                    e->changeHitPoints(-EnemyHitPoints);
                    break;
                }
            }

            if(!proj.isActive)
                continue;

            for(Enemy::EnemyPtr& e : mHunters)
            {
                sf::FloatRect enemyBounds = e->getBounds();

                if(enemyBounds.intersects(projBounds))
                {
                    proj.isActive = false;
                    e->changeHitPoints(-EnemyHitPoints);
                    break;
                }
            }

            //  Check for projectile hitting obstacle
            for(Obstacle& ob : mObstacles)
            {
                sf::FloatRect obBounds = ob.rect.getGlobalBounds();

                if(projBounds.intersects(obBounds))
                {
                    proj.isActive = false;
                    break;
                }
            }
        }
    }

    if(mAiType != Ai::Harvester)
    {
        for(Enemy::EnemyPtr& e : mHunters)
        {
            std::vector<Projectile>& projectiles = e->getProjectiles();

            for(Projectile& proj : projectiles)
            {
                if(proj.isActive)
                {
                    sf::Vector2f pos = proj.projectile.getPosition();

                    // Check if projectile has hit world boundaries
                    if((pos.x < mWorldBounds.left + BorderSize) || (pos.x > mWorldBounds.width - BorderSize)
                        || (pos.y < mWorldBounds.top + BorderSize) || (pos.y > mWorldBounds.height + - BorderSize))
                    {
                        proj.isActive = false;
                        continue;
                    }

                    sf::FloatRect projBounds = proj.getBounds();

                    // Check if projectile is in enemy spawn
                    if(projBounds.intersects(mShootingBounds))
                    {
                        proj.isActive =  false;
                        continue;
                    }

                    // Check for enemy projectile hitting player
                    if(projBounds.intersects(mPlayer->getBounds()))
                    {
                        proj.isActive = false;
                        mPlayer->changeHitPoints(-EnemyDamage);
                        break;
                    }

                    if(!proj.isActive)
                        continue;

                    for(Enemy::EnemyPtr& e : mHarvesters)
                    {
                        if(projBounds.intersects(e->getBounds()))
                        {
                            proj.isActive =  false;
                            break;
                        }
                    }

                    if(!proj.isActive)
                        continue;

                    for(Obstacle& ob : mObstacles)
                    {
                        sf::FloatRect obBounds = ob.rect.getGlobalBounds();

                        if(projBounds.intersects(obBounds))
                        {
                            proj.isActive = false;

                            break;
                        }
                    }
                }
            }
        }
    }
}

void World::setupQuadTree()
{
    // Setup quad tree
    mQuadTree.clearTree();

    for(Collectable& x : mCollectables)
    {
        x.collideBounds = x.sprite.getGlobalBounds();
        Collidable* colPtr = &x;
        mQuadTree.insert(colPtr);
    }

    for(Obstacle& x : mObstacles)
    {
        x.collideBounds = x.rect.getGlobalBounds();
        Collidable* colPtr = &x;
        mQuadTree.insert(colPtr);
    }
}

void World::handleCollisions()
{
    setupQuadTree();
    sf::FloatRect bounds;

    // Player collision
    mQuadTree.clearReturnObjects();
    std::vector<Collidable*>& nearObjects =  mQuadTree.retrieve(mPlayer->getBounds());

    if(mEvoType == Evolution::Train
       && mAppType == Application::Training
       && mAiType == Ai::Hunter)
    {
        nearObjects = mQuadTree.retrieve(mPlayerBot->getBounds());

        bounds = mPlayerBot->getBounds();

        for(Collidable* ob : nearObjects)
        {
            switch(ob->collidableType)
            {
                case Collidable::Type::C:
                {
                    if(!ob->collected)
                    {
                        if(bounds.intersects(ob->collideBounds))
                        {
                            ob->collected = true;
                        }
                    }

                    break;
                }
                case Collidable::Type::O:
                {
                    if(bounds.intersects(ob->collideBounds))
                    {
                        mPlayerBot->reverseMove();
                    }

                    break;
                }
                default: break;
            }
        }
    }

    nearObjects = mQuadTree.retrieve(mPlayer->getBounds());
    bounds = mPlayer->getBounds();

    if(bounds.intersects(mShootingBounds))
    {
        mPlayer->reverseMove();
    }

    for(Collidable* ob : nearObjects)
    {
        switch(ob->collidableType)
        {
            case Collidable::Type::C:
            {
                if(!ob->collected)
                {
                    if(bounds.intersects(ob->collideBounds))
                    {
                        mPlayer->incCollected();
                        mPlayer->incScore(1);
                        ob->collected = true;
                        mPlayer->playCollect();

                        if(mEvoType == Evolution::Train)
                            spawnCollectable();
                    }
                }

                break;
            }
            case Collidable::Type::O:
            {
                if(bounds.intersects(ob->collideBounds))
                {
                    mPlayer->reverseMove();
                }

                break;
            }
            case Collidable::Type::M:
            {
                if(bounds.intersects(ob->collideBounds))
                {
                    mPlayer->changeHitPoints(-PlayerHitPoints);
                }

                break;
            }
            default: break;
        }
    }

    // Harvester collision
    for(Enemy::EnemyPtr& e : mHarvesters)
    {
        bounds = e->getBounds();

        mQuadTree.clearReturnObjects();
        nearObjects.clear();
        nearObjects = mQuadTree.retrieve(e->getBounds());

        for(Collidable* ob : nearObjects)
        {
            switch(ob->collidableType)
            {
                case Collidable::Type::C:
                {
                    if(!ob->collected)
                    {
                        if(bounds.intersects(ob->collideBounds))
                        {
                            e->incFitness(10);
                            ob->collected = true;
                            e->incCollected();
                            mEnemyCollected ++;

                            if(mEvoType == Evolution::Train)
                                spawnCollectable();
                        }
                    }

                    break;
                }
                case Collidable::Type::O:
                {
                    if(bounds.intersects(ob->collideBounds))
                    {
                        e->reverseMove();
                    }

                    break;
                }
                case Collidable::Type::M:
                {
                    if(bounds.intersects(ob->collideBounds))
                    {
                        e->changeHitPoints(-EnemyHitPoints / 4);
                        mEnemyHitMine ++;
                    }

                    break;
                }
                default: break;
            }
        }
    }

    // Hunter collision
    for(Enemy::EnemyPtr& e : mHunters)
    {
        bounds = e->getBounds();

        mQuadTree.clearReturnObjects();
        nearObjects.clear();
        nearObjects = mQuadTree.retrieve(e->getBounds());

        for(Collidable* ob : nearObjects)
        {
            switch(ob->collidableType)
            {
                case Collidable::Type::O:
                {
                    if(bounds.intersects(ob->collideBounds))
                    {
                        e->reverseMove();
                    }

                    break;
                }
                case Collidable::Type::M:
                {
                    if(bounds.intersects(ob->collideBounds))
                    {
                        e->changeHitPoints(-EnemyHitPoints / 4);
                        mEnemyHitMine ++;
                    }

                    break;
                }
                default: break;
            }
        }
    }
}

vector<double> World::getFitnessScores(Ai::Type aiType)
{
    vector<double> scores;

    if(aiType == Ai::Type::Harvester)
    {
        for(Enemy::EnemyPtr& e : mHarvesters)
        {
            scores.push_back(e->getFitness());
        }

    }
    else if(aiType == Ai::Type::Hunter)
    {
        for(Enemy::EnemyPtr& e : mHunters)
        {
            scores.push_back(e->getFitness());
        }
    }

    return scores;
}

void World::checkEnemyStatus()
{
    sf::FloatRect viewBounds = getViewBounds();

    for(Enemy::EnemyPtr& e : mHarvesters)
    {
        if(e->getFitness() < 0)
            e->setFitness(0);

        if(e->getHitPoints() <= 0)
        {
            createParticleSystem(e->getPosition(), sf::Color::Blue);

            if(e->getBounds().intersects(viewBounds))
                e->playDeathSound();

            mEnemyCollected -= e->getCollected();
            mCollectablePool += e->getCollected();
            e->reset();
            positionEnemy(e);
            mPlayer->incHarvesterKillCount();
            mPlayer->incScore(HarvesterPts);
        }
    }

    for(Enemy::EnemyPtr& e : mHunters)
    {
        if(e->getFitness() < 0)
            e->setFitness(0);

        if(e->getHitPoints() <= 0)
        {
            createParticleSystem(e->getPosition(), sf::Color::Red);

           if(e->getBounds().intersects(viewBounds))
                e->playDeathSound();

            e->reset();
            positionEnemy(e);

            mPlayer->incHunterKillCount();
            mPlayer->incScore(HunterPts);
        }
    }
}

void World::updateTraining(sf::Time dt)
{
    // Update enemies
    if(mAiType == Ai::Harvester)
    {
        for(Enemy::EnemyPtr& e : mHarvesters)
            e->update(dt, mCollectables, mObstacles, mHunters, inNum(Ai::Type::Harvester), outNum(Ai::Type::Harvester));
    }
    else if(mAiType == Ai::Hunter)
    {
        if(mEvoType == Evolution::Train)
        {
            for(Enemy::EnemyPtr& e : mHunters)
                e->update(dt, mCollectables, mObstacles, mPlayerBot, mHunters, getViewBounds(), inNum(Ai::Type::Hunter), outNum(Ai::Type::Hunter));

            mPlayerBot->update(dt, mCollectables, mObstacles, mHunters, inNum(Ai::Type::Harvester), outNum(Ai::Type::Harvester));
        }
        else if(mEvoType == Evolution::TrainWithPlayer)
        {
            for(Enemy::EnemyPtr& e : mHunters)
                e->update(dt, mCollectables, mObstacles, mPlayer, mHunters, getViewBounds(), inNum(Ai::Type::Hunter), outNum(Ai::Type::Hunter));
        }
    }

    std::vector<double> fitnessScores = getFitnessScores(mAiType);

    double totalFitness = 0;

    for(double& d : fitnessScores)
        totalFitness += d;

    mAvgFitness = totalFitness / fitnessScores.size();

    if(mIterCount > NumTicksPerGen)
    {
        mGenCount ++;
        mIterCount = 0;
        trainingEpoch();
        resetLevel();

        mLastAvgFitness = mAvgFitness;
        mRecorded ++;
    }

    mIterCount ++;
}

void World::updateGame(sf::Time dt)
{
    // Update enemies
    for(Enemy::EnemyPtr& e : mHarvesters)
        e->update(dt, mCollectables, mObstacles, mHunters, inNum(Ai::Type::Harvester), outNum(Ai::Type::Harvester));

    for(Enemy::EnemyPtr& e : mHunters)
        e->update(dt, mCollectables, mObstacles, mPlayer, mHunters, getViewBounds(), inNum(Ai::Type::Hunter), outNum(Ai::Type::Hunter));

    if(mCollectablePool > 0)
    {
        if(mCollectableCountdown <= sf::Time::Zero)
        {
            spawnCollectable();
            mCollectableCountdown += sf::seconds(0.5);
        }
        else
        {
            mCollectableCountdown -= dt;
        }
    }

    // If game check for player win or lose conditions
    if(mPlayer->getHitPoints() <= 0)
    {
        mPlayer->reset();
        mPlayer->decLives();
        createParticleSystem(mPlayer->getPosition(), sf::Color::Green, true);
        mGame->setPlayerDead(true);
        mPlayer->playDeathSound();
    }

    if(mPlayer->getCollected() >= mGame->getPlayerCollectedThreshold())
    {
        mPlayerWin = true;

    }
    else if(mEnemyCollected >= mGame->getEnemyCollectedThreshold()
            || mPlayer->getLives() < 0)
    {
        mGame->changeGameStatus(GameStatus::Type::PlayerLose);
    }

    // Evolution depending on if training or game
    mLevelTime += dt;

    if(mEvoType == Evolution::RealTime)
    {
        updateRtNEAT();
    }

    if(mPlayerWin)
    {
        mGame->updateDirector(mLevelTime, mEnemyCollected, mPlayer->getLives(), mPlayer->getHarvesterKillCount(), mPlayer->getHunterKillCount());
//        mGame->saveLevelStats(mLevelCount);

        if(mEvoType == Evolution::NEATEvolution)
            gameEpoch();

        mPlayer->incScore(LevelPts + (mPlayer->getLives() * LifePts));

        mLevelTime = sf::Time::Zero;
        mPlayerWin = false;

        mGame->setPlayerDead(false);
        mLevelCount ++;
        resetLevel();
        mRealTimeSpawned = 0;

        mGame->setPlayerStart(false);

        for(Enemy::EnemyPtr& e : mHarvesters)
        {
            positionEnemy(e);
            e->setCollected(0);
        }


        for(Enemy::EnemyPtr& e : mHunters)
            positionEnemy(e);

        mGame->changeGameStatus(GameStatus::Type::PlayerWin);
    }
}

void World::updateRtNEAT()
{
    if(mMinTicksPassed
           && mIterCount > mNumTicks)
    {
        mIterCount = 0;

        std::pair<std::pair<int, int>, NEATNeuralNet*> newEnemyData = mGame->rtEpoch(Ai::Harvester, getFitnessScores(Ai::Harvester));

        int currentID = newEnemyData.first.first;
        int currentIndex = -1;

        for(int i = 0; i < mHarvesters.size(); i++)
        {
            if(mHarvesters[i]->getID() == currentID)
            {
                currentIndex = i;
                break;
            }
        }

        if(currentIndex == -1)
            throw std::runtime_error("Couldn't find harvester ID. Looking for: " + std::to_string(newEnemyData.first.first));

        mHarvesters[currentIndex]->setID(newEnemyData.first.second);
        mHarvesters[currentIndex]->implantBrain(newEnemyData.second);
        mHarvesters[currentIndex]->setFitness(0);
        mHarvesters[currentIndex]->resetMap();
        positionEnemy(mHarvesters[currentIndex]);

        mRealTimeSpawned ++;

        newEnemyData = mGame->rtEpoch(Ai::Hunter, getFitnessScores(Ai::Hunter));

        currentID = newEnemyData.first.first;
        currentIndex = -1;

        for(int i = 0; i < mHunters.size(); i++)
        {
            if(mHunters[i]->getID() == currentID)
            {
                currentIndex = i;
                break;
            }
        }

        if(currentIndex == -1)
            throw std::runtime_error("Couldn't find hunter ID. Looking for: " + std::to_string(newEnemyData.first.first));

        mHunters[currentIndex]->setID(newEnemyData.first.second);
        mHunters[currentIndex]->implantBrain(newEnemyData.second);
        mHunters[currentIndex]->setFitness(0);
        mHunters[currentIndex]->resetMap();
        positionEnemy(mHunters[currentIndex]);

         mRealTimeSpawned ++;
    }

    // Evolution depending on if training or game
    mGame->tickGA();

    mIterCount ++;

    if(mIterCount > MinTicks
       && !mMinTicksPassed)
    {
        mMinTicksPassed = true;
        mIterCount = 0;
    }
}

void World::update(sf::Time dt)
{
    for(int i = 0; i < mParticleSystems.size(); i++)
    {
        if(mParticleSystems[i].isActive())
            mParticleSystems[i].update(dt);
    }

    if(!mGame->playerDead())
    {
        // Update player
        while(!mCommandQueue.isEmpty())
            mPlayer->onCommand(mCommandQueue.pop(), dt);

        mPlayer->update(dt);
        adaptPlayerVelocity();

        switch(mAppType)
        {
            case Application::Training: updateTraining(dt); break;
            case Application::Game: updateGame(dt); break;
        }

        // Check collisions
        handleCollisions();
        handleProjectileCollisions();

        // Check game status
        checkEnemyStatus();
    }
    else
    {
        if(!mParticleSystems[mPlayerParticleIndex].isActive())
        {
            mGame->setPlayerDead(false);
            mParticleSystems.erase(mParticleSystems.begin() + mPlayerParticleIndex);
            positionPlayer();
        }
    }
}

void World::gameEpoch()
{

    std::vector<NEATNeuralNet*> nets = mGame->gameEpoch(Ai::Harvester, getFitnessScores(Ai::Harvester));
    int index = 0;
    for(Enemy::EnemyPtr& e : mHarvesters)
    {
        e->reset();
        e->resetMap();
        e->setFitness(0);
        e->implantBrain(nets[index++]);
        positionEnemy(e);
    }



    nets.clear();
    nets = mGame->gameEpoch(Ai::Hunter, getFitnessScores(Ai::Hunter));
    index = 0;

    for(Enemy::EnemyPtr& e : mHunters)
    {
        e->reset();
        e->resetMap();
        e->setFitness(0);
        e->implantBrain(nets[index++]);
        positionEnemy(e);
    }
}

void World::trainingEpoch()
{
    std::vector<NEATNeuralNet*> nets;
    int index = 0;

    std::vector<double> fitnessScores = getFitnessScores(mAiType);

    mChangeInFitness = mAvgFitness - mLastAvgFitness;

    if(mGenCount > NumGen)
    {
        mPopCount ++;

        if(mPopCount >= NumPop)
        {
            mGame->saveBestGenotypes(mAiType);

            return;
        }

        mGame->resetBestFitness();
        mGame->resetGA(mAiType);
        mGenCount = 0;
        mLastAvgFitness = 0;

        nets = mGame->getPhenotypes(mAiType);

        if(mAiType == Ai::Type::Harvester)
        {
            for(Enemy::EnemyPtr& e : mHarvesters)
            {
                e->reset();
                e->resetMap();
                e->setFitness(0);
                e->implantBrain(nets[index++]);
                positionEnemy(e);
            }
        }
        else if(mAiType == Ai::Type::Hunter)
        {
            for(Enemy::EnemyPtr& e : mHunters)
            {
                e->reset();
                e->resetMap();
                e->setFitness(0);
                e->implantBrain(nets[index++]);
                positionEnemy(e);
            }

            for(Enemy::EnemyPtr& e : mHarvesters)
            {
                positionEnemy(e);
            }
        }
    }
    else
    {

        nets = mGame->epoch(mAiType, getFitnessScores(mAiType));
        mGame->saveGenerationStats(mAiType, getFitnessScores(mAiType));

        if(mAiType == Ai::Type::Harvester)
        {
            for(Enemy::EnemyPtr& e : mHarvesters)
            {
                e->reset();
                e->resetMap();
                e->setFitness(0);
                e->implantBrain(nets[index++]);
                positionEnemy(e);
            }
        }
        else if(mAiType == Ai::Type::Hunter)
        {
            for(Enemy::EnemyPtr& e : mHunters)
            {
                e->reset();
                e->resetMap();
                e->setFitness(0);
                e->implantBrain(nets[index++]);
                positionEnemy(e);
            }

            for(Enemy::EnemyPtr& e : mHarvesters)
            {
                positionEnemy(e);
            }
        }
    }

}

void World::increaseEnemies()
{
    std::pair<int, int> enemyNum = mGame->getEnemyNumbers();
    std::vector<NEATNeuralNet*> brains = mGame->getPhenotypes(Ai::Harvester, enemyNum.first);

    Sounds::Type deathSound;
    int count = 0;

    for(Enemy::EnemyPtr& e : mHarvesters)
    {
        e->implantBrain(brains[count++]);
        e->reset();
        e->resetMap();
        e->setFitness(0);
    }

    if(mHarvesters.size() < brains.size())
    {
        int dif = brains.size() - mHarvesters.size();

        for(int i = 0; i < dif; i++)
        {
            if(randFloat() < 0.5)
                deathSound = Sounds::Type::EnemyDeath1;
            else
                deathSound = Sounds::Type::EnemyDeath2;

            Enemy::EnemyPtr harvester(new Enemy(mTextures[Textures::Harvester], Entity::EntityType::Enemy, brains[count], count++, mSounds[deathSound], mSounds[Sounds::EnemyLaser], mShootingBounds));
            mHarvesters.push_back(std::move(harvester));
        }
    }

    brains.clear();
    brains = mGame->getPhenotypes(Ai::Hunter, enemyNum.second);
    count = 0;

    for(Enemy::EnemyPtr& e : mHarvesters)
    {
        e->implantBrain(brains[count++]);
        e->reset();
        e->resetMap();
        e->setFitness(0);
    }

     if(mHunters.size() < brains.size())
    {
        int dif = brains.size() - mHunters.size();

        for(int i = 0; i < dif; i++)
        {
            if(randFloat() < 0.5)
                deathSound = Sounds::Type::EnemyDeath1;
            else
                deathSound = Sounds::Type::EnemyDeath2;

            Enemy::EnemyPtr hunter(new Enemy(mTextures[Textures::Hunter], Entity::EntityType::Enemy, brains[count], count++, mSounds[deathSound], mSounds[Sounds::EnemyLaser], mShootingBounds));
            mHunters.push_back(std::move(hunter));
        }
    }

}

void World::render()
{
    mWorldView.setCenter(mPlayer->getPosition());

    mWindow.setView(mWorldView);

    sf::FloatRect viewBounds = getViewBounds();

    for(Collectable& col : mCollectables)
    {
        if(!col.collected)
        {
            if(viewBounds.intersects(col.sprite.getGlobalBounds()))
            {
                sf::Sprite colSprt = col.getSprite();
                mWindow.draw(colSprt);
            }
        }
    }


    for(Enemy::EnemyPtr& enemy : mHarvesters)
    {
        if(viewBounds.intersects(enemy->getSpriteBounds()))
        {
            std::vector<Projectile>& projectiles = enemy->getProjectiles();

            for(Projectile& proj : projectiles)
            {
                if(proj.isActive)
                    mWindow.draw(proj.projectile);
            }

            sf::Sprite enemySprt = enemy->getSprite();
            mWindow.draw(enemySprt);


        }

        if(mEvoType == Evolution::Train
               || mEvoType == Evolution::TrainWithPlayer)
            {
                mWindow.draw(enemy->getLongSensorLines());
//                mWindow.draw(enemy->getShortSensorLines());
                mWindow.draw(enemy->getCollectableLine());
//                mWindow.draw(enemy->getMineLine());
    //            mWindow.draw(enemy->getPlayerLine());
            }
    }

    if(mEvoType == Evolution::Train
       && mAiType == Ai::Hunter)
    {
        sf:: Sprite playerBot = mPlayerBot->getSprite();
        mWindow.draw(playerBot);
    }

    for(Enemy::EnemyPtr& enemy : mHunters)
    {
        if(viewBounds.intersects(enemy->getSpriteBounds()))
        {
            std::vector<Projectile>& projectiles = enemy->getProjectiles();

            for(Projectile& proj : projectiles)
            {
                if(proj.isActive)
                    mWindow.draw(proj.projectile);
            }

            sf::Sprite enemySprt = enemy->getSprite();
            mWindow.draw(enemySprt);

        }

        if(mEvoType == Evolution::Train
               || mEvoType == Evolution::TrainWithPlayer)
            {
                mWindow.draw(enemy->getLongSensorLines());
        //            mWindow.draw(enemy->getShortSensorLines());
    //            mWindow.draw(enemy->getCollectableLine());
    //            mWindow.draw(enemy->getMineLine());
                mWindow.draw(enemy->getPlayerLine());
            }
    }

    if(!mGame->playerDead())
        mWindow.draw(mPlayer->getSprite());

    std::vector<Projectile>& projectiles = mPlayer->getProjectiles();

    for(Projectile& proj : projectiles)
    {
        if(proj.isActive)
        {
//            if(viewBounds.intersects(proj.projectile.getGlobalBounds()))
//            {
                mWindow.draw(proj.projectile);
//            }
        }
    }

    for(Obstacle& ob : mObstacles)
    {
        if(viewBounds.intersects(ob.getBounds()))
        {
            mWindow.draw(ob.vertices);
        }
    }

    mWindow.draw(mEnemySpawnBox);

    for(ParticleSystem& sys : mParticleSystems)
    {
        if(sys.isActive())
            mWindow.draw(sys);
    }

    sf::RectangleShape UIBackDrop(sf::Vector2f(viewBounds.width, 75.f));
    UIBackDrop.setPosition(viewBounds.left, viewBounds.top);
    UIBackDrop.setFillColor(sf::Color::Black);
    mWindow.draw(UIBackDrop);


}

sf::FloatRect World::getViewBounds() const
{
	return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}
