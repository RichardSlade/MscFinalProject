#include <algorithm>
#include <Entity.hpp>
#include <CommandHandler.hpp>
#include <Constants.hpp>

/*
    Code taken from Book SFML Game Development 2013
    Authors:
    Artur Moreiram,
    Henrik Vogelius Hansson,
    Jan Haller,
*/

// Struct acts as function to be passed to move entity
struct EntityMover
{
    EntityMover(float thrust)
    : acceleration(thrust)
    {}

    void operator() (Entity& entity, sf::Time dt) const
    {
        entity.accelerate(acceleration, dt);
    }

    float acceleration;
};

// Struct acts as function to be passed to rotate entity
struct EntityRotate
{
    EntityRotate(float angle)
    : rotation(angle)
    {}

    void operator() (Entity& entity, sf::Time dt) const
    {
        entity.rotate(rotation);
    }

    float rotation;
};

struct EntityFire
{
    EntityFire(){};

    void operator() (Entity& entity, sf::Time dt) const
    {
        entity.fire();
    }
};

// Constructor sets up keybindings map
CommandHandler::CommandHandler()
{
    mKeyBinding[sf::Keyboard::Space] = Thrust;
    mKeyBinding[sf::Keyboard::Right] = RotateRight;
    mKeyBinding[sf::Keyboard::Left] = RotateLeft;
    mKeyBinding[sf::Keyboard::V] = Fire;

    // Set initial action bindings
    initializeActions();
}

bool CommandHandler::isRealTimeAction(Action action)
{
    // Return true if real time action
	switch (action)
	{
        case Thrust:
        case ReverseThrust:
        case RotateRight:
        case RotateLeft:
        case Fire:
			return true;

		default:
			return false;
	}
}

void CommandHandler::handleRealtimeInput(CommandQueue& commands)
{
    // Traverse all assigned keys and check if they are pressed
    for(auto pair : mKeyBinding){

        // If key is pressed lookup action and trigger corresponding command
        if(sf::Keyboard::isKeyPressed(pair.first) && isRealTimeAction(pair.second))
            commands.push(mActionBinding[pair.second]);
    }
}

// Initialises functors
void CommandHandler::initializeActions()
{
    mActionBinding[Thrust].action = derivedAction<EntityMover>(EntityMover(PlayerSpeed));
    mActionBinding[ReverseThrust].action = derivedAction<EntityMover>(EntityMover(-PlayerSpeed));
    mActionBinding[RotateRight].action = derivedAction<EntityRotate>(EntityRotate(PlayerRotate));
    mActionBinding[RotateLeft].action = derivedAction<EntityRotate>(EntityRotate(-PlayerRotate));
    mActionBinding[Fire].action = derivedAction<EntityFire>(EntityFire());
}
