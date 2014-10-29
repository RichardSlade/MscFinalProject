#include <SFML/Graphics/Color.hpp>

#include <Entity.hpp>
#include <Constants.hpp>
#include <Utility.hpp>

void Entity::reset()
{
//    mScore = 0;
    mVelocity = sf::Vector2f(0.f, 0.f);
    mProjectiles.clear();
}

void Entity::destroy()
{

}

const sf::FloatRect Entity::getBounds()
{
    sf::FloatRect rect = mSprite.getGlobalBounds();

    float border = 4.f;

//    if(mType == EntityType::Player)
//    {
        rect.top -= border;
        rect.left -= border;
        rect.width += border;
        rect.height += border;
//    }
//    else if(mType == EntityType::Enemy)
//    {
//        rect.top -= EnemyBorder;
//        rect.left -= EnemyBorder;
//        rect.width += EnemyBorder;
//        rect.height += EnemyBorder;
//    }
//
    return rect;

//    return mSprite.getGlobalBounds();
}

void Entity::checkProjectileLaunch(sf::Time dt, sf::FloatRect viewBounds)
{
	// Check for automatic gunfire, allow only in intervals
	if (mFireProjectile && mFireCountdown <= sf::Time::Zero)
	{
		// Interval expired: We can fire a new bullet
		fireProjectile(viewBounds);

		if(mType == EntityType::Enemy)
		{
		    mFireCountdown += sf::seconds(1.f / 2.f);
		}
        else if(mType == EntityType::Player)
        {
            mFireCountdown += sf::seconds(1.f / 8.f);

        }

		mFireProjectile = false;
	}
	else if(mFireCountdown > sf::Time::Zero)
	{
		// Interval not expired: Decrease it further
		mFireCountdown -= dt;
		mFireProjectile = false;
	}

	updateProjectiles(dt);
}


void Entity::fireProjectile(sf::FloatRect viewBounds)
{
    sf::Vector2f vel;

    float currentRotation = mSprite.getRotation() * (3.14159265 / 180);

    vel.x = sin(currentRotation) * ProjectileSpeed;
    vel.y = -cos(currentRotation) * ProjectileSpeed;

    if(mProjectiles.size() == 0)
    {
        mProjectiles.push_back(Projectile(mSprite.getPosition(), mSprite.getRotation(), vel));
    }
    else
    {
        int index = 0;

        for(Projectile& proj : mProjectiles)
        {
            if(!proj.isActive)
            {
                proj = Projectile(mSprite.getPosition(), mSprite.getRotation(), vel);
                break;
            }

            index ++;
        }

        if(index = mProjectiles.size())
        {
            mProjectiles.push_back(Projectile(mSprite.getPosition(), mSprite.getRotation(), vel));
        }
    }

    if(mType == EntityType::Player)
        mLaserSound.play();
    else if(getBounds().intersects(viewBounds))
        mLaserSound.play();
}

void Entity::updateProjectiles(sf::Time dt)
{
    for(Projectile& proj : mProjectiles)
    {
        if(proj.isActive)
        {
            proj.projectile.setPosition(proj.projectile.getPosition() + (proj.velocity));
        }
    }
}

void Entity::reverseMove()
{
    if(mType == EntityType::Player)
    {
        mSprite.setPosition(mOldPosition);
        mVelocity = (-mVelocity / 2.f);
    }
    else
    {
        mSprite.setPosition(mOldPosition);
        mVelocity = sf::Vector2f(0.f, 0.f);
    }
}
