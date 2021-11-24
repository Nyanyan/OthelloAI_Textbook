#pragma once
#include <iostream>
#include "board.hpp"

using namespace std;

const int cell_weight[hw2] = {
    120, -20, 20, 5, 5, 20, -20, 120,
    -20, -40, -5, -5, -5, -5, -40, -20,
    20, -5, 15, 3, 3, 15, -5, 20,
    5, -5, 3, 3, 3, 3, -5, 5,
    5, -5, 3, 3, 3, 3, -5, 5,
    20, -5, 15, 3, 3, 15, -5, 20,
    -20, -40, -5, -5, -5, -5, -40, -20,
    120, -20, 20, 5, 5, 20, -20, 120
};

int evaluate(board *b) {
    int board_arr[hw2];
    int res = 0, i, j;
    b->translate_to_arr(board_arr);
    for (i = 0; i < hw; ++i) {
        for (j = 0; j < hw; ++j) {
            if (board_arr[i * hw + j] == b->player)
                res += cell_weight[i * hw + j];
            else if (board_arr[i * hw + j] == 1 - b->player)
                res -= cell_weight[i * hw + j];
        }
    }
    return res;
}

int evaluate(board b) {
    return evaluate(&b);
}