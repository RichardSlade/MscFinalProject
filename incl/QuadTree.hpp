#ifndef INCL_QUADTREE_HPP
#define INCL_QUADTREE_HPP

//////////////////////////
//  Code Based on original code by Steve Lambert 2012
//  originally implemented in java
//////////////////////////

#include <memory>
#include <array>

#include <Collidable.hpp>

class QuadTree
{
    private:
        typedef                     std::unique_ptr<QuadTree> QuadPtr;

        int                         mMaxObjects;
        int                         mMaxLevels;
//        int                         mCurrentNumObjects;
        int                         mLevel;

        bool                        mActive;

//        std::vector<Collidable>     mReturnObjects;
//        std::vector<Collidable>     mObjects;
        std::vector<Collidable*>    mReturnObjects;
        std::vector<Collidable*>    mObjects;
//        std::vector<Item&>          mItems;
//        std::vector<Obstacle&>      mObstacles;
//        std::vector<Enemy&>         mEnemies;

        sf::FloatRect               mBounds;
        std::vector<QuadPtr>        mNodes;

    public:
                                    QuadTree(int level, sf::FloatRect bounds)
                                    : mMaxObjects(10)
                                    , mMaxLevels(5)
                                    , mLevel(level)
                                    , mBounds(bounds)
                                    , mActive(true)
                                    {
                                        for(int i = 0; i < 4; i++)
                                            mNodes.push_back(nullptr);
                                    };

//                                    ~QuadTree();

        void                        clearTree();
        void                        split();
        int                         getIndex(sf::FloatRect rect);
        void                        insert(Collidable*);

        std::vector<Collidable*>& retrieve(sf::FloatRect);

        void                        clearReturnObjects(){ mReturnObjects.clear(); }

        bool                        isActive(){ return mActive; }
        void                        setActive(bool status){ mActive = status; }

};

#endif // INCL_QUADTREE_HPP
