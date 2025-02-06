#pragma once
#include <iostream>
#include "board.hpp"

using namespace std;

#define sc_w 64 // 評価値の絶対値が取る最大値

// 評価値が最終石差の256倍になるように調整した
const int cell_weight[hw2] = {
    2714,  147,   69,  -18,  -18,   69,  147, 2714,
     147, -577, -186, -153, -153, -186, -577,  147,
      69, -186, -379, -122, -122, -379, -186,   69,
     -18, -153, -122, -169, -169, -122, -153,  -18,
     -18, -153, -122, -169, -169, -122, -153,  -18,
      69, -186, -379, -122, -122, -379, -186,   69,
     147, -577, -186, -153, -153, -186, -577,  147,
    2714,  147,   69,  -18,  -18,   69,  147, 2714
};

int cell_score[hw / 2][n_line];      // 盤面のインデックス(行のインデックス)において黒番目線のスコアを前計算しておく

inline void evaluate_init() {
    int idx, i, place, b, w;
    for (idx = 0; idx < n_line; ++idx) {
        b = create_one_color(idx, 0);
        w = create_one_color(idx, 1);
        for (i = 0; i < hw / 2; ++i)
            cell_score[i][idx] = 0;
        for (place = 0; place < hw; ++place) {
            for (i = 0; i < hw / 2; ++i) {
                cell_score[i][idx] += (1 & (b >> place)) * cell_weight[i * hw + place];
                cell_score[i][idx] -= (1 & (w >> place)) * cell_weight[i * hw + place];
            }
        }
    }
}

inline int evaluate(board b) {
    int res = 0, i;
    for (i = 0; i < hw / 2; ++i)
        res += cell_score[i][b.board_idx[i]];
    for (i = 0; i < hw / 2; ++i)
        res += cell_score[hw / 2 - 1 - i][b.board_idx[hw / 2 + i]];
    if (b.player == white)
        res = -res;
    if (res > 0) // 四捨五入のため
        res += 128;
    else
        res -= 128;
    res /= 256; // 生の評価値は最終石差の256倍なので、256で割る
    if (res > sc_w) // 評価値を[-64, 64] に収める
        res = sc_w;
    else if (res < -sc_w)
        res = -sc_w;
    return res;
}
