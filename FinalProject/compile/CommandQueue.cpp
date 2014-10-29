#include <CommandQueue.hpp>
#include <Command.hpp>

/*
    Code taken from Book SFML Game Development 2013
    Authors:
    Artur Moreiram,
    Henrik Vogelius Hansson,
    Jan Haller,
*/

void CommandQueue::push(const Command& command)
{
    mQueue.push(command);
}

Command CommandQueue::pop()
{
    Command command = mQueue.front();
    mQueue.pop();
    return command;
}

bool CommandQueue::isEmpty() const
{
    return mQueue.empty();
}
