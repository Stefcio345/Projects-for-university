#include <iostream>
#include "Game.h"

#define WHITEBG "\033[1m\033[47m"
#define RESET   "\033[0m"

using std::cout, std::endl, std::cin;
namespace fs=std::filesystem;

int main(){
   Game game1;
   game1.startGame();
}