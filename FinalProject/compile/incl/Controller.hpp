#ifndef INCL_CONTROLLER_HPP
#define INCL_CONTROLLER_HPP

#include <string>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/Sound.hpp>

#include <State.hpp>
//#include <Utility.hpp>
#include <LoadSave.hpp>
#include <Constants.hpp>
#include <Enum.hpp>
#include <DirectorResults.hpp>
#include <HighScore.hpp>

class Controller
{
    private:

        static const sf::Time       TimePerFrame;

        sf::RenderWindow            mWindow;
        State*                      mCurrentState;
        State::Type                 mCurrentStateType;
        State::Type                 mNextStateType;

        sf::Clock                   mClock;
        sf::Time                    mTimeSinceLastUpdate;
        std::string                 mPlayerName;
        bool                        mFpsCap;

        Application::Type           mAppType;
        Ai::Type                    mAiType;
        Evolution::Type             mEvoType;

        sf::Sound                   mTestSound;

        std::vector<HighScore>      mHighScores;

        std::array<sf::Texture,
                    Textures::TextureCount>         mTextures;

        std::array<sf::SoundBuffer,
                    Sounds::SoundCount>             mSounds;

        void                        loadTextures();
        void                        loadSounds();
        void                        loadHighScoreData();
//        void                        loadScores();
        void                        reset();
        void                        render();

    public:
                                    Controller();

        std::vector<HighScore>&     getHighScores(){ return mHighScores; }
        void                        run();
        void                        changeState();
        void                        closeApp();

        bool                        getFpsCap(){ return mFpsCap; };
        void                        setFpsCap(bool status);

        void                        setNextState(State::Type type){ mNextStateType = type; }

        void                        setPlayerName(std::string name){ mPlayerName = name; }

        void                        setAppType(Application::Type type){ mAppType = type; }
        void                        setAiType(Ai::Type type){ mAiType = type; }
        void                        setEvoType(Evolution::Type type){ mEvoType = type; }

//        void                        saveInnovation(Ai::Type aiType
//                                                , std::vector<SInnovation> innovations)
//                                                { saveInnovationToFile(std::string("brains/"
//                                                                                 + appTypeToString(mAppType)
//                                                                                 + "/"
//                                                                                 + aiTypeToString(aiType)
//                                                                                 + "/innovation/"
//                                                                                 + "innovations.dat"), innovations);
//                                                }
//
//        void                        saveInnovationData( Ai::Type aiType
//                                                        , int neuronID
//                                                        , int innoID)
//                                                            { saveInnovationDataToFile(std::string("brains/"
//                                                                                             + appTypeToString(mAppType)
//                                                                                             + "/"
//                                                                                             + aiTypeToString(aiType)
//                                                                                             + "/innovation/"
//                                                                                             + "params.dat"), neuronID, innoID);
//                                                            }
        void                        saveInnovation(Ai::Type aiType
                                                , std::vector<SInnovation> innovations)
                                                { saveInnovationToFile(std::string("brains/"
                                                                                 + aiTypeToString(aiType)
                                                                                 + "/innovation/"
                                                                                 + "innovations.dat"), innovations);}


        void                        saveInnovationData( Ai::Type aiType
                                                        , int neuronID
                                                        , int innoID)
                                                            { saveInnovationDataToFile(std::string("brains/"
                                                                                             + aiTypeToString(aiType)
                                                                                             + "/innovation/"
                                                                                             + "params.dat"),
                                                                                             neuronID, innoID);}

//        void                        saveNeurons(int id
//                                                , Ai::Type aiType
//                                                , std::vector<SNeuronGene> neuronData)
//                                                { saveNeuronsToFile(std::string("brains/"
//                                                                                 + appTypeToString(mAppType)
//                                                                                 + "/" + aiTypeToString(aiType)
//                                                                                 + "/neurons/"
//                                                                                 + std::to_string(id)
//                                                                                 + "neurons.dat"), neuronData); }

        void                        saveNeurons(int id
                                                ,Ai::Type aiType
                                                ,std::vector<SNeuronGene> neuronData)
                                                { saveNeuronsToFile(std::string("brains/"
                                                                                 + aiTypeToString(aiType)
                                                                                 + "/neurons/"
                                                                                 + std::to_string(id)
                                                                                 + "neurons.dat"), neuronData); }

        void                        saveLinks(int id
                                                ,Ai::Type aiType
                                                ,std::vector<SLinkGene> linkData)
                                                { saveLinksToFile(std::string("brains/"
                                                                               + aiTypeToString(aiType)
                                                                               + "/links/"
                                                                               + std::to_string(id)
                                                                               + "links.dat"), linkData); }


//        void                        saveLinks(int id,
//                                               Ai::Type aiType,
//                                                std::vector<SLinkGene> linkData)
//                                                { saveLinksToFile(std::string("brains/"
//                                                                               + appTypeToString(mAppType)
//                                                                               + "/"
//                                                                               + aiTypeToString(aiType)
//                                                                               + "/links/"
//                                                                               + std::to_string(id)
//                                                                               + "links.dat"), linkData); }

//        void                        saveNeurons(int id, Ai::Type aiType1, Ai::Type type2, std::vector<SNeuronGene> neuronData){ saveNeuronsToFile(std::string("brains/" + appTypeToString(mAppType) + "/" + aiTypeToString(aiType1) + "/" + aiTypeToString(type2) + "/" + std::to_string(id) + "neurons.dat"), neuronData); }
//        void                        saveLinks(int id, Ai::Type aiType1, Ai::Type type2, std::vector<SLinkGene> linkData){ saveLinksToFile(std::string("brains/" + appTypeToString(mAppType) + "/" + aiTypeToString(aiType1) + "/" + aiTypeToString(type2) + "/" + std::to_string(id) + "links.dat"), linkData); }

//        std::vector<SNeuronGene>    loadNeurons(int id
//                                                , Ai::Type type)
//                                                { return loadNeuronsFromFile("brains/"
//                                                                              + appTypeToString(mAppType)
//                                                                              + "/"
//                                                                              + aiTypeToString(type)
//                                                                              + "/neurons/"
//                                                                              + std::to_string(id)
//                                                                              + "neurons.dat"); }

