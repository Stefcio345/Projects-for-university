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

#ifndef HEXXAGON_MENU_H
#define HEXXAGON_MENU_H

/**
 * \class The class Menu.
 *
 * \details A class responsible for displaying a menu with choices big prompt and a small prompt.
 * It also allows for player to select one of the displayed choices with a cursor.
 */
class Menu{

    Pointer1D pointer{0};
    std::vector<std::string> choices;
    std::string prompt;
    std::string miniPrompt;

public:

    Menu(std::string bigPrompt, std::vector<std::string> &posChoices){
        prompt = std::move(bigPrompt);
        choices = posChoices;
    }

    Menu(std::string bigPrompt, std::vector<std::string> &posChoices, std::string smallPrompt){
        prompt = std::move(bigPrompt);
        choices = posChoices;
        miniPrompt = std::move(smallPrompt);

    }

    /**
     * \brief The method display
     *
     * \details This method is responsible for displaying big prompt,
     * small prompt and all the choices until the player chooses one of them,
     * Which numerical representation is then returned.
     *
     * @return An int representing choice selected by player.
     */
    int display(){
        //Reset pointer
        pointer.select(-1);
        pointer.currentPos = 0;
        //Start selection
        while(pointer.getSelectedPos() == -1) {
            system("cls");
            if (cLion) Sleep(70);
            cout<< prompt<<endl;
            displayChoices();
            choose();
        }
        return pointer.getSelectedPos();
    }

    void addChoice(const std::string& s){
        choices.push_back(s);
    }

    void emptyChoices(){
        choices.clear();
    }

    void changePrompt(std::string s){
        prompt = std::move(s);
    }

    void setMiniPrompt(std::string s){
        miniPrompt = std::move(s);
    }

private:
    /**
     * \brief The method displayChoices.
     *
     * \details This method displays all choices, centered.
     */
    void displayChoices(){
        int counter = 0;
        //Display mini prompt wyśrodkowany
        std::stringstream ss(miniPrompt);
        std::string to;

        while(std::getline(ss,to,'\n')){
            for(int i = 0; i < countAverageChars(prompt)-(to.length()/2); i++) cout<< " ";
            cout<< WHITE << to << RESET << endl;
        }

        //Display all choices
        for(const std::string& line : choices){
            for(int i = 0; i < countAverageChars(prompt)-(line.length()/2); i++) cout<< " ";
            if(pointer.getCurrPos() == counter) cout<< WHITEBG;
            cout<<line<<RESET<<endl;
            counter++;
        }
    }

    /**
     * \brief The method choose.
     *
     * \details Method responsible for changing the cursor position and selecting a choice.
     */
    void choose(){
        int input;
        if (!cLion) {
            input = getch();
            //If pressed arrow get the second value
            if (input == 224) input = getch();
        }else{
            input = getch();
            getch();
        }

        int currentX = pointer.getCurrPos();
        switch(input){
            //MOVE UP
            case 72:
            case 'w':
                moveUp();
                break;
                //MOVE DOWN
            case 80:
            case 's':
                moveDown();
                break;
                //SELECT
            case 32:
            case (char)13:
                pointer.select(currentX);
                break;
            default:
                break;
        }
    }

    void moveUp(){
        if(pointer.canMove(pointer.currentPos -1, choices)) pointer.setCurrPos(pointer.currentPos-1);//If can move up
        else {};//Stay in place
    }
    void moveDown(){
        if(pointer.canMove(pointer.currentPos+1, choices)) pointer.setCurrPos(pointer.currentPos+1);//If can move down
        else {};//Stay in place
    }

    int countAverageChars(std::string s){
        //Count average number of characters per line
        int newLines = 0;
        for(char ch : s){
            if(ch == '\n'){
                newLines++;
            }
        }
        return (int)(((double)s.length()/(double)newLines)/2);
    }
};


#endif //HEXXAGON_MENU_H
