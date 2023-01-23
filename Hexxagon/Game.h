//
// Created by Franciszek on 15/01/2023.
//

#include "Board.h"
#include "Menu.h"
#include "Pointers.h"

#ifndef HEXXAGON_GAME_H
#define HEXXAGON_GAME_H

/**
 * \class Class Game.
 *
 * \details Class responsible for maintaining gameplay loop and walking between different menus.
 */
class Game{
    Board board1;
    std::vector<std::string> saveList;
    std::vector<std::string> mainChoices{"Play vs another player", "Play vs computer", "Load saved game",  "Check leaderboard", "Exit"};
    std::vector<std::string> difficulties{"Easy", "Medium", "Return to main menu"};
    std::vector<std::string> endgame{"Return back to main menu", "Exit"};
    std::vector<std::string> lBoard{"Return to main menu"};
    std::string leaderboard;

    Menu mainMenu{" ____  ____ _________ ____  ____ ____  ____      __        ______     ____   ____  _____ \n"
                  "|_   ||   _|_   ___  |_  _||_  _|_  _||_  _|    /  \\     .' ___  |  .'    \\.|_   \\|_   _|\n"
                  "  | |__| |   | |_  \\_| \\ \\  / /   \\ \\  / /     / /\\ \\   / .'   \\_| /  .--.  \\ |   \\ | |  \n"
                  "  |  __  |   |  _|  _   > \\' <     > \\' <     / ____ \\  | |    ____| |    | | | |\\ \\| |  \n"
                  " _| |  | |_ _| |___/ |_/ /'\\\\ \\_ _/ /'\\\\ \\_ _/ /    \\ \\_\\ \\.___]  _|  \\--'  /_| |_\\   |_ \n"
                  "|____||____|_________|____||____|____||____|____|  |____|\\._____.'  \\.____.'|_____|\\____|\n",
                  mainChoices};
    Menu saveMenu{"  _______      __      ____   ____ _________  _______ \n"
                  " /  ___  |    /  \\    |_  _| |_  _|_   ___  |/  ___  |\n"
                  "|  (__ \\_|   / /\\ \\     \\ \\   / /   | |_  \\_|  (__ \\_|\n"
                  " '.___\\-.   / ____ \\     \\ \\ / /    |  _|  _ '.___\\-. \n"
                  "|\\\\____) |_/ /    \\ \\_    \\ ' /    _| |___/ |\\\\____) |\n"
                  "|_______.'____|  |____|    \\_/    |_________|_______.'\n",
                  saveList, "Choose a save file to load:"};
    Menu difficultyMenu{" ________   _____ _________ _________ _____   ______ _____  _____ _____    _________ ____  ____ \n"
                        "|_   ___ \\.|_   _|_   ___  |_   ___  |_   _|./ ___  |_   _||_   _|_   _|  |  _   _  |_  _||_  _|\n"
                        "  | |   \\. \\ | |   | |_  \\_| | |_  \\_| | | / ./   \\_| | |    | |   | |    |_/ | | \\_| \\ \\  / /  \n"
                        "  | |    | | | |   |  _|     |  _|     | | | |        | '    ' |   | |   _    | |      \\ \\/ /   \n"
                        " _| |___.' /_| |_ _| |_     _| |_     _| |_\\ \\.___.'\\  \\ \\--' /   _| |__/ |  _| |_     _|  |_   \n"
                        "|________.'|_____|_____|   |_____|   |_____|\\._____.'   \\.__.'   |________| |_____|   |______|  \n",
                        difficulties, "Choose AI difficulty"};
    Menu leaderboardMenu{" _____    _________      __      ________   _________ _______    ______     ____        __      _______    ________   \n"
                         "|_   _|  |_   ___  |    /  \\    |_   ___ \\.|_   ___  |_   __ \\  |_   _ \\  .'    \\.     /  \\    |_   __ \\  |_   ___ \\. \n"
                         "  | |      | |_  \\_|   / /\\ \\     | |   \\. \\ | |_  \\_| | |__) |   | |_) |/  .--.  \\   / /\\ \\     | |__) |   | |   \\. \\\n"
                         "  | |   _  |  _|  _   / ____ \\    | |    | | |  _|  _  |  __ /    |  __/.| |    | |  / ____ \\    |  __ /    | |    | |\n"
                         " _| |__/ |_| |___/ |_/ /    \\ \\_ _| |___.' /_| |___/ |_| |  \\ \\_ _| |__) |  \\--'  /_/ /    \\ \\_ _| |  \\ \\_ _| |___.' /\n"
                         "|________|_________|____|  |____|________.'|_________|____| |___|_______/ \\.____.'|____|  |____|____| |___|________.' \n",
                         lBoard};
    Menu endingMenu{"", endgame};