        std::vector<SNeuronGene>    loadNeurons(int id
                                                , Ai::Type type)
                                                { return loadNeuronsFromFile("brains/"
                                                                              + aiTypeToString(type)
                                                                              + "/neurons/"
                                                                              + std::to_string(id)
                                                                              + "neurons.dat"); }

        std::vector<SLinkGene>      loadLinks(int id
                                              , Ai::Type type)
                                              { return loadLinksFromFile("brains/"
                                                                          + aiTypeToString(type)
                                                                          + "/links/"
                                                                          + std::to_string(id)
                                                                          + "links.dat"); }

//        std::vector<SLinkGene>      loadLinks(int id
//                                              , Ai::Type type)
//                                              { return loadLinksFromFile("brains/"
//                                                                          + appTypeToString(mAppType)
//                                                                          + "/" + aiTypeToString(type)
//                                                                          + "/links/" + std::to_string(id)
//                                                                          + "links.dat"); }
//
//        std::vector<SNeuronGene>    loadNeurons(int id,
//                                                 Ai::Type type1,
//                                                 Ai::Type type2)
//                                                 { return loadNeuronsFromFile("brains/"
//                                                                                + appTypeToString(mAppType)
//                                                                                + "/"
//                                                                                 + aiTypeToString(type1)
//                                                                                + "/"
//                                                                                 + aiTypeToString(type2)
//                                                                                + "/neurons/"
//                                                                                 + std::to_string(id) + "neurons.dat"); }
//
//        std::vector<SLinkGene>      loadLinks(int id
//                                              , Ai::Type type1
//                                              , Ai::Type type2)
//                                              { return loadLinksFromFile("brains/"
//                                                                          + appTypeToString(mAppType)
//                                                                          + "/"
//                                                                          + aiTypeToString(type1)
//                                                                          + "/" + aiTypeToString(type2)
//                                                                          + "/links/" + std::to_string(id)
//                                                                          + "links.dat"); }

//        std::vector<SInnovation>    loadInnovations(Ai::Type type1,
//                                                    Ai::Type type2)
//                                                    { return loadInnovationFromFile("brains/"
//                                                                                + appTypeToString(mAppType)
//                                                                                + "/"
//                                                                                + aiTypeToString(type1)
//                                                                                + "/"
//                                                                                + aiTypeToString(type2)
//                                                                                + "/innovation/innovations.dat");
//                                                    }
//
//        std::pair<int, int>         loadInnovationData( Ai::Type type1,
//                                                        Ai::Type type2)
//                                                        { return loadInnovationDataFromFile("brains/"
//                                                                          + appTypeToString(mAppType)
//                                                                          + "/"
//                                                                          + aiTypeToString(type1)
//                                                                          + "/"
//                                                                          + aiTypeToString(type2)
//                                                                          + "/innovation/params.dat");
//                                                        }

        std::vector<SInnovation>    loadInnovations(Ai::Type type)
                                                    { return loadInnovationFromFile("brains/"
                                                                                + aiTypeToString(type)
                                                                                + "/ga/innovations.dat");}

        std::pair<int, int>         loadInnovationData( Ai::Type type)
                                                        { return loadInnovationDataFromFile("brains/"
                                                                          + aiTypeToString(type)
                                                                          + "/ga/innoData.dat");}

//        std::vector<SInnovation>    loadInnovations( Ai::Type type)
//                                                    { return loadInnovationFromFile("brains/"
//                                                                                + appTypeToString(mAppType)
//                                                                                + "/"
//                                                                                + aiTypeToString(type)
//                                                                                + "/innovation/innovations.dat");
//                                                    }
//
//        std::pair<int, int>         loadInnovationData( Ai::Type type)
//                                                        { return loadInnovationDataFromFile("brains/"
//                                                                          + appTypeToString(mAppType)
//                                                                          + "/"
//                                                                          + aiTypeToString(type)
//                                                                          + "/innovation/params.dat");
//                                                       }

//        void                        saveScores()
        void                        saveLevelStats(int level, DirectorResults& dR){ saveStats(std::string("results/"
                                                                                                         + aiTypeToString(mAiType)
                                                                                                         + "/" + mPlayerName
                                                                                                         + std::to_string(level)
                                                                                                         + "stats.txt")
                                                                                                        , dR);
                                                                                 }

};

#endif // INCL_CONTROLLER_HPP
