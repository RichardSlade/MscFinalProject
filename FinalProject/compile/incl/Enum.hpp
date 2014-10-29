#ifndef INCL_ENUM_HPP
#define INCL_ENUM_HPP

#include <Constants.hpp>

namespace Textures
{
    enum Type
    {
        Background,
        Help,
        Player,
        Harvester,
        Hunter,
        Best,
        Collectable,
        Mine,
        TextureCount
    };
}

namespace Sounds
{
    enum Type
    {
        PlayerDeath,
        EnemyDeath1,
        EnemyDeath2,
        PlayerLaser,
        EnemyLaser,
        Collected,
        SoundCount
    };
}

namespace Application
{
    enum Type
    {
        Training,
        Testing,
        Game
    };
}

namespace Ai
{
    enum Type
    {
        Harvester,
        Hunter,
        Easy,
        Medium,
        Hard

    };
}

namespace GameStatus
{
    enum Type
    {
        InProgress,
        PlayerWin,
        PlayerLose,
        HighScore,
        Pause
    };
}

namespace Evolution
{
    enum Type
    {
        Train,
        TrainWithPlayer,
//        NEAT,
        RealTime,
//        RealTimeTrained,
//        RealTimeTrainedMutation,
        NEATEvolution,
        NoEvolution
    };
}

static bool isGameAi(Ai::Type aiType)
{
    switch(aiType)
    {
        case Ai::Type::Harvester:
        case Ai::Type::Hunter: return false;
        case Ai::Type::Easy:
        case Ai::Type::Medium:
        case Ai::Type::Hard: return true;
    }
}

static int appTextNum(Application::Type type)
{
    switch(type)
    {
        case Application::Type::Training:
        case Application::Type::Testing: return 7;
        case Application::Type::Game: return 12;
        default: break;
    }
}

static std::string appTypeToString(Application::Type type)
{
    switch(type)
    {
        case Application::Type::Training: return "Training";
        case Application::Type::Testing: return "Testing";
        case Application::Type::Game: return "Game";
        default: return " ";
    }
}

static std::string aiTypeToString(Ai::Type type)
{
    switch(type)
    {
        case Ai::Type::Harvester: return "Harvester";
        case Ai::Type::Hunter: return "Hunter";
        case Ai::Type::Easy: return "Easy";
        case Ai::Type::Medium: return "Medium";
        case Ai::Type::Hard: return "Hard";
        default: return " ";
    }
}

static int inNum(Ai::Type type)
{
    switch(type)
    {
        case Ai::Type::Harvester: return 6;
        case Ai::Type::Hunter: return 12;
    }
}

static int outNum(Ai::Type type)
{
    switch(type)
    {
        case Ai::Type::Harvester: return 2;
        case Ai::Type::Hunter: return 2;
    }
}

static int getIterNum(Ai::Type aiType)
{
    switch(aiType)
    {
        case Ai::Harvester: return 1000;
        case Ai::Hunter: return 1000;
    }
}

static int getGenNum(Ai::Type aiType)
{
    switch(aiType)
    {
        case Ai::Type::Harvester: return 4;
        case Ai::Type::Hunter: return 4;
        default: return 100;
    }
}

static int getSpawnPoolSize(Ai::Type aiType)
{
    switch(aiType)
    {
        case Ai::Type::Harvester: return 4;
        case Ai::Type::Hunter: return 4;
        case Ai::Type::Easy: // return 8;
        case Ai::Type::Medium: //return 7;
        case Ai::Type::Hard: return 50;
        default: return 4;
    }
}

static int dirNumEnemies(Ai::Type difficulty)
{
    switch(difficulty)
    {
        case Ai::Type::Easy: return 5;
        case Ai::Type::Medium: return 10;
        case Ai::Type::Hard: return 20;
        default: return 0;
    }
}

static int getAiPopNum(Ai::Type aiType)
{
    switch(aiType)
    {
        case Ai::Type::Easy: return dirNumEnemies(aiType);
        case Ai::Type::Medium: return dirNumEnemies(aiType);
        case Ai::Type::Hard: return dirNumEnemies(aiType);
        default: return 50;
    }
}

static int dirScoreThreshold(Ai::Type difficulty)
{
    switch(difficulty)
    {
        case Ai::Type::Easy: return 10;
        case Ai::Type::Medium: return 15;
        case Ai::Type::Hard: return 20;
        default: return 0;
    }
}

static float dirScoreRatio(Ai::Type difficulty)
{
    switch(difficulty)
    {
        case Ai::Type::Easy: return 0.1;
        case Ai::Type::Medium: return 0.2;
        case Ai::Type::Hard: return 0.3;
    }
}

#endif // INCL_ENUM_HPP
