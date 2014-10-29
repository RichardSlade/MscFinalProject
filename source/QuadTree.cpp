#include <QuadTree.hpp>

//////////////////////////
//  Code Based on original code by Steve Lambert 2012
//  originally implemented in java
//////////////////////////


void QuadTree::clearTree()
{
    mObjects.clear();

    for(QuadPtr& node : mNodes)
    {
        if(node != nullptr)
        {
            node->clearTree();
            node = nullptr;
        }

    }
}

void QuadTree::split()
{
    int subWidth = static_cast<int>(mBounds.width / 2.f);
    int subHeight = static_cast<int>(mBounds.height / 2.f);

    int x = static_cast<int>(mBounds.left);
    int y = static_cast<int>(mBounds.top);

    mNodes[0] = QuadPtr(new QuadTree(mLevel + 1, sf::FloatRect(x + subWidth, y, subWidth, subHeight)));
    mNodes[1] = QuadPtr(new QuadTree(mLevel + 1, sf::FloatRect(x, y, subWidth, subHeight)));
    mNodes[2] = QuadPtr(new QuadTree(mLevel + 1, sf::FloatRect(x, y + subHeight, subWidth, subHeight)));
    mNodes[3] = QuadPtr(new QuadTree(mLevel + 1, sf::FloatRect(x + subWidth, y + subHeight, subWidth, subHeight)));
}

int QuadTree::getIndex(sf::FloatRect pRect)
{
    int index = -1;
    float verticalMidpoint = mBounds.left + (mBounds.width / 2);
    float horizontalMidpoint = mBounds.top + (mBounds.height / 2);

    bool topQuadrant = (pRect.top < horizontalMidpoint
                        && pRect.top + pRect.height < horizontalMidpoint);

    bool bottomQuadrant = (pRect.top > horizontalMidpoint);

    if(pRect.left < verticalMidpoint
       && pRect.left + pRect.width  < verticalMidpoint)
    {
        if(topQuadrant)
            index = 1;
        else if(bottomQuadrant)
            index = 2;
    }
    else if(pRect.left > verticalMidpoint)
    {
        if(topQuadrant)
            index = 0;
        else if(bottomQuadrant)
            index = 3;
    }

    return index;
}

void QuadTree::insert(Collidable* item)
{
    sf::FloatRect pRect = item->collideBounds;

    if(mNodes[0] != nullptr)
    {
        int index = getIndex(pRect);

        if(index != -1)
        {
            mNodes[index]->insert(item);
            return;
        }
    }

    mObjects.push_back(item);

    if(mObjects.size() > mMaxObjects && mLevel < mMaxLevels)
    {
        if(mNodes[0] == nullptr)
            split();

        int i = 0;

        while(i < mObjects.size())
        {
            if(mObjects[i] != nullptr)
            {

                int index = getIndex(mObjects[i]->collideBounds);

                if(index != -1)
                {
                    mNodes[index]->insert(mObjects[i]);
                    mObjects[i] = nullptr;

                    i ++;
                }
                else
                {
                    i++;
                }
            }
            else
            {
                i++;
            }
        }
    }
}

std::vector<Collidable*>& QuadTree::retrieve(sf::FloatRect pRect)
{
    int index = getIndex(pRect);

    if(index != -1
        && mNodes[0] != nullptr)
    {
        mReturnObjects = mNodes[index]->retrieve(pRect);
    }

    for(Collidable* col : mObjects)
    {
        if(col != nullptr)
        {
            mReturnObjects.push_back(col);
        }
    }

    return mReturnObjects;
}
