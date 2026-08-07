#pragma once
#include <string>
#include <utility>

class Sudoku {
private:
    static constexpr int SIZE = 9;
    int board[SIZE][SIZE]{};
    const std::pair<int, int> easyRange = {30, 35};
    const std::pair<int, int> mediumRange = {40, 45};
    const std::pair<int, int> hardRange = {50, 55};


    bool existsInRow(int row, int number) const;
    bool existsInColumn(int column, int number) const;
    bool existsInBox(int row, int column, int number) const;
    bool isNumberAllowed(int row, int column, int number) const;
    bool findEmptyCell(int& row, int& column) const;
    bool hasDuplicateInRow(int row, int column, int number) const;
    bool hasDuplicateInColumn(int row, int column, int number) const;
    bool hasDuplicateInBox(int row, int column, int number) const;
    bool solveRecursive();
    bool generateRecursive();
    bool generateSolvedBoard();

public:
    void loadBoard(const int newBoard[SIZE][SIZE]);
    void printBoard() const;
    void resetBoard();
    int getCell(int row, int column) const;
    void setCell(int row, int column, int number);
    bool isBoardValid() const;
    bool solve();
    bool generateGame(const std::string& difficulty);
};