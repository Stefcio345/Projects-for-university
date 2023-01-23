//
// Created by Franciszek on 13/01/2023.
//
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>
#include <windows.h>
#include <algorithm>
#include <conio.h>
#include <random>

#ifndef HEXXAGON_POINTERS_H
#define HEXXAGON_POINTERS_H

/**
 * \struct The struct point
 *
 * \details A struct holding information about its coordinates, that can be accessed and changed via methods.
 */
struct Point {
    int X;
    int Y;

public:

    void setPos(int x, int y){
        X = x;
        Y = y;
    }

    [[nodiscard]] int getX() const{
        return X;
    }

    [[nodiscard]] int getY() const{
        return Y;
    }
};

/**
 * \struct The struct Pointer1D
 *
 * \details It is a struct that hold information about its current 1 dimensional position and a selected 1 dimensional position, which can be changed and accessed via methods.
 */
struct Pointer1D{
    int currentPos;
    int selectedPos = -1;

public:

    explicit Pointer1D(int x) {
        currentPos = x;
    }

    template <typename K>
    bool canMove(int x, std::vector<K> vec){
        try {
            vec.at(x);
            return true;
        }catch (std::out_of_range &e){
            return false;
        }
    }

    void select(int x){
        selectedPos = x;
    }

    [[nodiscard]] int getCurrPos() const{
        return currentPos;
    }

    [[nodiscard]] int getSelectedPos() const{
        return selectedPos;
    }

    void setCurrPos(int x){
        currentPos = x;
    }

};

/**
 * \struct The struct Pointer
 *
 * \details It is a struct that hold information about its current 2 dimensional position and a selected 2 dimensional position, which can be changed and accessed via methods.
 */
struct Pointer {

    Point currentPos{};
    Point selectedPos{};

public:

    Pointer(int x, int y) {
        currentPos.setPos(x, y);
    }

    Point& getCurrPos(){
        return currentPos;
    }

    Point& getSelectedPos(){
        return selectedPos;
    }
};


#endif //HEXXAGON_POINTERS_H
