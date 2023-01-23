//
// Created by Franciszek on 13/01/2023.
//

#include <iostream>
#include <utility>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include <windows.h>
#include <algorithm>
#include <conio.h>
#include <random>
#include "Pointers.h"

#define BLUE    "\033[1m\033[34m"
#define RED     "\033[1m\033[31m"
#define GREEN   "\033[1m\033[32m"
#define YELLOW  "\033[1m\033[33m"
#define WHITE "\033[1m\033[37m"
#define WHITEBG "\033[1m\033[47m"
#define RESET   "\033[0m"

using std::cout, std::endl, std::cin;
namespace fs=std::filesystem;
///////////////////////////////////////////////////
//IF USING CLION SET THIS TO TRUE;
const bool cLion = false;
///////////////////////////////////////////////////
enum GAME_MODE{PVP, PVC};
enum DIFFICULTY{EASY, MEDIUM, NumberOfTypes};

#ifndef HEXXAGON_BOARD_H
#define HEXXAGON_BOARD_H


/**
 * \struct Struct holding info about position in leaderboard.
 *
 * \details This struct contains name of player, their score and difficulty they got score at.
 *
 * \param[in,out] pName Name of player.
 * \param[in,out] score Score the player got.
 * \param[in,out] diff  Difficulty they got the score at.
 */
struct LeaderboardPos{
    std::string pName;
    int score;
    DIFFICULTY diff;
};

/**
 * \struct Move struct.
 *
 * \details Struct responsible for holding information about moves a pawn can make.
 *
 * \param[in,out] pawnPos     Starting position of pawn.
 * \param[in,out] moveTarget  End position of pawn.
 * \param[in,out] points      How many points a move is worth.
 */
struct Move{
    Point pawnPos{};
    Point moveTarget{};
    int points;

    Move(Point selectedPos, Point targetPos, int pointsC){
        pawnPos = selectedPos;
        moveTarget = targetPos;
        points = pointsC;
    }

public:
    void printMove() const{
        cout<< "From: (" << pawnPos.getX() << ", " << pawnPos.getY() << "); To: (" << moveTarget.getX() << ", " << moveTarget.getY() << "); Points: " << points << endl;
    }
};

/**
 * \class Board class.
 *
 * \detatils Class responsible for maintaining state of the board, updating it,
 * moving the cursor on the board, loading and saving the board, and moving pawns on the board.
 */
class Board{

    std::vector<std::vector<char>> boardState;
    int p1Score = 1;
    int p2Score = 1;
    int tilesLeft = 1;
    char currentPlayer = '1';
    int iteration = 0;
    bool selectionMode = false;
    bool suddenExit = false;

    GAME_MODE mode = PVC;
    DIFFICULTY difficulty = MEDIUM;
    Pointer pointer{2,14};
    bool debug = false;
    fs::path currentPath = fs::current_path();
    std::string save;
    std::string errorMsg;
    std::string debugErrorMsg;
    std::vector<Point> p1Pawns;
    std::vector<Point> p2Pawns;

    void resetParameters(){
        suddenExit = false;
        boardState.clear();
        p1Score = 1;
        p2Score = 1;
        tilesLeft = 1;
        currentPlayer = '1';
        iteration = 0;
        selectionMode = false;
    }

public:

    Board()= default;

    /**
     * \brief The method saveBoard.
     *
     * \details This method saves current state of the board (pawns, their location, and empty fields),
     * current player, game mode and if need AI difficulty to a file given as a parameter, in a saveFile folder.
     *
     * @param fileName name of files sate of board should be saved as.
     */
    void saveBoard(const std::string& fileName){
        int isLineEven = 0;
        std::ofstream saveFile(currentPath.append("./SaveFiles/"+fileName));
        saveFile << currentPlayer << " " << (int)mode << " " << (int)difficulty << "\n";
        for(const std::vector<char>& line : boardState) {
            if (isLineEven % 2 == 1) saveFile << "\t";
            for (char pos: line) {
                saveFile << pos << "\t\t";
            }
            isLineEven++;
            saveFile << "\n";
        }
        saveFile.close();
        currentPath = fs::current_path();
    }

