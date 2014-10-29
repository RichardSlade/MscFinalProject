#ifndef INCL_HIGHSCORE_HPP
#define INCL_HIGHSCORE_HPP

#include <string>

struct HighScore
{
    HighScore(std::string n, int s)
    : name(n)
    , score(s)
    {};

    std::string name;
    int score;

    friend bool operator<(const HighScore& lhs, const HighScore& rhs)
    {
        return (lhs.score > rhs.score);
    }
};

#endif INCL_HIGHSCORE_HPP
