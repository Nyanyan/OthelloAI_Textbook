#pragma once
#include <iostream>
#include "board.hpp"

using namespace std;

#define hw 8            // ボードの大きさ
#define hw2 64          // ボードのマス数
#define n_board_idx 38  // インデックスの個数 縦横各8x2、斜め11x2
#define n_line 6561     // ボードの1つのインデックスが取りうる値の種類。3^8

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