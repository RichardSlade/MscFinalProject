#include <memory>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>

#include <Controller.hpp>
#include <TitleState.hpp>
#include <GameState.hpp>
#include <NEATGenotype.hpp>

const sf::Time Controller::TimePerFrame = sf::seconds(1.f/60.f);

Controller::Controller()
: mWindow(sf::VideoMode(MenuScreenWidth, MenuScreenHeight), "First Prototype", sf::Style::Close)
, mCurrentStateType(State::Type::Title)
, mNextStateType(State::Type::Title)
, mTimeSinceLastUpdate(sf::Time::Zero)
, mFpsCap(true)
{
//    mWindow.setMouseCursorVisible(false);

    loadTextures();
    loadSounds();

    mCurrentState = new TitleState(mWindow, this, mTextures);

    loadHighScoreData();
}

// Loads all textures to be used in the application
void Controller::loadTextures()
{
    // Add background
    sf::Texture texture;

    std::string fileName = "media/textures/rgbscaled.png";
    if(!texture.loadFromFile(fileName))
        throw std::runtime_error("Texture - Failed to load " + fileName);

    mTextures[Textures::Background] = texture;

    fileName = "media/textures/helpscreen.png";
    if(!texture.loadFromFile(fileName))
        throw std::runtime_error("Texture - Failed to load " + fileName);

    mTextures[Textures::Help] = texture;

    fileName = "media/textures/player.png";
    if(!texture.loadFromFile(fileName))
        throw std::runtime_error("Texture - Failed to load " + fileName);

    mTextures[Textures::Player] = texture;

    fileName = "media/textures/hunter.png";
    if(!texture.loadFromFile(fileName))
        throw std::runtime_error("Texture - Failed to load " + fileName);

    mTextures[Textures::Hunter] = texture;

    fileName = "media/textures/harvester.png";
    if(!texture.loadFromFile(fileName))
        throw std::runtime_error("Texture - Failed to load " + fileName);

    mTextures[Textures::Harvester] = texture;

    fileName = "media/textures/collectable.png";
    if(!texture.loadFromFile(fileName))
        throw std::runtime_error("Texture - Failed to load " + fileName);

    mTextures[Textures::Collectable] = texture;
}

// Loads all sound files to be ued in application
void Controller::loadSounds()
{
        // Add background
    sf::SoundBuffer sound;

    std::string fileName = "media/sounds/playerdeath.wav";
    if(!sound.loadFromFile(fileName))
        throw std::runtime_error("sound - Failed to load " + fileName);

    mSounds[Sounds::PlayerDeath] = sound;

    fileName = "media/sounds/enemydeath1.wav";
    if(!sound.loadFromFile(fileName))
        throw std::runtime_error("sound - Failed to load " + fileName);

    mSounds[Sounds::EnemyDeath1] = sound;

    fileName = "media/sounds/enemydeath2.wav";
    if(!sound.loadFromFile(fileName))
        throw std::runtime_error("sound - Failed to load " + fileName);

    mSounds[Sounds::EnemyDeath2] = sound;

    fileName = "media/sounds/playerlaser.wav";
    if(!sound.loadFromFile(fileName))
        throw std::runtime_error("sound - Failed to load " + fileName);

    mSounds[Sounds::PlayerLaser] = sound;

    fileName = "media/sounds/enemylaser.wav";
    if(!sound.loadFromFile(fileName))
        throw std::runtime_error("sound - Failed to load " + fileName);

    mSounds[Sounds::EnemyLaser] = sound;

    fileName = "media/sounds/collected.wav";
    if(!sound.loadFromFile(fileName))
        throw std::runtime_error("sound - Failed to load " + fileName);

    mSounds[Sounds::Collected] = sound;
}

void Controller::reset()
{
    mPlayerName = " ";
    mFpsCap = true;
}

void Controller::run()
{
    mTimeSinceLastUpdate = sf::Time::Zero;

    // While window is open loop
    while(mWindow.isOpen())
    {
        sf::Time elapsedTime = mClock.restart();

        if(mFpsCap) mTimeSinceLastUpdate += elapsedTime;

        while(mTimeSinceLastUpdate > TimePerFrame || !mFpsCap)
        {
            if(mFpsCap) mTimeSinceLastUpdate -= TimePerFrame;

            mCurrentState->handleInput();

            if(mFpsCap) mCurrentState->update(TimePerFrame);
            else mCurrentState->update(elapsedTime);
        }

        render();
        changeState();
    }

}