    /**
     * \brief The method loadBoard.
     *
     * \details the function loads (from SaveFile folder) board state(and possibly current player, mode and AI difficulty) form a file name given as a parameter.
     *
     * @param board file name which should be loaded.
     */
    void loadBoard(const std::string& board){
        //Before loading reset all parameters
        resetParameters();
        int counter = 0;
        bool firstLine = true;
        std::string s;
        currentPath.append(board);
        if(!currentPath.empty()) {
            std::ifstream stream(currentPath);
            while (getline(stream, s)) {
                //If loading save first load current player, gamemode and ai difficulty
                if(board != "Board" && firstLine){
                    currentPlayer = s[0];
                    mode = static_cast<GAME_MODE>((int)(s[2]-'0'));
                    try {
                        difficulty = static_cast<DIFFICULTY>((int)(s[4]-'0'));
                    }catch (std::out_of_range &e){
                        debugErrorMsg = "No difficulty set";
                    }
                    firstLine = false;
                }
                    //Else load normally
                else{
                    boardState.push_back(*new std::vector<char>());
                    //Erase all spaces
                    s.erase(std::remove(s.begin(), s.end(), ' '), s.end());
                    s.erase(std::remove(s.begin(), s.end(), '\t'), s.end());
                    //Add every character to a vector
                    for (char character: s) {
                        boardState[counter].push_back(character);
                    }
                    counter++;
                }
            }
        }
        else{
            cout<<"Board file not found at path: "<< currentPath << endl;
        }
        currentPath = fs::current_path();
    }

    /**
     * \brief The method drawBoard.
     *
     * \details This method is responsible for drawing the board from attribute boardState, draw additional information
     * on the right such as steering or current player number, draw cursor position, and if needed to a message to player.
     * It also collects coordinates of all the pawns and stores them in p1Pawns and p2Pawns.
     */
    void drawBoard(){
        int currentX = 0, currentY = 0;
        p1Pawns.clear();
        p2Pawns.clear();
        //Clear console
        if (!debug) system("cls");
        if (cLion) Sleep(70);
        //Draw board
        int isLineEven = 0;
        if(!boardState.empty()){
            //Read board state and print it
            for(const std::vector<char>& line : boardState) {
                //If odd line add tab and begin counting X from 1 not 0
                if (isLineEven % 2 == 1){
                    cout << "\t";
                    currentX = 1;
                }
                // For each X in Y
                for (char pos: line) {
                    //highlight pointer position
                    if(currentX == pointer.getCurrPos().getX() && currentY == pointer.getCurrPos().getY()) cout<< WHITEBG;
                    //Highlight fields that I can move to
                    Point selectedPos = pointer.getSelectedPos();
                    if(!(selectedPos.getX() == 0 && selectedPos.getY() == 0)){
                        if(checkInnerHex(Point{currentX, currentY}, selectedPos)) cout << GREEN;
                        else if(checkOuterHex(Point{currentX, currentY}, selectedPos)) cout << YELLOW;
                    }
                    //Write symbol for each position
                    switch (pos) {
                        case 'X':
                            cout << "\t\t";
                            break;
                        case 'O':
                            if(debug) cout << currentX << ","<< currentY << "," << "O\t\t";
                            else cout << "O\t\t";
                            break;
                        case '1':
                            //When drawing board save pawn positions
                            cout << BLUE << 1 << "\t\t";
                            p1Pawns.push_back(Point{currentX ,currentY});
                            break;
                        case '2':
                            //When drawing board save pawn positions
                            cout << RED << 2 << "\t\t";
                            p2Pawns.push_back(Point{currentX ,currentY});
                            break;
                        default:
                            cout << "\t\t";
                            break;
                    }
                    cout << RESET;
                    currentX += 2;
                }
                if (isLineEven % 2 == 1)cout << "\t";

                //Right side information
                updateScore();
                switch(isLineEven){
                    //Line 0 and 1 Display information about number of take tiles.
                    case 0:
                        cout << "This is the turn of player ";
                        if(currentPlayer == '1') cout << BLUE << currentPlayer << RESET;
                        else cout << RED << currentPlayer << RESET;
                        if (debug) cout << "Current mode: " << mode << " " << "AI difficulty: " << difficulty;
                        break;
                    case 1:
                        cout << "Number of tiles taken by player 1: " << BLUE << p1Score << RESET;
                        break;
                    case 2:
                        cout << "Number of tiles taken by player 2: " << RED << p2Score << RESET;
                        break;
                    case 3:
                        cout << "Number of free tiles left: " << tilesLeft;
                        break;
                    case 4:
                        if (debug) cout << "Nearby sum: " << checkNearby(pointer.getCurrPos());
                        break;
                    case 5:
                        cout << "Steering:";
                        break;
                    case 6:
                        cout << "w - UP";
                        break;
                    case 7:
                        cout << "s - DOWN";
                        break;
                    case 8:
                        cout << "a - LEFT";
                        break;
                    case 9:
                        cout << "d - RIGHT";
                        break;
                    case 10:
                        cout << "space - SELECT";
                        break;
                    case 11:
                        cout << "q - QUIT AND SAVE";
                        break;
                    case 12:
                        cout << "Q - QUIT WITHOUT SAVING";
                        break;
                    case 13:
                        if (debug) cout << "Selected pos: " << pointer.getSelectedPos().getX() << ", " << pointer.getSelectedPos().getY();
                        break;
                    case 14:
                        if (debug) cout << "Current pos: " << pointer.getCurrPos().getX() << ", " << pointer.getCurrPos().getY();
                        break;
                    case 15:
                        if (debug) cout << "Cursor is on tile: " << getValueAtPos(pointer.getCurrPos().getX(), pointer.getCurrPos().getY());
                        break;
                    case 16:
                        if (debug) cout << "Iteration: " << iteration;
                        break;
                    case 17:
                        if (debug) cout << "Debug error message: " << debugErrorMsg;
                        break;
                    default:
                        cout << "";
                }

                isLineEven++;
                currentY++;
                currentX = 0;
                cout << "" << endl;
            }
            //Print a message if needed to
            for(int i = 0; i < 33-(errorMsg.length()/2); i++){
                cout << " ";
            }
            cout << RED << errorMsg << RESET << endl;
            errorMsg = "";
            iteration+=1;
        }else{
            cout << "No board state, please load board first" <<  endl;
        }
    }

