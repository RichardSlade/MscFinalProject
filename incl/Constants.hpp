#ifndef INCL_CONSTANTS_HPP
#define INCL_CONSTANTS_HPP

#include <SFML/System/Vector2.hpp>

const int MenuScreenWidth = 1280;
const int MenuScreenHeight = 640;
const sf::Vector2u MenuScreenDim(MenuScreenWidth, MenuScreenHeight);

const int GameScreenWidth = 1280;
const int GameScreenHeight = 640;
const sf::Vector2u GameScreenDim(GameScreenWidth, GameScreenHeight);

const int MenuFontSize = 28;
const int GameFontSize = 20;

const int WorldX = 2000;
const int WorldY = 2000;
const int CellDivider = 20;
const int BorderSize = 20;
const int SpawnZoneWidth = 100;

const int MaxNumProjectiles = 10;
const float ProjectileSpeed = 15.f;
const int AmmoRechargeMax = 10;
const int PlayerAmmoCount = 4;
const int EnemyAmmoCount = 1;

const int HarvesterPts = 5;
const int HunterPts = 10;
const int LevelPts = 1000;
const int LifePts = 100;
const float TimePts = -2;

const int MinTicks = 2000;

const int PlayerLives = 5;
const int PlayerHitPoints = 100;
const int EnemyHitPoints = 100;
const int PlayerDamage = 10;
const int EnemyDamage = 5;

const float ScalingFactor = 10.f;
const float PlayerSpeed = 20.f;
const float PlayerRotate = 3.f;
const float PlayerBorder = 0;
const float EnemyBorder = 0;
const float ObstacleBorder = 16.f;
const float MineBorder = 4.f;

const int NumSensors = 5;
const int NumBackSensors = 3;
const int LongSensorRange = 50;
const int ShortSensorRange = 50;
const float CollisionDist = 0.6;
const double MaxTurnRate = 0.5;

const int CollectableCount = 100;
const int MineCount = 0;
const int ScoreThreshold = 4;
const int HighScoreSaveNum = 10;
const int iNumBestSweepers = 20;
const int NumTicksPerGen  = 2000;
const int NumGen = 50;
const int NumPop = 25;
const int SpawnPool = 50;
const int iNumSweepers = 50;

const int iNumAddLinkAttempts = 5;
const int iNumTrysToFindOldLink = 10;
const int iNumTrysToFindLoopedLink = 10;
const float dSurvivalRate = 0.2;
const int iNumGensAllowedNoImprovement = 15;
const int iMaxPermittedNeurons = 100;
//const float dChanceAddLink = 0.07;
const float dChanceAddLink = 0.2;
//const float dChanceAddNode = 0.03;
const float dChanceAddNode = 0.1;
const float dChanceAddRecurrentLink = 0.05;
const float dMutationRate = 0.2;
//const float dMutationRate = 0.8;
const float dMaxWeightPerturbation = 0.5;
const float dProbabilityWeightReplaced = 0.1;
const float dActivationMutationRate = 0.1;
const float dMaxActivationPerturbation = 0.1;
const float dCompatibilityThreshold = 0.26;
const int iOldAgeThreshold = 50;
const float dOldAgePenalty = 0.7;
const float dYoungFitnessBonus = 1.3;
const int iYoungBonusAgeThreshhold = 10;
const float dCrossoverRate = 0.7;

#endif // INCL_CONSTANTS_HPP
