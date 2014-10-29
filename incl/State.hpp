#ifndef INCL_STATE_HPP
#define INCL_STATE_HPP

#include <SFML/System/Time.hpp>

class State
{
    public:

        enum Type
        {
            Title,
            Game
        };

//        virtual             ~State();

//        virtual void        run();
        virtual void        handleInput(){};
        virtual void        update(sf::Time dt){};
        virtual void        render(){};

        virtual void        implantBrainsFromFile(std::vector<double> brains){};
        virtual void        putBrainsGA(std::vector<double> brians){};
};

#endif // INCL_STATE_HPP
