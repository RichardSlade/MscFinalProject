#include <iostream>
#include <string>

#include <SFML/Window/Event.hpp>

#include <TitleState.hpp>
#include <Utility.hpp>
#include <Constants.hpp>

TitleState::TitleState(sf::RenderWindow& window, Controller* controller, std::array<sf::Texture, Textures::Type::TextureCount>& textures)
: mWindow(window)
, mController(controller)
, mTextures(textures)
, mUserStr("")
, mStage(StageType::GameMenu)
, mEvoType(Evolution::NoEvolution)
, mAiType(Ai::Easy)
, mEvoSelection(1)
, mDiffSelection(1)
, mMenuSelection(1)
, mStageChange(true)
{
    sf::View view = mWindow.getDefaultView();

    setupText();

    sf::IntRect viewBounds(mWindow.getViewport(view));

    mBackground.setTexture(mTextures[Textures::Type::Background]);
    sf::FloatRect bounds(mBackground.getLocalBounds());
    mBackground.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    mBackground.setPosition(viewBounds.width / 4.f, viewBounds.height / 2.f);

    mHelpScreen.setTexture(mTextures[Textures::Type::Help]);
    bounds = mHelpScreen.getLocalBounds();
    mHelpScreen.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

    mHelpScreen.setPosition(viewBounds.width / 2.f, viewBounds.height / 2.f);

    mWindow.setView(view);
}

void TitleState::setupText()
{
    std::string fileName = "media/Sansation.ttf";

    if(!mFont.loadFromFile(fileName))
       throw std::runtime_error("Font - Failed to load " + fileName);

    for(int i = 0; i < 6; i++)
    {
        mTexts.push_back(sf::Text(" ", mFont, GameFontSize));
        sf::FloatRect strBounds(mTexts[i].getLocalBounds());
        mTexts[i].setOrigin(strBounds.width / 2.f, strBounds.height / 2.f);
        mTexts[i].setCharacterSize(MenuFontSize);
    }

    mTexts[0].setCharacterSize(32);

    mUserText.setFont(mFont);
    mUserText.setCharacterSize(MenuFontSize);
}

void TitleState::handleInput()
{
    sf::Event event;

    while(mWindow.pollEvent(event))
    {
        if(event.type == sf::Event::Closed)
            mController->closeApp();

        if(event.type == sf::Event::KeyReleased
           && event.key.code == sf::Keyboard::Escape)
        {
            if(mStage != StageType::TrainMenu)
            {
                reset();
                mStage = menuBack();
                mStageChange = true;
            }

            break;
        }
        else if(mStage != StageType::EnterName)
        {
            if(event.type == sf::Event::KeyReleased)
            {
                switch(event.key.code)
                {
                    case sf::Keyboard::Up: // '1' Pressed
                    {
                        mMenuSelection --;

                        if(mMenuSelection < 1)
                            mMenuSelection = menuNum(mStage);

                        break;
                    }
                    case sf::Keyboard::Down:// '2' Pressed
                    {
                        mMenuSelection ++;

                        if(mMenuSelection > menuNum(mStage))
                            mMenuSelection = 1;

                        break;
                    }
                    case sf::Keyboard::Return:
                    {
                        if(mStage != StageType::EnterName)
                        {
                            mStage = menuInput(mMenuSelection, mStage);
                            mStageChange = true;
                        }

                        break;
                    }
                }
            }
        }
        else if(mStage == StageType::EnterName)
        {
            if(event.type == sf::Event::TextEntered)
            {
                std::string msg = "Enter valid name";

                if(event.text.unicode == 13) // If Enter key pressed
                {
                    if(mUserStr.size() > 0
                       && mUserStr != msg)
                    {
                        if(mUserStr[0] != ' ')
                        {
                            mController->setPlayerName(mUserStr);
                            mUserStr = "";
                            mStage = StageType::StartApp;
                            mStageChange = true;
                        }
                        else
                        {
                            mUserStr = msg;
                        }
                    }
                    else
                    {
                        mUserStr = msg;
                    }
                }
                else if(event.text.unicode == 8) // If backspace pressed
                {
                    if(mUserStr != msg)
                        mUserStr = mUserStr.substr(0, mUserStr.length() - 1);
                }
                else // Else if key pressed
                {
                    if(mUserStr == msg)
                        mUserStr.clear();

                    if(!mStageChange && mUserStr.size() < 15)
                        mUserStr += static_cast<char>(event.text.unicode);
                }
            }
        }
    }

}