    /**
     * \brief The method updateScore.
     *
     * \details This method counts all empty spaces, and player pawns and stores their numbers in class attributes.
     */
    void updateScore(){
        p1Score = 0;
        p2Score = 0;
        tilesLeft = 0;
        for(const std::vector<char>& line : boardState) {
            for (char pos: line){
                switch(pos){
                    case 'O':
                        tilesLeft++;
                        break;
                    case '1':
                        p1Score++;
                        break;
                    case '2':
                        p2Score++;
                        break;
                    default:
                        break;
                }
            }
        }
    }

    /**
     * \brief The method movePointer
     *
     * \details This method is responsible taking input from player, and taking action in response to it.
     * Such actions include, changing pointer position, selecting pawn, moving selected pawn, entering debug mode,
     * returning to main menu, and setting win condition to true.
     */
    void movePointer(){
        int input;
        if (!cLion) {
            input = getch();
            //If pressed arrow get the second value
            if (input == 224) input = getch();
        }else{
            input = getch();
            getch();
        }

        int currentY = pointer.getCurrPos().getY(), currentX = pointer.getCurrPos().getX();
        switch(input){
            //MOVE UP
            case 72:
            case 'w':
                if(canMove(currentX, currentY-2)) pointer.getCurrPos().setPos(currentX, currentY-2);//If can move up
                else if(canMove(currentX+1, currentY-1)) pointer.getCurrPos().setPos(currentX+1, currentY-1);//Else if can move up right
                else if(canMove(currentX-1, currentY-1)) pointer.getCurrPos().setPos(currentX-1, currentY-1);//Else if can move up left
                else {}//Stay in the same place
                break;
            //MOVE DOWN
            case 80:
            case 's':
                if(canMove(currentX, currentY+2)) pointer.getCurrPos().setPos(currentX, currentY+2);//If can move down
                else if(canMove(currentX+1, currentY+1)) pointer.getCurrPos().setPos(currentX+1, currentY+1);//Else if can move down right
                else if(canMove(currentX-1, currentY+1)) pointer.getCurrPos().setPos(currentX-1, currentY+1);//Else if can move down left
                else {}//Stay in the same place
                break;
            //MOVE RIGHT
            case 77:
            case 'd':
                if(canMove(currentX+1, currentY-1)) pointer.getCurrPos().setPos(currentX+1, currentY-1);//If can move up right
                else if(canMove(currentX+1, currentY+1)) pointer.getCurrPos().setPos(currentX+1, currentY+1);//Else if can move down right
                else {}//Stay in the same place
                break;
            //MOVE LEFT
            case 75:
            case 'a':
                if(canMove(currentX-1, currentY-1)) pointer.getCurrPos().setPos(currentX-1, currentY-1);//If can move up left
                else if(canMove(currentX-1, currentY+1)) pointer.getCurrPos().setPos(currentX-1, currentY+1);//Else if can move down left
                else {}//Stay in the same place
                break;
            //QUIT
            case 'Q':
                suddenExit = true;
                break;
            //QUIT AND SAVE
            case 'q':
                cout << "Please write name of the file you want to save as: ";
                std::getline(cin, save);
                saveBoard(save);
                suddenExit = true;
                break;
            //SELECT CURRENT TILE
            case (char)13:
            case 32:
                if(selectionMode){
                    //Select other tile to move to
                    //If selecting the same tile again unselect
                    if (currentY == pointer.getSelectedPos().getY() && currentX == pointer.getSelectedPos().getX()) {
                        //Deselect
                        pointer.getSelectedPos().setPos(0, 0);
                        //Exit selection mode
                        selectionMode = false;
                    }
                        //Else move to that tile
                    else if (movePawn(pointer.getSelectedPos(), pointer.getCurrPos())) {
                        //If move successful change player and deselect
                        selectionMode = false;
                        pointer.getSelectedPos().setPos(0, 0);
                        //Change player
                        if (mode == PVP) currentPlayer = getOtherPlayer();
                        else if (mode == PVC) {
                            currentPlayer = getOtherPlayer();
                            drawBoard();
                            try {
                                //If possible to move, then do it
                                Move compMove{computerMove(difficulty, p2Pawns)};
                                movePawn(compMove.pawnPos, compMove.moveTarget);
                            }catch (std::exception e){
                                //If AI can't move end game
                                tilesLeft = 0;
                            }
                            currentPlayer = getOtherPlayer();
                        }

                    }else debugErrorMsg = "Move unsuccessful";
                }else {
                    //If selecting wrong tile send error message
                    if (getValueAtPos(pointer.getCurrPos().getX(), pointer.getCurrPos().getY()) != currentPlayer)
                        errorMsg = "You can't select that position";
                        //else select the tile and enter selection mode
                    else {
                        pointer.getSelectedPos().setPos(pointer.getCurrPos().getX(), pointer.getCurrPos().getY());
                        selectionMode = true;
                    }
                }
                break;
            //TURN ON DEBUG MODE
            case '|':
                //Turn debug mode on
                debug = !debug;
                break;
            //INSTA_WIN
            case '`':
                if (debug) tilesLeft = 0;
            default:
                break;
        }
    }

