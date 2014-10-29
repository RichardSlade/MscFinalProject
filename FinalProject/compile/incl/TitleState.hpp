#ifndef INCL_TITLESTATE_HPP
#define INCL_TITLESTATE_HPP

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <Controller.hpp>
#include <State.hpp>



class TitleState : public State
{
    private:

        enum StageType
        {
            AppMenu,
            GameMenu,
//            TestMenu,
            TrainMenu,
            SelectDifficulty,
            SelectEvolution,
            SelectHunterTrainType,
            Help,
            EnterName,
            StartApp,
            Quit
        };

        sf::RenderWindow&       mWindow;
        Controller*             mController;

        std::array<sf::Texture,
                    Textures::TextureCount>&         mTextures;

//        bool                    mIsGame;
        int                     mMenuSelection;

        sf::Sprite              mBackground;
        sf::Sprite              mHelpScreen;

        bool                    mStageChange;
        sf::Font                mFont;
//        sf::Text                mInfoText;

        std::vector<sf::Text>   mTexts;

        sf::Text                mUserText;

        std::string             mUserStr;
        StageType               mStage;
        Application::Type       mAppType;
        Ai::Type                mAiType;
        Evolution::Type         mEvoType;

        int                     mEvoSelection;
        int                     mDiffSelection;

        void                    setupText();
        void                    handleInput();
        void                    update(sf::Time dt);
        void                    render();

        void                    changeStage();
        void                    reset();

        StageType               menuInput(int i, StageType type);
        StageType               menuBack();
        int                     menuNum(StageType type);

    public:
                                TitleState(sf::RenderWindow& window
                                           , Controller* controller
                                           , std::array<sf::Texture, Textures::Type::TextureCount>& textures);
};

#endif // INCL_TITLESTATE_HPP
