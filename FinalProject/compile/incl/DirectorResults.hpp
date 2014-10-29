#ifndef INCL_DIRECTORRESULTS_HPP
#define INCL_DIRECTORRESULTS_HPP

#include <fstream>

struct DirectorResults
{
                    DirectorResults()
                    : totalTime(sf::Time::Zero)
                    {};

    sf::Time        totalTime;
    sf::Time        avgTime;
    sf::Time        levelTime;

    int             playerLives;
    int             playerScore;
    int             harvesterKills;
    int             hunterKills;
    int             playerKills;
    float           killPercent;

    int             harvesterNum;
    int             hunterNum;
    float           harvesterBestFitness;
    float           hunterBestFitness;
    float           enemyPercentageCollected;

    float           collectableRatio;
    int             mineNum;
    int             mineHitNum;

};

#endif // INCL_DIRECTORRESULTS_HPP