    /**
     * \brief The method movePawn.
     *
     * \details this method first checks if pawn can be moved to endPos if it can it either move it there or duplicates it
     * if endPos is in any of 6 tile around startingPos.
     *
     * @param[in] startingPos Pos the pawn is standing at.
     * @param[in] endPos Pos the pawn should be moved to.
     * @return function return true if pawn was moved and false if it was not.
     */
    bool movePawn(Point startingPos, Point endPos){

        //If selecting wrong tile send error msg
        if (getValueAtPos(endPos.getX(), endPos.getY()) != 'O') {
            errorMsg = "You can't move to that tile";
            return false;
        }
            //If selecting tile in inner Hexagon dupe the hexagon and deselect
        else if(checkInnerHex(endPos ,startingPos)) {
            changeValueAtPos(endPos.getX(), endPos.getY(), currentPlayer);
            seizeNearby(endPos, currentPlayer);
            //Deselect
            pointer.getSelectedPos().setPos(0, 0);
            return true;
        }
            //If selecting outer hex move hexagon to that pos
        else if(checkOuterHex(endPos,startingPos)) {
            changeValueAtPos(endPos.getX(), endPos.getY(), currentPlayer);
            changeValueAtPos(startingPos.getX(), startingPos.getY(), 'O');
            seizeNearby(endPos, currentPlayer);
            return true;
        }
            //Else print message that you cannot move to that tile
        else{
            errorMsg = "You can't move to this tile";
            return false;
        }

    }