void TitleState::update(sf::Time dt)
{
    if(mStageChange)
        changeStage();

    mUserText.setString(mUserStr);

    sf::FloatRect strBounds(mUserText.getLocalBounds());
    sf::Vector2f oldPos(mUserText.getPosition());

    mUserText.setOrigin(strBounds.width / 2.f, strBounds.top);
    mUserText.setPosition(oldPos);

    if(mStage != StageType::EnterName)
    {
        for(int i = 1; i < mTexts.size(); i++)
            mTexts[i].setColor(sf::Color(125, 125, 125, 255));

        if(mStage == StageType::SelectEvolution)
            mTexts[mEvoSelection].setColor(sf::Color::Green);
        else if(mStage == StageType::SelectDifficulty)
            mTexts[mDiffSelection].setColor(sf::Color::Green);

        mTexts[mMenuSelection].setColor(sf::Color(255, 255, 0));

        if(mStage == StageType::GameMenu
           && (!fileExist(Ai::Harvester) || !fileExist(Ai::Hunter)))
            mTexts[1].setColor(sf::Color::Red);

        if(mStage == StageType::SelectEvolution
           && (!fileExist(Ai::Harvester) || !fileExist(Ai::Hunter)))
            mTexts[3].setColor(sf::Color::Red);

        if(mStage == StageType::SelectHunterTrainType
           && !fileExist(Ai::Harvester))
            mTexts[1].setColor(sf::Color::Red);
    }
}

void TitleState::render()
{
    if(mStage != StageType::Help)
    {
        mWindow.draw(mBackground);

        for(sf::Text& txt : mTexts)
            mWindow.draw(txt);

        mWindow.draw(mUserText);
    }
    else
    {
        mWindow.draw(mHelpScreen);
    }

}

