#include <string>

#include <SFML/Window/Event.hpp>

#include <GameState.hpp>
#include <Constants.hpp>
#include <LoadSave.hpp>

GameState::~GameState()
{
    mNEATGA[0].reset();
    mNEATGA[1].reset();
}

void GameState::update(sf::Time dt)
{
    if(mGameStatus == GameStatus::Type::InProgress
       && mPlayerStart)
        mWorld.update(dt);
}

void GameState::saveBestGenotypes(Ai::Type aiType)
{
    std::vector<NEATGenotype> genotypes = mNEATGA[aiType]->getBestGenotypes();

    for(NEATGenotype& g : genotypes)
    {

        saveNeuronsToFile(std::string("brains/"
                                     + aiTypeToString(aiType)
                                     + "/neurons/"
                                     + std::to_string(mIDCount)
                                     + "neurons.dat"), g.Neurons());

        saveLinksToFile(std::string("brains/"
                                   + aiTypeToString(aiType)
                                   + "/links/"
                                   + std::to_string(mIDCount)
                                   + "links.dat"), g.Genes());

        mIDCount ++;

        saveInnovationToFile(std::string("brains/"
                                         + aiTypeToString(aiType)
                                         + "/ga/"
                                         + "innovations.dat"), mNEATGA[aiType]->getInnovations());

        saveInnovationDataToFile(std::string("brains/"
                                             + aiTypeToString(aiType)
                                             + "/ga/innoData.dat"),
                                             mNEATGA[aiType]->getNeuronID(), mNEATGA[aiType]->getInnoNum());


        saveSplitDepthsToFile(std::string("brains/"
                                    + aiTypeToString(aiType)
                                    + "/ga/"
                                    + "splits.dat"),
                                    mNEATGA[aiType]->getSplitDepths());

        saveSplitClientToFile(std::string("brains/"
                                          + aiTypeToString(aiType)
                                          + "/ga/splitData.dat"),
                                            mNEATGA[aiType]->getSplitClient());

        mController->setFpsCap(true);
        mController->setNextState(State::Type::Title);

    }
}

void GameState::saveGenerationStats(Ai::Type aiType, std::vector<double> fitnessScores)
{
    std::vector<double> stats;

    stats.push_back(mWorld.getGenCount());

    std::sort(fitnessScores.begin(), fitnessScores.end());
    std::reverse(fitnessScores.begin(), fitnessScores.end());
    double sumFitness = 0;

    for(double& d : fitnessScores)
        sumFitness += d;

    if(mBestFitness < fitnessScores[0])
        mBestFitness = fitnessScores[0];

    stats.push_back(static_cast<float>(mBestFitness)); // Best ever fitness
    stats.push_back(static_cast<float>(fitnessScores[0])); // Best fitness for generation
    stats.push_back(static_cast<float>(fitnessScores[iNumBestSweepers - 1])); // Worst of best
    stats.push_back(static_cast<float>(fitnessScores[fitnessScores.size() - 1])); // Worst in generation
    stats.push_back(static_cast<float>(sumFitness / fitnessScores.size())); // Avg Fitness for generation

//    saveGenerationStatsToFile(std::string("results/Generations/" + aiTypeToString(aiType) + "/" + std::to_string(mWorld.getPopCount()) + ".txt"), stats);
}