void Controller::changeState()
{
    if(mCurrentStateType != mNextStateType)
    {
        delete mCurrentState;

        switch(mNextStateType)
        {
            case State::Title:
            {
                mCurrentState = new TitleState(mWindow, this, mTextures);
                reset();
                break;
            }
            case State::Game:
            {
                std::vector<NEATGA::GAptr> gaVec;

                NEATGA::GAptr harvesterGA(new NEATGA(getAiPopNum(mAiType), inNum(Ai::Type::Harvester), outNum(Ai::Type::Harvester), 10, 10));
                NEATGA::GAptr hunterGA(new NEATGA(getAiPopNum(mAiType), inNum(Ai::Type::Hunter), outNum(Ai::Type::Hunter), 10, 10));
                Innovation inno;

                switch(mAppType)
                {
                    case Application::Type::Training:
                    {
                        if(mAiType == Ai::Type::Hunter)
                        {
                            if(mEvoType == Evolution::Train)
                            {
                                std::vector<NEATGenotype> genotypes;

                                for(int id = 0; id < 1; id++)
                                {
                                    genotypes.push_back(NEATGenotype(   id,
                                                                        loadNeurons(id, Ai::Harvester),
                                                                        loadLinks(id, Ai::Harvester),
                                                                        inNum(Ai::Type::Harvester),
                                                                        outNum(Ai::Type::Harvester)));
                                }


                                inno.setInnovations(loadInnovations(Ai::Type::Harvester));
                                std::pair<int, int> data = loadInnovationData(Ai::Type::Harvester);

                                inno.setNeuronID(data.first);
                                inno.setInnoNum(data.second);

                                data = loadSplitClientFromFile("brains/"
                                                               + aiTypeToString(Ai::Harvester)
                                                               + "/ga/splitData.dat");

                                std::vector<SplitDepth> splits = loadSplitDepthsFromFile("brains/"
                                                               + aiTypeToString(Ai::Harvester)
                                                               + "/ga/splits.dat");

                                harvesterGA->setSplitClient(data);
                                harvesterGA->setSplitDepths(splits);
                                harvesterGA->setInnovation(inno);
                                harvesterGA->setGenotypes(genotypes);
                                harvesterGA->setPopSize(1);

                                hunterGA->createGenotypes();
                            }
                            else
                            {
                                hunterGA->createGenotypes();
                            }
                        }
                        else
                        {
                            harvesterGA->createGenotypes();
                        }

                        gaVec.push_back(std::move(harvesterGA));
                        gaVec.push_back(std::move(hunterGA));

                        break;

                    }
                    case Application::Type::Game:
                    {
                        if(mEvoType == Evolution::NoEvolution)
                        {
                            std::vector<NEATGenotype> genotypes;

                            for(int id = 0; id < iNumBestSweepers; id++)
                            {
                                genotypes.push_back(NEATGenotype(   id,
                                                                    loadNeurons(id, Ai::Type::Harvester),
                                                                    loadLinks(id, Ai::Type::Harvester),
                                                                    inNum(Ai::Type::Harvester),
                                                                    outNum(Ai::Type::Harvester)));
                            }

                            inno.setInnovations(loadInnovations(Ai::Type::Harvester));
                            std::pair<int, int> data = loadInnovationData(Ai::Type::Harvester);

                            inno.setNeuronID(data.first);
                            inno.setInnoNum(data.second);

                            data = loadSplitClientFromFile("brains/"
                                                           + aiTypeToString(Ai::Harvester)
                                                           + "/ga/splitData.dat");

                            std::vector<SplitDepth> splits = loadSplitDepthsFromFile("brains/"
                                                           + aiTypeToString(Ai::Harvester)
                                                           + "/ga/splits.dat");

                            harvesterGA->setSplitClient(data);
                            harvesterGA->setSplitDepths(splits);
                            harvesterGA->setGenotypes(genotypes);
                            harvesterGA->setInnovation(inno);

                            genotypes.clear();

                            for(int id = 0; id < iNumBestSweepers; id++)
                            {
                                genotypes.push_back(NEATGenotype(   id,
                                                                    loadNeurons(id, Ai::Type::Hunter),
                                                                    loadLinks(id, Ai::Type::Hunter),
                                                                    inNum(Ai::Type::Hunter),
                                                                    outNum(Ai::Type::Hunter)));
                            }

                            inno.setInnovations(loadInnovations(Ai::Type::Hunter));
                            data = loadInnovationData(Ai::Type::Hunter);

                            inno.setNeuronID(data.first);
                            inno.setInnoNum(data.second);

                            data = loadSplitClientFromFile("brains/"
                                                           + aiTypeToString(Ai::Hunter)
                                                           + "/ga/splitData.dat");

                            splits = loadSplitDepthsFromFile("brains/"
                                                           + aiTypeToString(Ai::Hunter)
                                                           + "/ga/splits.dat");

                            hunterGA->setSplitClient(data);
                            hunterGA->setSplitDepths(splits);
                            hunterGA->setGenotypes(genotypes);
                            hunterGA->setInnovation(inno);
                        }
                        else
                        {
                            harvesterGA->setPopSize(25);
                            harvesterGA->createGenotypes();

                            hunterGA->setPopSize(25);
                            hunterGA->createGenotypes();
                        }
//

                        gaVec.push_back(std::move(harvesterGA));
                        gaVec.push_back(std::move(hunterGA));

                        break;
                    }
                    default: break;
                }

                mCurrentState = new GameState(mWindow, this, mTextures, mSounds, mPlayerName, mAppType, mAiType, mEvoType, gaVec);

                break;
            }
            default: break;
        }

        mCurrentStateType = mNextStateType;
        mNextStateType = mCurrentStateType;
    }
}

void Controller::closeApp()
{
    std::vector<std::string> names;
    std::vector<int> scores;

    for(HighScore& hs : mHighScores)
    {
        names.push_back(hs.name);
        scores.push_back(hs.score);
    }

    saveHighScores(names, scores);

    mWindow.close();
}

void Controller::loadHighScoreData()
{
    std::vector<std::string> names = loadHighScoreNamesFromFile();
    std::vector<int> scores = loadHighScoresFromFile();

    for(int i = 0; i < names.size(); i++)
    {
        mHighScores.push_back(HighScore(names[i], scores[i]));
    }
}

void Controller::render()
{
    mWindow.clear();
    mWindow.setView(mWindow.getDefaultView());
    mCurrentState->render();
    mWindow.display();
}

void Controller::setFpsCap(bool status)
{
    if(status && !mFpsCap)
    {
        mTimeSinceLastUpdate = sf::Time::Zero;
        mClock.restart();
    }

    mFpsCap = status;
}
