#pragma once
#include <iostream>
#include "board.hpp"

using namespace std;

const int cell_weight[hw2] = {
    30, -12, 0, -1, -1, 0, -12, 30,
    -12, -15, -3, -3, -3, -3, -15, -12,
    0, -3, 0, -1, -1, 0, -3, 0,
    -1, -3, -1, -1, -1, -1, -3, -1,
    -1, -3, -1, -1, -1, -1, -3, -1,
    0, -3, 0, -1, -1, 0, -3, 0,
    -12, -15, -3, -3, -3, -3, -15, -12,
    30, -12, 0, -1, -1, 0, -12, 30
};

int cell_score[n_line];      // 盤面のインデックス(行のインデックス)において黒番目線のスコアを前計算しておく
int count_arr[n_line];              // count_arr[ボードのインデックス] = そのインデックスでの黒石数 - 白石数
int count_all_arr[n_line];          // count_all_arr[ボードのインデックス] = そのインデックスでの黒石数 + 白石数
int canput_arr[2][n_line];          // canput_arr[プレイヤー][ボードのインデックス] = そのインデックスでプレイヤーが着手可能な位置
int surround_arr[2][n_line];        // surround_arr[プレイヤー][ボードのインデックス] = そのインデックスでプレイヤーが空マスに接している数

inline void evaluate_init() {
    int idx, i, place, b, w;
    for (idx = 0; idx < n_line; ++idx) {
        b = create_one_color(idx, 0);
        w = create_one_color(idx, 1);
        count_arr[idx] = 0;
        count_all_arr[idx] = 0;
        reverse_board[idx] = 0;
        canput_arr[black][idx] = 0;
        canput_arr[white][idx] = 0;
        surround_arr[black][idx] = 0;
        surround_arr[white][idx] = 0;
        for (i = 0; i < hw / 2; ++i)
            cell_score[i][idx] = 0;
        for (place = 0; place < hw; ++place) {
            for (i = 0; i < hw / 2; ++i) {
                cell_score[i][idx] += (1 & (b >> place)) * cell_weight[i * hw + place];
                cell_score[i][idx] -= (1 & (w >> place)) * cell_weight[i * hw + place];
            }
            count_arr[idx] += 1 & (b >> place);
            count_arr[idx] -= 1 & (w >> place);
            count_all_arr[idx] += 1 & (b >> place);
            count_all_arr[idx] += 1 & (w >> place);
            reverse_board[idx] *= 3;
            if (place > 0) {
                if ((1 & (b >> (place - 1))) == 0 && (1 & (w >> (place - 1))) == 0) {
                    if (1 & (b >> place))
                        ++surround_arr[black][idx];
                    else if (1 & (w >> place))
                        ++surround_arr[white][idx];
                }
            }
            if (place < hw - 1) {
                if ((1 & (b >> (place + 1))) == 0 && (1 & (w >> (place + 1))) == 0) {
                    if (1 & (b >> place))
                        ++surround_arr[black][idx];
                    else if (1 & (w >> place))
                        ++surround_arr[white][idx];
                }
            }
        }
        for (place = 0; place < hw; ++place) {
            if (legal_arr[black][idx][place])
                ++canput_arr[black][idx];
            if (legal_arr[white][idx][place])
                ++canput_arr[white][idx];
        }
    }
}