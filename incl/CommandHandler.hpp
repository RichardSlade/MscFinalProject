#ifndef INCL_COMMANDHANDLER_HPP
#define INCL_COMMANDHANDLER_HPP

#include <map>

#include <SFML/Window/Event.hpp>

#include <Command.hpp>
#include <CommandQueue.hpp>

class CommandQueue;

class CommandHandler
{
    public:
        enum Action
        {
            Thrust,
            ReverseThrust,
            RotateRight,
            RotateLeft,
            Fire,
            NoAction,
            ActionCount
        };

    private:
        std::map<sf::Keyboard::Key, Action>         mKeyBinding;
        std::map<Action, Command>                   mActionBinding;

        void initializeActions();

    public:
                                    CommandHandler();

        bool                        isRealTimeAction(Action action);
        void                        handleRealtimeInput(CommandQueue& commands);
};

#endif // INCL_COMMANDHANDLER_HPP
