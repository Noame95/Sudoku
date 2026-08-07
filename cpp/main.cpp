#include <iostream>
#include <string>
#include "Sudoku.hpp"

int main() {
    Sudoku game;
    std::string command;
    if (!(std::cin >> command)) {
        std::cout << "INVALID_INPUT\n";
        return 1;
    } if (command == "solve") {
        int puzzle[9][9] = {};
        for (int row = 0; row < 9; row++) {
            for (int column = 0; column < 9; column++) {
                if (!(std::cin >> puzzle[row][column])) {
                    std::cout << "INVALID_INPUT\n";
                    return 1;
                }
            }
        } game.loadBoard(puzzle);
        if (!game.solve()) {
            std::cout << "NO_SOLUTION\n";
            return 1;
        } game.printBoard();
        return 0;
    } if (command == "generate") {
        std::string difficulty;
        if (!(std::cin >> difficulty)) {
            std::cout << "INVALID_DIFFICULTY\n";
            return 1;
        } if (!game.generateGame(difficulty)) {
            std::cout << "GENERATION_FAILED\n";
            return 1;
        } game.printBoard();
        return 0;
    } std::cout << "INVALID_COMMAND\n";
    return 1;
}