    /**
     * \brief The method checkInnerHex.
     *
     * \details This method checks if selected pos is within 6 closed places around starting pos.
     *
     * @param[in] startingPos Position used as a reference for checking 6 closest tiles.
     * @param[in] selectedPos Position to be checked if it is close to startingPos.
     * @return returns true if selected pos is within those places otherwise returns false;
     */
    static bool checkInnerHex(Point startingPos, Point selectedPos){
        if ((startingPos.getX() >= selectedPos.getX()-1 && startingPos.getX() <= selectedPos.getX()+1)
            &&
            (startingPos.getY() >= selectedPos.getY()-2 && startingPos.getY() <= selectedPos.getY()+2)
                )return true;
        else return false;
    }

    /**
     * \brief The method checkOuterHex.
     *
     * \details This method checks if selected pos is within 18 places around starting pos.
     *
     * @param[in] startingPos Position used as a reference for checking 18 tiles around it.
     * @param[in] selectedPos Position to be checked if it is in those 18 places startingPos.
     * @return returns true if selected pos is within those places otherwise returns false;
     */
    static bool checkOuterHex(Point startingPos, Point selectedPos){
        if((startingPos.getX() >= selectedPos.getX()-2 && startingPos.getX() <= selectedPos.getX()+2)
           &&
           (startingPos.getY() >= selectedPos.getY()-3 && startingPos.getY() <= selectedPos.getY()+3)
           ||
           (((startingPos.getY() == selectedPos.getY()+4)||(startingPos.getY() == selectedPos.getY()-4)) && startingPos.getX() == selectedPos.getX())
                ) return true;
        else return false;
    }

    /**
     * \brief The method seizeNearby.
     *
     * \details this method changes values around selectedPos to be the same value as parameter player.
     *
     * @param[in] selectedPos Position selected, around which all enemy tiles will be transformed to passed value.
     * @param[in] player value the seized tiles should be replaced with.
     */
    void seizeNearby(Point selectedPos, char player){
        int x = selectedPos.getX();
        int y = selectedPos.getY();
        if(getValueAtPos(x,y+2) == getOtherPlayer()) changeValueAtPos(x, y+2, player);
        if(getValueAtPos(x,y-2) == getOtherPlayer()) changeValueAtPos(x, y-2, player);
        if(getValueAtPos(x+1,y+1) == getOtherPlayer()) changeValueAtPos(x+1, y+1, player);
        if(getValueAtPos(x+1,y-1) == getOtherPlayer()) changeValueAtPos(x+1, y-1, player);
        if(getValueAtPos(x-1,y+1) == getOtherPlayer()) changeValueAtPos(x-1, y+1, player);
        if(getValueAtPos(x-1,y-1) == getOtherPlayer()) changeValueAtPos(x-1, y-1, player);
    }

    /**
     * \brief The method checkNearby.
     *
     * \details This method returns sum of how many enemy pawns are around passed selectedPos.
     *
     * @param[in] selectedPos Position around which the method checks how many enemy pawns are there.
     * @return return the sum of how many enemy pawns are around the selected tile;
     */
    int checkNearby(Point selectedPos){
        int sum = 0;
        int x = selectedPos.getX();
        int y = selectedPos.getY();
        if(getValueAtPos(x,y+2) == getOtherPlayer()) sum++;
        if(getValueAtPos(x,y-2) == getOtherPlayer()) sum++;
        if(getValueAtPos(x+1,y+1) == getOtherPlayer()) sum++;
        if(getValueAtPos(x+1,y-1) == getOtherPlayer()) sum++;
        if(getValueAtPos(x-1,y+1) == getOtherPlayer()) sum++;
        if(getValueAtPos(x-1,y-1) == getOtherPlayer()) sum++;
        return sum;
    }

    /**
     * \brief The method whoWon.
     *
     * \details Depending on score the method return either a winning player char or 3 to indicate draw.
     * In case of sudden exit (quitting the board), method returns char 0.
     *
     * @return The method returns char depending on player scores
     */
    [[nodiscard]] char whoWon() const{
        if (suddenExit) return '0';
        else if(p1Score > p2Score) return '1';
        else if(p1Score == p2Score) return '3';
        else return '2';
    }

