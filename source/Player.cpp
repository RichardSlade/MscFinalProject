#include <Player.hpp>
#include <Constants.hpp>
#include <Utility.hpp>

void Player::onCommand(const Command& command, sf::Time dt)
{
    // Execute commands for SpaceShip
    command.action(*this, dt);

    if(!mHasMoved) mHasMoved = true;
}

void Player::accelerate(float accel, sf::Time dt)
{
    float currentRotation = mSprite.getRotation() * (3.14159265 / 180);

    float entityAccel = ((accel * dt.asSeconds()) / 1) * ScalingFactor;

    mVelocity.x += entityAccel * std::sin(currentRotation);
	mVelocity.y -= entityAccel * std::cos(currentRotation);
}

void Player::update(sf::Time dt)
{
    checkProjectileLaunch(dt, sf::FloatRect());

    mOldPosition = mSprite.getPosition();

    mSprite.move(mVelocity * dt.asSeconds());
}

void Player::reset()
{
    Entity::reset();
    mHitPoints = PlayerHitPoints;
}
