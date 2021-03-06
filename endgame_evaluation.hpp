#pragma once
#include <iostream>
#include "board.hpp"

using namespace std;

#define step 100    // 1石が評価値のいくつに当たるか


int count_arr[n_line];              // count_arr[ボードのインデックス] = そのインデックスでの黒石数 - 白石数
int count_all_arr[n_line];          // count_all_arr[ボードのインデックス] = そのインデックスでの黒石数 + 白石数

// 石数の前計算
inline void endgame_evaluate_init() {
    int idx, place, b, w;
    for (idx = 0; idx < n_line; ++idx) {
        b = create_one_color(idx, 0);
        w = create_one_color(idx, 1);
        count_arr[idx] = 0;
        count_all_arr[idx] = 0;
        for (place = 0; place < hw; ++place) {
            count_arr[idx] += 1 & (b >> place);
            count_arr[idx] -= 1 & (w >> place);
            count_all_arr[idx] += 1 & (b >> place);
            count_all_arr[idx] += 1 & (w >> place);
        }
    }
}

inline int endgame_evaluate(const board b){
    int count = 0, n_vacant = hw2;
    for (int i = 0; i < hw; ++i) {
        count += count_arr[b.board_idx[i]];
        n_vacant -= count_all_arr[b.board_idx[i]];
    }
    if (b.player == 1)
        count = -count;
    if (count > 0)
        count += n_vacant;
    else if (count < 0)
        count -= n_vacant;
    return count * step;
}