#ifndef INCL_GAMESTATE_HPP
#define INCL_GAMESTATE_HPP

#include <memory>
#include <vector>
#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Audio/Sound.hpp>

#include <Director.hpp>
#include <DirectorResults.hpp>
#include <Innovation.hpp>
#include <World.hpp>
#include <CommandHandler.hpp>
#include <NEATGA.hpp>
#include <State.hpp>
#include <Controller.hpp>

class NEATNeuralNet;
//class World;

class GameState : public State
{
    private:
        sf::RenderWindow&           mWindow;
        CommandHandler              mCommandHandler;
        Controller*                 mController;
        Director                    mDirector;
        std::vector<NEATGA::GAptr>  mNEATGA;
        World                       mWorld;

        int                         mIDCount;
        Application::Type           mAppType;
        Ai::Type                    mAiType;
        Evolution::Type             mEvoType;
        GameStatus::Type            mGameStatus;

        bool                        mPlayerDead;
        bool                        mPlayerStart;
        double                      mBestFitness;

//        sf::Sound                    mSound;

        std::array<sf::Texture,
                    Textures::TextureCount>&         mTextures;

//        std::array<sf::SoundBuffer,
//                    Sounds::SoundCount>&            mSounds;

//        std::array<sf::Sound, Sounds::SoundCount> mGameSounds;

        sf::Font                    mFont;
        std::vector<sf::Text>       mTexts;
        std::string                 mPlayerName;

        void                        setupText();
        void                        gameStats();


        void                        render();

    public:
                                    GameState(sf::RenderWindow& window
                                              , Controller* controller
                                              , std::array<sf::Texture
                                                                , Textures::Type::TextureCount>& textures
                                              , std::array<sf::SoundBuffer
                                                                , Sounds::Type::SoundCount>& sounds
                                              , std::string playerName
                                              , Application::Type appType
                                              , Ai::Type aiType
                                              , Evolution::Type evoType
                                              , std::vector<NEATGA::GAptr>& ga)
                                    : mWindow(window)
                                    , mController(controller)
                                    , mDirector(aiType, dirScoreRatio(aiType), dirNumEnemies(aiType), CollectableCount, 10, 0.5)
                                    , mNEATGA(std::move(ga))
                                    , mWorld(mWindow, this, textures, appType, aiType, evoType, sounds)
                                    , mIDCount(0)
                                    , mTextures(textures)
                                    , mAppType(appType)
                                    , mAiType(aiType)
                                    , mEvoType(evoType)
                                    , mPlayerStart(false)
                                    , mGameStatus(GameStatus::Type::InProgress)
                                    , mPlayerDead(false)
                                    , mPlayerName(playerName)
                                    , mBestFitness(0)
                                    {
                                        setupText();
                                    };

                                    ~GameState();

        void                        update(sf::Time dt); //{ mWorld.update(dt); };
        void                        handleInput();

//        void                        playSound(Sounds::Type type){ mGameSounds[type].play(); }

        void                        saveBestGenotypes(Ai::Type aiType);

        void                        changeGameStatus(GameStatus::Type status);
        GameStatus::Type            gameStatus(){ return mGameStatus; }
        void                        playerLose();

        bool                        playerDead(){ return mPlayerDead; }
        void                        setPlayerDead(bool status){ mPlayerDead = status; }

        std::pair<int, int>         getEnemyNumbers(){ return mDirector.getEnemyNumbers(); }
        int                         getPlayerCollectedThreshold(){ return mDirector.getPlayerScoreThreshold(); }
        int                         getEnemyCollectedThreshold(){ return mDirector.getEnemyScoreThreshold(); }
        int                         getCollectableNum(){ return mDirector.getCollectableNum(); }
        int                         getMineNum(){ return mDirector.getMineNum(); }

        void                        resetGA(Ai::Type aiType){ mNEATGA[aiType]->resetGA(); }


        void                        updateDirector(sf::Time levelTime, int enemyScore, int playerLives, int harvesterKills, int hunterKills){ mDirector.update(levelTime, enemyScore, playerLives, harvesterKills, hunterKills); }
//        void                        saveLevelStats(int levelNum){ mController->saveLevelStats(levelNum, mDirector.getResults()); }


        std::vector<NEATNeuralNet*> epoch(Ai::Type aiType, std::vector<double> fitnessScores) { return mNEATGA[aiType]->Epoch(fitnessScores); }
        std::vector<NEATNeuralNet*> gameEpoch(Ai::Type aiType, std::vector<double> fitnessScores) { return mNEATGA[aiType]->gameEpoch(fitnessScores); }

        std::pair<std::pair<int, int>, NEATNeuralNet*>              rtEpoch(Ai::Type aiType, std::vector<double> fitnessScores){ return mNEATGA[aiType]->rtEpoch(fitnessScores); }
        std::vector<NEATNeuralNet*> getPhenotypes(Ai::Type aiType, int num = -1){ return mNEATGA[aiType]->CreatePhenotypes(num); }

        void       tickGA(){ mNEATGA[Ai::Harvester]->tickGA(); mNEATGA[Ai::Hunter]->tickGA(); }
        void        setPlayerStart(bool status){ mPlayerStart = status; }
        void        resetGATime(){ mNEATGA[Ai::Harvester]->resetTimes(); mNEATGA[Ai::Hunter]->resetTimes(); }

        void        saveGenerationStats(Ai::Type aiType, std::vector<double> fitnessScores);

        void        backToTitle(){ mController->setNextState(State::Type::Title); }
        void        resetBestFitness(){ mBestFitness = 0; }

};

#endif // INCL_GAME_HPP
