#include "Sudoku.hpp"
#include <iostream>
#include <algorithm>
#include <random>

const char SAME_LINE =  ' ';
const char END_LINE = '\n';
const int EMPTY = 0;


void Sudoku::loadBoard(const int newBoard[SIZE][SIZE]) {
    for (int row = 0; row < SIZE; row++) {
        for (int column = 0; column < SIZE; column++) {
            board[row][column] = newBoard[row][column];
        }
    }
}

bool Sudoku::existsInRow(const int row, const int number) const {
    for (int column = 0; column < SIZE; column++) {
        if (board[row][column] == number) {return true;}
    } return false;
}

bool Sudoku::existsInColumn(const int column, const int number) const {
    for (int row = 0; row < SIZE; row++) {
        if (board[row][column] == number) {return true;}
    } return false;
}

bool Sudoku::existsInBox(const int row, const int column, const int number) const {
    int startRow = (row / 3) * 3;
    int startColumn = (column / 3) * 3;
    for (int r = startRow; r < startRow+3; r++) {
        for (int c = startColumn; c < startColumn+3; c++) {
            if (board[r][c] == number) {return true;}
        }
    }
    return false;
}

bool Sudoku::isNumberAllowed(const int row, const int column, const int number) const {
    return !existsInRow(row, number) && !existsInColumn(column, number) && !existsInBox(row, column, number);
}

bool Sudoku::findEmptyCell(int& row, int& column) const { // changeable
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (board[r][c] == EMPTY) {row = r; column = c; return true;}
        }
    } return false;
}


void Sudoku::printBoard() const {
    for (int row = 0; row < SIZE; row++) {
        for (int column = 0; column < SIZE; column++) {
            std::cout << board[row][column] << SAME_LINE;
        } std::cout << END_LINE;
    }
}

void Sudoku::resetBoard() {
    for (int row = 0; row < SIZE; row++) {
        for (int column = 0; column < SIZE; column++) {
            board[row][column] = 0;
        }
    }
}

int Sudoku::getCell(const int row, const int column) const {
    return board[row][column];
}

void Sudoku::setCell(const int row, const int column, const int number) {
    board[row][column] = number;
}

bool Sudoku::hasDuplicateInRow(int row, int column, int number) const {
    for (int checkColumn = 0; checkColumn < SIZE; checkColumn++) {
        if (checkColumn == column) {continue;}
        if (board[row][checkColumn] == number) {return true;}
    } return false;
}
bool Sudoku::hasDuplicateInColumn(int row, int column, int number) const {
    for (int checkRow = 0; checkRow < SIZE; checkRow++) {
        if (checkRow == row) {continue;}
        if (board[checkRow][column] == number) {return true;}
    } return false;
}
bool Sudoku::hasDuplicateInBox(int row, int column, int number) const {
    int startRow = (row / 3) * 3;
    int startColumn = (column / 3) * 3;
    for (int checkRow = startRow; checkRow < startRow + 3; checkRow++) {
        for (int checkColumn = startColumn; checkColumn < startColumn + 3; checkColumn++) {
            if (checkRow == row && checkColumn == column) {continue;}
            if (board[checkRow][checkColumn] == number) {return true;}
        }
    } return false;
}

bool Sudoku::isBoardValid() const {
    for (int row = 0; row < SIZE; row++) {
        for (int column = 0; column < SIZE; column++) {
            if (board[row][column] == EMPTY) {continue;}
            const int number = board[row][column];
            if (hasDuplicateInRow(row, column, number) || hasDuplicateInColumn(row, column, number) || hasDuplicateInBox(row, column, number)) {return false;}
        }
    } return true;
}

bool Sudoku::solve() {
    if (!isBoardValid()) {return false;}
    return solveRecursive();
}

bool Sudoku::solveRecursive() {
    int row;
    int column;
    if (!findEmptyCell(row, column)) {return true;}
    for (int checkNumber = 1; checkNumber <= SIZE; checkNumber++) {
        if (isNumberAllowed(row, column, checkNumber)) {
            board[row][column] = checkNumber;
            if (solveRecursive()) {return true;}
            board[row][column] = EMPTY;
        }
    } return false;
}

bool Sudoku::generateRecursive() {
    int row;
    int column;
    if (!findEmptyCell(row, column)) {return true;}
    int numbers[SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    static std::mt19937 generator(std::random_device{}());
    std::shuffle(std::begin(numbers),std::end(numbers),generator);
    for (const int number : numbers) {
        if (isNumberAllowed(row, column, number)) {
            board[row][column] = number;
            if (generateRecursive()) {
                return true;
            } board[row][column] = EMPTY;
        }
    } return false;
}

bool Sudoku::generateSolvedBoard() {
    resetBoard();
    return generateRecursive();
}

bool Sudoku::generateGame(const std::string& difficulty) {
    if (!generateSolvedBoard()) {return false;}
    
    std::pair<int, int> selectedRange;
    if (difficulty == "easy") {selectedRange = easyRange;}
    else if (difficulty == "medium") {selectedRange = mediumRange;}
    else if (difficulty == "hard") {selectedRange = hardRange;}
    else {return false;}

    static std::mt19937 generator(std::random_device{}());
    std::uniform_int_distribution<int> cellsDistribution(selectedRange.first,selectedRange.second);
    std::uniform_int_distribution<int> position(0, SIZE - 1);
    const int cellsToRemove = cellsDistribution(generator);
    int removedCells = 0;
    while (removedCells < cellsToRemove) {
        const int row = position(generator);
        const int column = position(generator);
        if (board[row][column] == EMPTY) {continue;}
        board[row][column] = EMPTY;
        removedCells++;
    } return true;
}