    int gamemode;
    int saveNumber;
    int aiDiff;
    int playerFinalChoice;
    bool restartFlag;

    /**
     * \brief The method loadSaves.
     *
     * \details This method checks if SaveFile folder is created if not it creates it,
     * else it loads saves form the folder into a vectors saveMenu.choices and saveList.
     */
    void loadSaves(){
        saveList.clear();
        saveMenu.emptyChoices();
        fs::path path(fs::current_path().append("./SaveFiles/"));
        //If not savefile folder create one
        if(fs::is_directory(path)){
            for(auto const &file : fs::directory_iterator(path)){
                saveMenu.addChoice(file.path().filename().string());
                saveList.push_back(file.path().filename().string());
            }
        }else{
            cout<<"SaveFile folder not found, creating..."<<endl;
            fs::create_directory(path);
        }
        saveMenu.addChoice("Exit");
    }

    /**
     * \brief The method loadLeaderboard.
     *
     * \details This method loads and interprets info form file Leaderboard.txt and returns is as a string,
     * To form a nice looking leaderboard of top 5 scores.
     *
     * @return A string that is the leaderboard.
     */
    static std::string loadLeaderboard(){
        std::string leaderboard;
        fs::path leaderboardPath = fs::current_path().append("Leaderboard.txt");
        std::string s;
        int index = 1;

        if(!leaderboardPath.empty()) {
            std::ifstream stream(leaderboardPath);
            while (getline(stream, s)) {
                std::stringstream ss(s);
                std::string subString;
                int counter = 0;
                leaderboard += std::to_string(index) + ". ";
                while(std::getline(ss, subString, ';')){
                    //Numbered pos
                    switch(counter){
                        case 0:
                            leaderboard += "Player: " + subString + "; ";
                            break;
                        case 1:
                            leaderboard += "Points: " + subString + "; ";
                            break;
                        case 2:
                            switch(static_cast<DIFFICULTY>((int)(subString[0]-'0'))){
                                case EASY:
                                    leaderboard += "On difficulty: EASY";
                                    break;
                                case MEDIUM:
                                    leaderboard += "On difficulty: MEDIUM";
                                    break;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                    counter++;
                }
                index++;
                leaderboard += "\n";
            }
        }
        return leaderboard;
    }

public:
    /**
     * \brief The method startGame.
     *
     * \details Method  responsible for maintaing game loop, displaying menu, walking between menus and processing player choices.
     */
    void startGame(){
        while (true) {
            restartFlag = false;

            gamemode = mainMenu.display();

            switch (gamemode) {
                //PVP
                case 0:
                    board1.loadBoard("Board");
                    board1.setGameMode(PVP);
                    break;

                    //PVC
                case 1:
                    board1.loadBoard("Board");
                    board1.setGameMode(PVC);
                    aiDiff = difficultyMenu.display();
                    if(aiDiff >= NumberOfTypes){
                        restartFlag = true;
                        break;
                    }
                    board1.setDifficulty(static_cast<DIFFICULTY>(aiDiff));
                    break;

                    //LOAD SAVE
                case 2:
                    loadSaves();
                    saveNumber = saveMenu.display();
                    if(saveNumber >= saveList.size()){
                        restartFlag = true;
                        break;
                    }
                    board1.loadBoard("./SaveFiles/" + saveList[saveNumber]);
                    break;

                    //LEADERBOARD
                case 3:
                    leaderboard = loadLeaderboard();
                    cout<<"Test1";
                    cout<<leaderboard;
                    leaderboardMenu.setMiniPrompt(leaderboard);
                    leaderboardMenu.display();
                    restartFlag = true;
                    break;

                    //EXIT
                case 4:
                    exit(0);

                default:
                    break;
            }
            //Return to main menu
            if(restartFlag) continue;

            //Gameplay loop
            board1.drawBoard();
            while (!board1.isWin()) {
                board1.drawBoard();
                board1.movePointer();
            }

            //After player has won choose appropriate ending screen
            if (board1.whoWon() == '1')
                endingMenu.changePrompt(
                        " ______   _____         __      ____  ____ _________ _______            __        ____  ____      __       _______     _____  _____   ____   ____  _____ \n"
                        "|_   __ \\|_   _|       /  \\    |_  _||_  _|_   ___  |_   __ \\          /  |      |_   ||   _|    /  \\     /  ___  |   |_   _||_   _|.'    \\.|_   \\|_   _|\n"
                        "  | |__) | | |        / /\\ \\     \\ \\  / /   | |_  \\_| | |__) |         \\| |        | |__| |     / /\\ \\   |  (__ \\_|     | | /\\ | | /  .--.  \\ |   \\ | |  \n"
                        "  |  ___/  | |   _   / ____ \\     \\ \\/ /    |  _|  _  |  __ /           | |        |  __  |    / ____ \\   '.___\\-.      | |/  \\| | | |    | | | |\\ \\| |  \n"
                        " _| |_    _| |__/ |_/ /    \\ \\_   _|  |_   _| |___/ |_| |  \\ \\_        _| |_      _| |  | |_ _/ /    \\ \\_|\\\\____) |     |   /\\   | \\  \\--'  /_| |_\\   |_ \n"
                        "|_____|  |________|____|  |____| |______| |_________|____| |___|      |_____|    |____||____|____|  |____|_______.'     |__/  \\__|  \\.____.'|_____|\\____|\n");
            else if (board1.whoWon() == '2')
                endingMenu.changePrompt(
                        " ______   _____         __      ____  ____ _________ _______           _____       ____  ____      __       _______     _____  _____   ____   ____  _____ \n"
                        "|_   __ \\|_   _|       /  \\    |_  _||_  _|_   ___  |_   __ \\         / ___ \\.    |_   ||   _|    /  \\     /  ___  |   |_   _||_   _|.'    \\.|_   \\|_   _|\n"
                        "  | |__) | | |        / /\\ \\     \\ \\  / /   | |_  \\_| | |__) |       |_/___) |      | |__| |     / /\\ \\   |  (__ \\_|     | | /\\ | | /  .--.  \\ |   \\ | |  \n"
                        "  |  ___/  | |   _   / ____ \\     \\ \\/ /    |  _|  _  |  __ /         .'____.'      |  __  |    / ____ \\   '.___\\-.      | |/  \\| | | |    | | | |\\ \\| |  \n"
                        " _| |_    _| |__/ |_/ /    \\ \\_   _|  |_   _| |___/ |_| |  \\ \\_      / /____       _| |  | |_ _/ /    \\ \\_|\\\\____) |     |   /\\   | \\  \\--'  /_| |_\\   |_ \n"
                        "|_____|  |________|____|  |____| |______| |_________|____| |___|     |_______|    |____||____|____|  |____|_______.'     |__/  \\__|  \\.____.'|_____|\\____|\n");
            else if (board1.whoWon() == '3')
                endingMenu.changePrompt(" ________   _______         __      _____  _____ \n"
                                        "|_   ___ \\.|_   __ \\       /  \\    |_   _||_   _|\n"
                                        "  | |   \\. \\ | |__) |     / /\\ \\     | | /\\ | |  \n"
                                        "  | |    | | |  __ /     / ____ \\    | |/  \\| |  \n"
                                        " _| |___.' /_| |  \\ \\_ _/ /    \\ \\_  |   /\\   |  \n"
                                        "|________.'|____| |___|____|  |____| |__/  \\__|  \n");
                //Else quit to main menu
            else continue;

            //Display final screen
            playerFinalChoice = endingMenu.display();
            if (playerFinalChoice == 1) exit(0);
            else continue;
        }
    }
};

#endif //HEXXAGON_GAME_H