void TitleState::changeStage()
{
    float spacing = 30.f;

    for(sf::Text& text : mTexts)
        text.setString("");

    switch(mStage)
    {
        case StageType::AppMenu:
        {
            mTexts[0].setString("Please choose application type:");
            mTexts[1].setString("Game");
            mTexts[2].setString("Training");
            mTexts[3].setString("Quit");

            sf::View view = mWindow.getDefaultView();
                sf::Vector2f textPos((view.getSize().x / 2.f) + (view.getSize().x / 8.f), view.getSize().y / 2.f);

            int inc = spacing;

            for(sf::Text& txt : mTexts)
            {
                sf::FloatRect bounds = txt.getGlobalBounds();
                txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

                txt.setPosition(textPos.x, textPos.y + inc);

                inc += spacing;
            }

            break;
        }
        case StageType::GameMenu:
        {
            mTexts[0].setString("Menu");

            if(fileExist(Ai::Harvester)
               && fileExist(Ai::Hunter))
            {
                mTexts[1].setString("Start Game");
            }
            else
            {
                mTexts[1].setString("No brains available for this option - Please train some");
            }

            mTexts[2].setString("Select Evolution type");
            mTexts[3].setString("Select Difficulty");
            mTexts[4].setString("Help");
            mTexts[5].setString("Quit");

            sf::View view = mWindow.getDefaultView();
            sf::Vector2f textPos((view.getSize().x / 2.f) + (view.getSize().x / 8.f), view.getSize().y / 2.f);

            int inc = spacing;

            for(sf::Text& txt : mTexts)
            {
                sf::FloatRect bounds = txt.getGlobalBounds();
                txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

                txt.setPosition(textPos.x, textPos.y + inc);

                inc += spacing;
            }

            break;
        }
        case StageType::SelectEvolution:
        {
            mTexts[0].setString("Select Evolution type");
//            mTexts[1].setString("NEAT evolution");
//            mTexts[2].setString("rtNeat evolution");
            mTexts[2].setString("rtNeat evolution");

            if(fileExist(Ai::Harvester)
               && fileExist(Ai::Hunter))
            {
                mTexts[1].setString("Brains pre trained with NEAT");
            }
            else
            {
                mTexts[1].setString("No brains available for this option - Please train some");
            }

            mTexts[2].setString("rtNeat evolution");

            sf::View view = mWindow.getDefaultView();
            sf::Vector2f textPos((view.getSize().x / 2.f) + (view.getSize().x / 8.f), view.getSize().y / 2.f);

            int inc = spacing;

            for(sf::Text& txt : mTexts)
            {
                sf::FloatRect bounds = txt.getGlobalBounds();
                txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

                txt.setPosition(textPos.x, textPos.y + inc);

                inc += spacing;
            }

            break;
        }
        case StageType::SelectDifficulty:
        {

            mTexts[0].setString("Select Difficulty");
            mTexts[1].setString("Easy");
            mTexts[2].setString("Medium");
            mTexts[3].setString("Hard");

            sf::View view = mWindow.getDefaultView();
            sf::Vector2f textPos((view.getSize().x / 2.f) + (view.getSize().x / 8.f), view.getSize().y / 2.f);

            int inc = spacing;

            for(sf::Text& txt : mTexts)
            {
                sf::FloatRect bounds = txt.getGlobalBounds();
                txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

                txt.setPosition(textPos.x, textPos.y + inc);

                inc += spacing;
            }

            break;
        }
        case StageType::EnterName:
        {
            for(sf::Text& txt : mTexts)
                txt.setString("");

            mTexts[0].setString("Please enter your name:");
            mTexts[0].setColor(sf::Color(255, 255, 255));

            sf::FloatRect bounds = mTexts[0].getGlobalBounds();
            mTexts[0].setOrigin(bounds.width / 2.f, bounds.height / 2.f);

            sf::View view = mWindow.getDefaultView();
            sf::Vector2f textPos((view.getSize().x / 2.f) + (view.getSize().x / 8.f), view.getSize().y / 2.f);

            mTexts[0].setPosition(textPos);
            mUserText.setPosition(textPos.x, textPos.y + spacing);

            break;
        }
        case StageType::TrainMenu:
        {
            for(sf::Text& txt : mTexts)
                txt.setString("");

            mTexts[0].setString("Ai Type");
            mTexts[1].setString("Harvester");
            mTexts[2].setString("Hunter");
            mTexts[3].setString("Quit");

            sf::View view = mWindow.getDefaultView();
            sf::Vector2f textPos((view.getSize().x / 2.f) + (view.getSize().x / 8.f), view.getSize().y / 2.f);

            int inc = spacing;

            for(sf::Text& txt : mTexts)
            {
                sf::FloatRect bounds = txt.getGlobalBounds();
                txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

                txt.setPosition(textPos.x, textPos.y + inc);

                inc += spacing;
            }

            break;
        }
        case StageType::SelectHunterTrainType:
        {
            mTexts[0].setString("Select Training type");

            if(fileExist(Ai::Harvester))
                mTexts[1].setString("With pseudo player bots");
            else
                mTexts[1].setString("No harvester brains trained");

            mTexts[2].setString("With player");

            sf::View view = mWindow.getDefaultView();
            sf::Vector2f textPos((view.getSize().x / 2.f) + (view.getSize().x / 8.f), view.getSize().y / 2.f);

            int inc = spacing;

            for(sf::Text& txt : mTexts)
            {
                sf::FloatRect bounds = txt.getGlobalBounds();
                txt.setOrigin(bounds.width / 2.f, bounds.height / 2.f);

                txt.setPosition(textPos.x, textPos.y + inc);

                inc += spacing;
            }

            break;
        }
        case StageType::StartApp:
        {
            mController->setAppType(mAppType);
            mController->setAiType(mAiType);
            mController->setEvoType(mEvoType);
            mController->setNextState(State::Type::Game);

            break;
        }
        case StageType::Quit:
        {
            mController->closeApp();
            break;
        }
        default: break;
    }

    mMenuSelection = 1;
    mStageChange = false;
}