void GameState::changeGameStatus(GameStatus::Type status)
{
    if(status == GameStatus::Type::PlayerWin)
    {
        for(sf::Text& t : mTexts)
        {
            t.setString("");
            t.setCharacterSize(MenuFontSize);
        }

        DirectorResults& results = mDirector.getResults();

        results.harvesterBestFitness = mNEATGA[Ai::Type::Harvester]->BestEverFitness();
        results.hunterBestFitness = mNEATGA[Ai::Type::Hunter]->BestEverFitness();
        results.playerScore = mWorld.getPlayerScore();
        results.mineHitNum = mWorld.getMineHitNum();

        float lTime = results.levelTime.asSeconds();

        int mins = static_cast<int>(lTime / 60.f);
        int secs = static_cast<int>(std::fmod(lTime, 60.f));
        std::string timeStr(std::to_string(mins) + ":" + std::to_string(secs));

        int timePts = static_cast<int>(lTime * TimePts);
        int livesPts = results.playerLives * LifePts;

        mWorld.incPlayerScore(LevelPts);
        mWorld.incPlayerScore(timePts);
        mWorld.incPlayerScore(livesPts);

        const sf::IntRect viewBounds = mWindow.getViewport(mWindow.getView());

        mTexts[0].setCharacterSize(32);
        mTexts[0].setColor(sf::Color::Yellow);
        mTexts[0].setString("Level " + std::to_string(mWorld.getLevelCount() - 1) +" complete! + " + std::to_string(LevelPts) + "Pts");
        sf::FloatRect strBounds = mTexts[0].getLocalBounds();
        mTexts[0].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);

        sf::Vector2f center(viewBounds.width / 2, viewBounds.top / 2);

        mTexts[0].setPosition(center.x, 200.f);

        mTexts[1].setCharacterSize(32);
        mTexts[1].setString("Press 'C' to continue to next level");
        strBounds = mTexts[1].getLocalBounds();
        mTexts[1].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);

        mTexts[1].setPosition(center.x, viewBounds.height - (viewBounds.height / 8.f));

        mTexts[2].setString("Name: " + mPlayerName);
        mTexts[3].setString("Score: " + std::to_string(mWorld.getPlayerScore()));
        mTexts[4].setString("Time: " + timeStr + " | " + std::to_string(timePts) + "Pts");
        mTexts[5].setString("Lives: " + std::to_string(static_cast<int>(results.playerLives)) + "/" + std::to_string(PlayerLives) + " | " + std::to_string(livesPts) + "Pts");
        mTexts[6].setString("Harvesters killed: " + std::to_string(static_cast<int>(results.harvesterKills)) + " | " + std::to_string(HarvesterPts * results.harvesterKills) + "Pts");
        mTexts[7].setString("Hunters killed: " + std::to_string(static_cast<int>(results.hunterKills)) + " | " + std::to_string(HunterPts * results.hunterKills) + "Pts");
        mTexts[8].setString("Enemy Score Percentage: " + std::to_string(static_cast<int>(results.enemyPercentageCollected)) + "%");

        int inc = 300;

        for(int i = 2; i < mTexts.size(); i++)
        {
            sf::FloatRect strBounds = mTexts[i].getLocalBounds();
            mTexts[i].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
            mTexts[i].setPosition(center.x, center.y + inc);
            inc += 30;
        }

    }
    else if(status == GameStatus::Type::PlayerLose)
    {
        for(sf::Text& t : mTexts)
        {
            t.setString("");
            t.setCharacterSize(MenuFontSize);
        }

        const sf::IntRect viewBounds = mWindow.getViewport(mWindow.getView());

        mTexts[0].setCharacterSize(32);
        mTexts[0].setColor(sf::Color::Red);
        mTexts[0].setString("Game Over");
        sf::FloatRect strBounds = mTexts[0].getLocalBounds();
        mTexts[0].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
        sf::Vector2f center(viewBounds.width / 2.f, viewBounds.height / 2.f);
        mTexts[0].setPosition(center);

        mTexts[1].setCharacterSize(28);
        mTexts[1].setString("Press 'Esc' to continue to high scores");
        strBounds = mTexts[1].getLocalBounds();
        mTexts[1].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
        mTexts[1].setPosition(center.x , viewBounds.height - (viewBounds.height / 8.f));


    }
    else if(status == GameStatus::Type::InProgress)
    {
        mTexts[0].setColor(sf::Color::White);

        for(sf::Text& t : mTexts)
        {
            t.setString("");
            t.setCharacterSize(GameFontSize);
        }
    }
    else if(status == GameStatus::Type::Pause)
    {
        const sf::FloatRect viewBounds = mWorld.getViewBounds();
        sf::Vector2f center(viewBounds.left + (viewBounds.width / 2.f), viewBounds.top + (viewBounds.height / 2.f));

        mTexts[8].setString("Paused");
        mTexts[8].setCharacterSize(32);
        sf::FloatRect strBounds = mTexts[8].getLocalBounds();
        mTexts[8].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
        mTexts[8].setPosition(center.x, center.y + (viewBounds.height / 16.f));

        mTexts[9].setString("Press 'P' to continue");
        mTexts[9].setCharacterSize(28);
        strBounds = mTexts[9].getLocalBounds();
        mTexts[9].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
        mTexts[9].setPosition(center.x, viewBounds.height - (viewBounds.height / 8.f));
    }
    else if(status == GameStatus::Type::HighScore)
    {
        mTexts[0].setColor(sf::Color::White);

        for(sf::Text& t : mTexts)
        {
            t.setString("");
            t.setCharacterSize(24);
        }

        std::string nameStr;
        std::string scoreStr;

        const sf::IntRect viewBounds = mWindow.getViewport(mWindow.getView());
        sf::Vector2f center(viewBounds.width / 2.f, viewBounds.height / 2.f);

        mTexts[0].setCharacterSize(32);
        mTexts[0].setString("High Scores");

        sf::FloatRect strBounds = mTexts[0].getLocalBounds();
        mTexts[0].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
        mTexts[0].setPosition(center.x, 50.f);

        mTexts[1].setCharacterSize(28);
        mTexts[1].setString("Names:");
        strBounds = mTexts[1].getLocalBounds();
        mTexts[1].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
        mTexts[1].setPosition(center.x - (viewBounds.width / 8.f), (viewBounds.height / 8.f) + (viewBounds.height / 16.f));

        mTexts[2].setCharacterSize(28);
        mTexts[2].setString("Scores:");
        strBounds = mTexts[2].getLocalBounds();
        mTexts[2].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
        mTexts[2].setPosition(center.x + (viewBounds.width / 8.f), (viewBounds.height / 8.f) + (viewBounds.height / 16.f));

        mTexts[3].setCharacterSize(32);
        mTexts[3].setString("Press 'Esc' to return to menu screen");
        strBounds = mTexts[3].getLocalBounds();
        mTexts[3].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
        mTexts[3].setPosition(center.x, viewBounds.height - (viewBounds.height / 8.f));

        std::vector<HighScore>& highScores = mController->getHighScores();

        int playerScore = mWorld.getPlayerScore();

        if(playerScore > highScores[highScores.size() - 1].score)
            highScores[highScores.size() - 1] = HighScore(mPlayerName, playerScore);

        std::sort(highScores.begin(), highScores.end());

        int count = 1;

        for(HighScore& hs : highScores)
        {
            std::string tempStr(std::to_string(count++) + ". " + hs.name);

            tempStr += "\n";
            nameStr += tempStr;
            tempStr.clear();
            tempStr = std::to_string(hs.score);
            tempStr += "\n";
            scoreStr += tempStr;
        }

        mTexts[4].setString(nameStr);
        mTexts[4].setPosition(center.x - (viewBounds.width / 8.f), (viewBounds.height / 8.f) + (viewBounds.height / 16.f) + 100.f);

        mTexts[5].setString(scoreStr);
        mTexts[5].setPosition(center.x + (viewBounds.width / 8.f), (viewBounds.height / 8.f) + (viewBounds.height / 16.f) + 100.f);

    }

    mGameStatus = status;

}

