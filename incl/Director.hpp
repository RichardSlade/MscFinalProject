#ifndef INCL_DIRECTOR_HPP
#define INCL_DIRECTOR_HPP

#include <memory>

#include <SFML/System/Time.hpp>

#include <Enum.hpp>
#include <DirectorResults.hpp>

class Director
{
    public:
        typedef std::unique_ptr<Director> DirPtr;

    private:
        Ai::Type                mDifficulty;

        int                     mPlayerScoreThreshold;
        int                     mEnemyScoreThreshold;

        int                     mEnemyNum;
        int                     mHarvesterNum;
        int                     mHunterNum;

        const int               mCollectableNum;
        int                     mMineNum;
        int                     mLevel;

        sf::Time                mTotalTime;
        int                     mTotalLives;

        float                   mEnemyRatio;
        float                   mCollectableRatio;

        DirectorResults         mResults;

        int                     mEnemyMax;
        int                     mEnemyMin;
        int                     mMineMax;
        int                     mMineMin;

        int                     mDifficultyInc;
        int                     mDifficultyDec;

        void                    setupDifficulty();
        void                    clearResults();
        void                    clampParams();

    public:
                                Director(Ai::Type difType, float scoreRatio, int numE, int numCol, int numMine, float ratioE)
                                : mDifficulty(difType)
//                                , mPlayerScoreThreshold(score)
//                                , mEnemyScoreThreshold()
                                , mEnemyNum(numE)
                                , mHarvesterNum(numE)
                                , mHunterNum(numE)
                                , mCollectableNum(numCol)
                                , mMineNum(numMine)
                                , mLevel(1)
                                , mTotalTime(sf::Time::Zero)
                                , mTotalLives(0)
                                , mEnemyRatio(ratioE)
                                , mCollectableRatio(scoreRatio)
                                {
                                    setupDifficulty();

                                    mPlayerScoreThreshold = mCollectableNum * scoreRatio;
                                    mEnemyScoreThreshold = mCollectableNum - mPlayerScoreThreshold;
                                };


        std::pair<int, int>     getEnemyNumbers();
        DirectorResults&        getResults(){ return mResults; }

        int                     getPlayerScoreThreshold(){ return mPlayerScoreThreshold; }
        int                     getEnemyScoreThreshold(){ return mEnemyScoreThreshold; }
        int                     getHarvesterNum(){ return mHarvesterNum; }
        int                     getHunterNum(){ return mHunterNum; }

        int                     getCollectableNum(){ return mCollectableNum; }
        int                     getMineNum(){ return mMineNum;}
        void                    update(sf::Time, int enemyScore, int playerLives, int harvesterKills, int hunterKills);

};

#endif // INCL_DIRECTOR_HPP
