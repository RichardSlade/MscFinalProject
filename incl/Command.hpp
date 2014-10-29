#ifndef INCL_COMMAND_HPP
#define INCL_COMMAND_HPP

#include <functional>

#include <SFML/System/Time.hpp>

class Entity;

struct Command
{
    std::function<void(Entity&, sf::Time)>   action;
};

template <typename Function>
std::function<void(Entity&, sf::Time)> derivedAction(Function fn)
{
    return [=] (Entity& entity, sf::Time dt)
    {
        // Invoke function on lander
        fn(entity, dt);
    };
}

#endif // INCL_COMMAND_HPP