void GameState::gameStats()
{
    sf::FloatRect viewBounds = mWorld.getViewBounds();

    for(sf::Text& t : mTexts)
        t.setString("");

    switch(mAppType)
    {
        case Application::Type::Training:
        case Application::Type::Testing:
        {
            mTexts[0].setString("Population Number: " + std::to_string(mWorld.getPopCount()));
            mTexts[1].setString("Generation Number: " + std::to_string(mWorld.getGenCount()));
            mTexts[2].setString("Iteration: " + std::to_string(mWorld.getIterCount()));
            mTexts[3].setString("Best Fitness: " + std::to_string(mBestFitness));
            mTexts[4].setString("Avg Fitness: " + std::to_string(mWorld.getAvgFitness()));
            mTexts[5].setString("Change in Fitness: " + std::to_string(mWorld.getChangeInFitness()));

            int inc = 20;

            for(sf::Text& text : mTexts)
            {
                sf::FloatRect strBounds(text.getLocalBounds());
                text.setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);

                text.setPosition(viewBounds.left + (strBounds.width / 2.f), viewBounds.top + inc);
                inc += 20;
            }

            break;
        }
        case Application::Type::Game:
        {
            for(sf::Text& txt : mTexts)
                txt.setString("");

            mTexts[0].setString("Level: " + std::to_string(mWorld.getLevelCount()) + "\n"
                                     + "Score: " + std::to_string(mWorld.getPlayerScore()));

            sf::FloatRect strBounds(mTexts[0].getLocalBounds());
            mTexts[0].setOrigin(strBounds.width / 2.f, strBounds.top);
            mTexts[0].setPosition(viewBounds.left + (viewBounds.width / 4.f), viewBounds.top + (viewBounds.height / 64.f));

            mTexts[1].setString("Health: " + std::to_string(mWorld.getPlayerHitPoints()) + "\n"
                                      + "Lives: " + std::to_string(mWorld.getPlayerLives()));

            strBounds = mTexts[1].getLocalBounds();
            mTexts[1].setOrigin(strBounds.width / 2.f, strBounds.top);
            mTexts[1].setPosition(viewBounds.left + (viewBounds.width / 2.f), viewBounds.top + (viewBounds.height / 64.f));

            mTexts[2].setString("Collectable Pool: " + std::to_string(mWorld.getCollectablePool()) + "/" + std::to_string(CollectableCount) + "\n"
                                + "Player collected: " + std::to_string(mWorld.getPlayerCollected()) + "/" + std::to_string(getPlayerCollectedThreshold()) + "\n"
                                + "Enemy collected: " + std::to_string(mWorld.getEnemyCollected()) + "/" + std::to_string(getEnemyCollectedThreshold()));

            strBounds = mTexts[2].getLocalBounds();
            mTexts[2].setOrigin(strBounds.width / 2.f, strBounds.top);
            mTexts[2].setPosition(viewBounds.left + (viewBounds.width - (viewBounds.width / 4.f)), viewBounds.top + (viewBounds.height / 64.f));


            if(mEvoType == Evolution::RealTime)
            {
                mTexts[3].setString("Real Time Stats:\nIteration Counter:" + std::to_string(mWorld.getIterCount()) + "\nNumber New" + std::to_string(mWorld.getRealTimeSpawned()));
                mTexts[3].setPosition(viewBounds.left + 50.f, viewBounds.top);
//                mTexts[4].setString
            }

            if(!mPlayerStart)
            {
                mTexts[5].setString("Press SPACE to start");
                strBounds = mTexts[5].getLocalBounds();
                mTexts[5].setOrigin(strBounds.width / 2.f, strBounds.top);
                mTexts[5].setPosition(viewBounds.left + viewBounds.width / 2.f, viewBounds.top + (viewBounds.height / 2.f) + (viewBounds.height / 8.f));
            }

            break;
        }
        default: break;
    }
}