void TitleState::reset()
{
    mUserStr = "";
    mUserText.setString("");
    mMenuSelection = 1;
}

TitleState::StageType TitleState::menuBack()
{
    switch(mStage)
    {
        case StageType::AppMenu:
        case StageType::TrainMenu:
//        case StageType::GameMenu: return StageType::AppMenu;
        case StageType::GameMenu: return StageType::GameMenu;
        case StageType::SelectHunterTrainType: return StageType::TrainMenu;
        case StageType::SelectEvolution:
        case StageType::SelectDifficulty:
        case StageType::EnterName:
        case StageType::Help: return StageType::GameMenu;
        case StageType::Quit: return StageType::Quit;
    }
}

TitleState::StageType TitleState::menuInput(int menuChoice, StageType type)
{
    switch(type)
    {
        case StageType::AppMenu:
        {
            switch(menuChoice)
            {
                case 1: return StageType::GameMenu;
                case 2: return StageType::TrainMenu;
                case 3: return StageType::Quit;
            }
        }
        case StageType::GameMenu:
        {
            mAppType = Application::Game;

            switch(menuChoice)
            {
                case 1: return StageType::EnterName;
                case 2: return StageType::SelectEvolution;
                case 3: return StageType::SelectDifficulty;
                case 4: return StageType::Help;
                case 5: return StageType::Quit;
            }
        }
        case StageType::SelectDifficulty:
        {
            switch(menuChoice)
            {
                case 1: mAiType = Ai::Easy; break;
                case 2: mAiType = Ai::Medium; break;
                case 3: mAiType = Ai::Hard; break;
            }

            mDiffSelection = menuChoice;

            return StageType::GameMenu;
        }
        case StageType::SelectEvolution:
        {
            switch(menuChoice)
            {
//                case 1: mEvoType = Evolution::NEATEvolution; break;
                case 1:
                {
                    if(fileExist(Ai::Harvester)
                       && fileExist(Ai::Hunter))
                    {
                        mEvoType = Evolution::NoEvolution; break;
                    }
                    else
                    {
                        break;
                    }
                }
                case 2: mEvoType = Evolution::RealTime; break;
            }

            mEvoSelection = menuChoice;

            return StageType::GameMenu;
        }
        case StageType::TrainMenu:
        {
            mAppType = Application::Training;
            mEvoType = Evolution::Train;

            switch(menuChoice)
            {
                case 1: mAiType = Ai::Harvester; return StageType::StartApp;
//                case 2: mAiType = Ai::Hunter; return StageType::SelectHunterTrainType;
                case 2: mAiType = Ai::Hunter; return StageType::StartApp;
                case 3: return StageType::Quit;
            }
        }
        case StageType::SelectHunterTrainType:
        {
            switch(menuChoice)
            {
                case 1:
                {
                    if(fileExist(Ai::Harvester))
                    {
                        mEvoType = Evolution::Train;
                        return StageType::StartApp;
                    }
                    else
                    {
                        return StageType::SelectHunterTrainType;
                    }
                }
                case 2:
                {
                    mEvoType = Evolution::TrainWithPlayer;
                    return StageType::StartApp;
                }
            }
        }
    }
}

int TitleState::menuNum(StageType type)
{
    switch(type)
    {
        case StageType::AppMenu: return 3;
        case StageType::GameMenu: return 5;
        case StageType::SelectEvolution: return 2;
        case StageType::SelectDifficulty: return 3;
        case StageType::SelectHunterTrainType: return 2;
        case StageType::TrainMenu: return 3;
    }
}

