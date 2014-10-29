#ifndef INCL_COMMANDQUEUE_HPP
#define INCL_COMMANDQUEUE_HPP

#include <queue>

class Command;

class CommandQueue
{
    private:
        std::queue<Command>     mQueue;

    public:
        void                    push(const Command& command);
        Command                 pop();
        bool                    isEmpty() const;
};

#endif // INCL_COMMANDQUEUE_HPP