void GameState::playerLose()
{
    saveBestGenotypes(Ai::Type::Harvester);
    saveBestGenotypes(Ai::Type::Hunter);
    mController->setNextState(State::Type::Title);
}

void GameState::setupText()
{
    std::string fileName = "media/Sansation.ttf";

    if(!mFont.loadFromFile(fileName))
       throw std::runtime_error("Font - Failed to load " + fileName);

    for(int i = 0; i < appTextNum(mAppType); i++)
    {
        mTexts.push_back(sf::Text(" ", mFont, GameFontSize));
        sf::FloatRect strBounds(mTexts[i].getLocalBounds());
        mTexts[i].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
    }

}

void GameState::handleInput()
{
    sf::Event event;

    CommandQueue& commands = mWorld.getCommandQueue();

    while(mWindow.pollEvent(event))
    {
        switch(event.type)
        {
            case sf::Event::Closed:
            {
                mController->closeApp();
            }
            case sf::Event::KeyPressed:
            {
                if(!mPlayerStart
                   && event.key.code == sf::Keyboard::Space)
                   mPlayerStart = true;

                switch(event.key.code)
                {
                    case sf::Keyboard::F:
                    {
                        if(mAppType == Application::Type::Training)
                        {
                            if(mController->getFpsCap())
                            {
                                mController->setFpsCap(false);
                            }
                            else
                            {
                                mController->setFpsCap(true);
                            }
                        }

                        break;
                    }
                    case sf::Keyboard::Escape:
                    {
                        if(mAppType == Application::Type::Game)
                        {
                            switch(mGameStatus)
                            {
                                case GameStatus::Type::PlayerLose:
                                case GameStatus::Type::InProgress: changeGameStatus(GameStatus::Type::HighScore); break;
                                case GameStatus::Type::HighScore: mController->setNextState(State::Type::Title); break;
                                default: break;
                            }
                        }
                        else
                        {
                            mController->setNextState(State::Type::Title);
                            break;
                        }

                    }
                    case sf::Keyboard::P:
                    {
                        if(mAppType == Application::Type::Game)
                        {
                            switch(mGameStatus)
                            {
                                case GameStatus::Type::InProgress: changeGameStatus(GameStatus::Type::Pause); break;
                                case GameStatus::Type::Pause: changeGameStatus(GameStatus::Type::InProgress); break;
                                default: break;
                            }
                        }

                    }
                    case sf::Keyboard::C:
                    {
                        if(mAppType == Application::Type::Game)
                        {
                            switch(mGameStatus)
                            {
                                case GameStatus::Type::PlayerWin: changeGameStatus(GameStatus::Type::InProgress); break;
                                case GameStatus::Type::PlayerLose: changeGameStatus(GameStatus::Type::HighScore); break;
                                default: break;
                            }
                        }

                    }
                    default: break;
                }
            }
        }
    }

    if(!mPlayerDead)
        mCommandHandler.handleRealtimeInput(commands);
}

void GameState::render()
{
    if(mGameStatus == GameStatus::Type::InProgress)
    {
        mWorld.render();
        gameStats();

        for(sf::Text& text : mTexts)
            mWindow.draw(text);
    }
    else if(mGameStatus == GameStatus::Type::Pause)
    {
            mWorld.render();

        for(sf::Text& text : mTexts)
            mWindow.draw(text);
    }
    else
    {
        for(sf::Text& text : mTexts)
            mWindow.draw(text);
    }

    if(!mPlayerStart)
    {
        mWindow.draw(mTexts[5]);
    }

//        std::vector<NEATNeuralNet*> bestNets = mNEATGA[Ai::Harvester]->GetBestPhenotypesFromLastGeneration();
//
//        int incX = 0;
//
//        for(NEATNeuralNet* net : bestNets)
//        {
//            net->DrawNet(0 + incX, 200 + incX, 200, 400);
//            std::vector<sf::CircleShape>& circles = net->neuronCircles();
//
//            sf::VertexArray& links = net->networkLinks();
//            links.setPrimitiveType(sf::Lines);
//
//            for(sf::CircleShape& c : circles)
//                mWindow.draw(c);
//            mWindow.draw(links);
//
//            incX += 200;
//        }

}
