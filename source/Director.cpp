#include <Director.hpp>

void Director::setupDifficulty()
{
    switch(mDifficulty)
    {
        case Ai::Type::Easy:
        {
            mEnemyMax = 3;
            mEnemyMin = 1;
            mDifficultyInc = 1;
            mDifficultyDec = 3;

            break;
        }
        case Ai::Type::Medium:
        {
            mEnemyMax = 5;
            mEnemyMin = 2;
            mDifficultyInc = 2;
            mDifficultyDec = 2;

            break;
        }
        case Ai::Type::Hard:
        {
            mEnemyMax = 10;
            mEnemyMin = 5;
            mDifficultyInc = 3;
            mDifficultyDec = 1;

            break;
        }
    }
}

void Director::clampParams()
{
    if(mHarvesterNum > mEnemyMax)
        mHarvesterNum = mEnemyMax;
    else if(mHarvesterNum < mEnemyMin)
        mHarvesterNum = mEnemyMin;

    if(mHunterNum > mEnemyMax)
        mHunterNum = mEnemyMax;
    else if(mHunterNum < mEnemyMin)
        mHunterNum = mEnemyMin;

    if(mCollectableRatio > 0.9)
        mCollectableRatio = 0.9;
    else if(mCollectableRatio < 0.1)
        mCollectableRatio = 0.1;
}

std::pair<int, int> Director::getEnemyNumbers()
{
    std::pair<int, int> enemyNums;

    enemyNums.first = mHarvesterNum;
    enemyNums.second = mHunterNum;

    return enemyNums;
}

void Director::update(sf::Time levelTime, int enemyScore, int playerLives, int harvesterKills, int hunterKills)
{
    // Decide number of hunters based on number of enemies killed
    float killPer = (static_cast<float>(harvesterKills + hunterKills) / static_cast<float>(mHarvesterNum + mHunterNum));

    if(killPer > 80)
    {
        mHunterNum += mDifficultyInc;
    }
    else
    {
        mHunterNum ++;
    }

    // Decide collectable ratio based on player time for ast level
    mTotalTime += levelTime;
    sf::Time avgTime = mTotalTime / static_cast<float>(mLevel);

    if(levelTime > avgTime + (avgTime / 8.f))
    {
        mCollectableRatio -= mDifficultyDec * 0.01;
    }
    else
    {
        mCollectableRatio += mDifficultyInc * 0.01;
    }

    mCollectableRatio += 0.01;

    // Decide number of harvesters based on percentage of collected
    float enemyScorePer = (enemyScore / static_cast<float>(mEnemyScoreThreshold)) * 100.f;

    if(enemyScorePer < 20)
    {
        mHarvesterNum += mDifficultyDec;
    }
    else if(enemyScorePer > 80)
    {
        mHarvesterNum -= mDifficultyDec;
    }
    else
    {
        mHarvesterNum ++;
    }

    mMineNum += mDifficultyInc;

    clampParams();

    mResults.totalTime += levelTime;
    mResults.avgTime = avgTime;
    mResults.levelTime = levelTime;
    mResults.playerLives = playerLives;
    mResults.harvesterKills = harvesterKills;
    mResults.hunterKills = hunterKills;
    mResults.playerKills = harvesterKills + hunterKills;
    mResults.killPercent = killPer;
    mResults.harvesterNum = mHarvesterNum;
    mResults.hunterNum = mHunterNum;
    mResults.enemyPercentageCollected = enemyScorePer;
    mResults.collectableRatio = mCollectableRatio;
    mResults.mineNum = mMineNum;

    mPlayerScoreThreshold = static_cast<int>(mCollectableNum * mCollectableRatio);
    mEnemyScoreThreshold = mCollectableNum - mPlayerScoreThreshold;

    mLevel ++;
}