    /**
     * \brief The method isWin.
     *
     * \details This method checks for all the win conditions such as no empty spaces left, no possible move for any of players,
     * or any player not having any more pawns, sets message to inform the player of who won and returns true if anybody wonm
     * else returns false.
     * If player won against AI this method also update the leaderboard with proper information about the winner.
     *
     * @return return true if any of player won or sudden exit is engaged else return false.
     */
    bool isWin() {

        //If current player can't move end game
        switch(currentPlayer){
            case '1':
                if (listOfMoves(p1Pawns).empty()) tilesLeft = 0;
                break;
            case '2':
                if (listOfMoves(p2Pawns).empty()) tilesLeft = 0;
                break;
        }
        //Else check for sudden exit
        if(suddenExit) return true;
        //Else normal win condition
        if(tilesLeft == 0 || p1Score == 0 || p2Score == 0) {
            if(whoWon() == '1'){
                errorMsg = BLUE "PLAYER 1 HAS WON";
            }else if (whoWon() == '3'){
                errorMsg = GREEN "DRAW";
            }else if (whoWon() == '2'){
                errorMsg = RED "PLAYER 2 HAS WON";
            }else{

            }
            drawBoard();
            if(mode == PVC && whoWon() == '1') {
                cout << "Please enter your name to save score (write space to not save): ";
                std::string playerName;
                std::getline(cin, playerName);
                if(playerName != " "){
                    updateLeaderboard(playerName, p1Score, difficulty);
                }
            }
            else{
                cout << "Press any key to continue...";
                getch();
            }

            return true;
        }
        else return false;
    }

    /**
     * \brief The method updateLeaderboard.
     *
     * \details This method reads the leaderboard, updates it with information passed as parameters, and writes it back to file.
     *
     * @param[in] pName Name of the player who won.
     * @param[in] score Score of the player who won.
     * @param[in] diff The difficulty, the player played on.
     */
    void updateLeaderboard(const std::string& pName, int score, DIFFICULTY diff) const{
        fs::path leaderboardPath = fs::current_path().append("Leaderboard.txt");
        std::string s;
        std::vector<LeaderboardPos> ranking;
        int index = 0;
        //First check if there is an existing leaderboard
        //If yes first load it
        if(!leaderboardPath.empty()) {
            std::ifstream stream(leaderboardPath);
            while (getline(stream, s)) {
                std::stringstream ss(s);
                std::string subString;
                int counter = 0;
                ranking.push_back(LeaderboardPos{});
                while(std::getline(ss, subString, ';')){
                    switch(counter){
                        case 0:
                            ranking.at(index).pName = subString;
                            break;
                        case 1:
                            ranking.at(index).score = std::stoi(subString);
                            break;
                        case 2:
                            ranking.at(index).diff = static_cast<DIFFICULTY>((int)(subString[0]-'0'));
                            break;
                        default:
                            break;
                    }
                    counter++;
                }
                index++;
            }
        }
        //Add score to arr and sort
        ranking.push_back(LeaderboardPos{pName, score, diff});
        std::sort(ranking.begin(), ranking.begin() + ranking.size(), [](const LeaderboardPos& a, const LeaderboardPos& b) {
            if(a.score > b.score) return true;
            else return false;
        });
        //Cut array to 5
        std::vector<LeaderboardPos> temp(ranking.begin(), ranking.begin() + ranking.size());
        ranking = std::move(temp);

        std::ofstream scoreFile(leaderboardPath);
        for (const LeaderboardPos& pos : ranking){
            if(debug) cout << pos.score << endl;
            scoreFile << pos.pName << ";" << pos.score << ";" << static_cast<int>(pos.diff) << "\n";
        }
    }

    void setGameMode(GAME_MODE gamemode){
        mode = gamemode;
    }

    bool canMove(int x, int y){
        if(getValueAtPos(x, y) != 'X') return true;
        else return false;
    }

    /**
     * \brief The method getValueAtPos.
     *
     * \details This method translates normal coordinates x, y to ones that fit the hexagonal grid,
     * and return value at that coordinates.
     *
     * @param[in] x Coordinate x of the position.
     * @param[in] y Coordinate y of the position.
     * @return Returns value of position x, y at a hexagonal grid.
     */
    char getValueAtPos(int x, int y){
        try {
            if(y%2==1) return boardState.at(y).at((x-1)/2);
            else return boardState.at(y).at(x/2);
        }catch (std::out_of_range &e){
            debugErrorMsg = "No such position";
            return 'X';
        }
    }

