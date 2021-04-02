#ifndef SUDOKU_H
#define SUDOKU_H

#include <string_view>
#include <string>

std::string solveSudoku(std::string_view puzzle);
const int kCells = 81;
extern const char kNoSolution[];

#endif  // MUDUO_EXAMPLES_SUDOKU_SUDOKU_H