    [[nodiscard]] char getOtherPlayer() const{
        if(currentPlayer == '1')
            return '2';
        else
            return '1';
    }

    /**
     * \brief The method getValueAtPos.
     *
     * \details This method translates normal coordinates x, y to ones that fit the hexagonal grid,
     * and changes value at pos x, y to input.
     *
     * @param[in] x Coordinate x of the position.
     * @param[in] y Coordinate y of the position.
     * @param[in] input value used to change value at x, y coordinates.
     */
    void changeValueAtPos(int x, int y, char input){
        try {
            if(y%2==1) boardState.at(y).at((x-1)/2) = input;
            else boardState.at(y).at(x/2) = input;
        }catch (std::out_of_range &e){
            debugErrorMsg = "No such position";
        }
    }

    /**
     * \brief The method computerMove.
     *
     * \details This method depending on difficulty and positions of pawns calculates and returns a move, on easy difficulty a random move,
     * on medium a move that takes over the most enemy pawns.
     *
     * @param diff difficulty of the AI.
     * @param pawns list of all pawns, the ai can move;
     * @return returns a computer generated move.
     */
    Move computerMove(DIFFICULTY diff, const std::vector<Point>& pawns){
        //Random generator
        int rando;

        //Make a list of all possible moves
        std::vector<Move> possibleMoves = listOfMoves(pawns);
        if (possibleMoves.empty()) throw std::exception();
        //Random number form 0 to number of all possible moves
        std::uniform_int_distribution<int> dist(0, (int)possibleMoves.size()+1);

        //Debug mode cout list of all possible moves
        if(debug){
            for(Move move : possibleMoves){
                move.printMove();
            }
        }

        switch(diff) {
            case EASY:
                Sleep(1000);
                rando = std::rand()%possibleMoves.size();
                if (debug) cout << "Random Number: " << rando;

                //Return random move
                return possibleMoves[rando];
                //movePawn(possibleMoves[rando].pawnPos, possibleMoves[rando].moveTarget);

            case MEDIUM:
                Sleep(1500);
                //Find the best move from list of all possible moves
                int maxPoints = 0;
                int maxIndex;
                for(int i = 0; i < possibleMoves.size(); i++){
                    if(possibleMoves[i].points > maxPoints) {
                        maxPoints = possibleMoves[i].points;
                        maxIndex = i;
                    }
                }

                //Return best move
                return possibleMoves[maxIndex];
                //movePawn(possibleMoves[maxIndex].pawnPos, possibleMoves[maxIndex].moveTarget);
        }
    }

    /**
     * \brief The method listOfMove
     *
     * \details This method returns a vector of all possible moves for given list of pawns positions.
     *
     * @param pawns A vector of pawns positions.
     * @return Returns a vector of all possible moves.
     */
    std::vector<Move> listOfMoves(const std::vector<Point>& pawns){
        //Find every possible move
        std::vector<Move> possibleMoves;
        int currentX, currentY = 0, isLineEven = 0;
        int points;
        for (const std::vector<char> &line: boardState) {
            if (isLineEven % 2 == 1){
                cout << "\t";
                currentX = 1;
            }else currentX = 0;
            for (char c: line) {
                points = 0;
                if (c == 'O'){
                    //If empty space, check if any of pawns can move here, if they can, save move with points
                    for(Point pawn: pawns){
                        if(checkInnerHex(Point{currentX, currentY}, pawn)){
                            //If in inner hex add 1 point, and calculate how many paws will this move take over 2 points for each taken
                            points += 1;
                            points += 2*checkNearby(Point{currentX, currentY});
                            if(points > 0) {
                                possibleMoves.emplace_back(pawn, Point{currentX, currentY}, points);
                            }
                        }else if(checkOuterHex(Point{currentX, currentY}, pawn)){
                            //If in outer hex, calculate how many paws will this move take over 2 points for each taken
                            points += 2*checkNearby(Point{currentX, currentY});
                            if(points > 0) {
                                possibleMoves.emplace_back(pawn, Point{currentX, currentY}, points);
                            }
                        }
                    }
                }
                currentX += 2;
            }
            currentY++;
            isLineEven++;
        }
        return possibleMoves;
    }

    void setDifficulty(DIFFICULTY diff){
        difficulty = diff;
    }

};


#endif //HEXXAGON_BOARD_H
