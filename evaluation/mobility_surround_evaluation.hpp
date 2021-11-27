#pragma once
#include <iostream>
#include "board.hpp"

using namespace std;

// 3の累乗
#define p31 3
#define p32 9
#define p33 27
#define p34 81
#define p35 243
#define p36 729
#define p37 2187
#define p38 6561
#define p39 19683
#define p310 59049

int mobility_arr[2][n_line];          // mobility_arr[プレイヤー][ボードのインデックス] = そのインデックスでプレイヤーが着手可能な位置
int surround_arr[2][n_line];        // surround_arr[プレイヤー][ボードのインデックス] = そのインデックスでプレイヤーが空マスに接している数

inline void evaluate_init() {
    int idx, i, place, b, w;
    for (idx = 0; idx < n_line; ++idx) {
        b = create_one_color(idx, 0);
        w = create_one_color(idx, 1);
        mobility_arr[black][idx] = 0;
        mobility_arr[white][idx] = 0;
        surround_arr[black][idx] = 0;
        surround_arr[white][idx] = 0;
        for (place = 0; place < hw; ++place) {
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
                ++mobility_arr[black][idx];
            if (legal_arr[white][idx][place])
                ++mobility_arr[white][idx];
        }
    }
}

// 着手可能数 黒の手番なら正の値、白の手番では負の値にする 世界1位AIの手動for展開の名残があります
inline int calc_canput(const board b){
    return (b.player ? -1 : 1) * 
        mobility_arr[b.player][b.board_idx[0]] + mobility_arr[b.player][b.board_idx[1]] + mobility_arr[b.player][b.board_idx[2]] + mobility_arr[b.player][b.board_idx[3]] + 
        mobility_arr[b.player][b.board_idx[4]] + mobility_arr[b.player][b.board_idx[5]] + mobility_arr[b.player][b.board_idx[6]] + mobility_arr[b.player][b.board_idx[7]] + 
        mobility_arr[b.player][b.board_idx[8]] + mobility_arr[b.player][b.board_idx[9]] + mobility_arr[b.player][b.board_idx[10]] + mobility_arr[b.player][b.board_idx[11]] + 
        mobility_arr[b.player][b.board_idx[12]] + mobility_arr[b.player][b.board_idx[13]] + mobility_arr[b.player][b.board_idx[14]] + mobility_arr[b.player][b.board_idx[15]] + 
        mobility_arr[b.player][b.board_idx[16] - p35 + 1] + mobility_arr[b.player][b.board_idx[26] - p35 + 1] + mobility_arr[b.player][b.board_idx[27] - p35 + 1] + mobility_arr[b.player][b.board_idx[37] - p35 + 1] + 
        mobility_arr[b.player][b.board_idx[17] - p34 + 1] + mobility_arr[b.player][b.board_idx[25] - p34 + 1] + mobility_arr[b.player][b.board_idx[28] - p34 + 1] + mobility_arr[b.player][b.board_idx[36] - p34 + 1] + 
        mobility_arr[b.player][b.board_idx[18] - p33 + 1] + mobility_arr[b.player][b.board_idx[24] - p33 + 1] + mobility_arr[b.player][b.board_idx[29] - p33 + 1] + mobility_arr[b.player][b.board_idx[35] - p33 + 1] + 
        mobility_arr[b.player][b.board_idx[19] - p32 + 1] + mobility_arr[b.player][b.board_idx[23] - p32 + 1] + mobility_arr[b.player][b.board_idx[30] - p32 + 1] + mobility_arr[b.player][b.board_idx[34] - p32 + 1] + 
        mobility_arr[b.player][b.board_idx[20] - p31 + 1] + mobility_arr[b.player][b.board_idx[22] - p31 + 1] + mobility_arr[b.player][b.board_idx[31] - p31 + 1] + mobility_arr[b.player][b.board_idx[33] - p31 + 1] + 
        mobility_arr[b.player][b.board_idx[21]] + mobility_arr[b.player][b.board_idx[32]];
}

// 囲み具合
inline int sfill5(int b){
    return pop_digit[b][2] != 2 ? b - p35 + 1 : b;
}

inline int sfill4(int b){
    return pop_digit[b][3] != 2 ? b - p34 + 1 : b;
}

inline int sfill3(int b){
    return pop_digit[b][4] != 2 ? b - p33 + 1 : b;
}

inline int sfill2(int b){
    return pop_digit[b][5] != 2 ? b - p32 + 1 : b;
}

inline int sfill1(int b){
    return pop_digit[b][6] != 2 ? b - p31 + 1 : b;
}

// 囲み度合い 世界1位AIの手動for展開の名残があります
inline int calc_surround(const board b, int p){
    return surround_arr[p][b.board_idx[0]] + surround_arr[p][b.board_idx[1]] + surround_arr[p][b.board_idx[2]] + surround_arr[p][b.board_idx[3]] + 
        surround_arr[p][b.board_idx[4]] + surround_arr[p][b.board_idx[5]] + surround_arr[p][b.board_idx[6]] + surround_arr[p][b.board_idx[7]] + 
        surround_arr[p][b.board_idx[8]] + surround_arr[p][b.board_idx[9]] + surround_arr[p][b.board_idx[10]] + surround_arr[p][b.board_idx[11]] + 
        surround_arr[p][b.board_idx[12]] + surround_arr[p][b.board_idx[13]] + surround_arr[p][b.board_idx[14]] + surround_arr[p][b.board_idx[15]] + 
        surround_arr[p][sfill5(b.board_idx[16])] + surround_arr[p][sfill5(b.board_idx[26])] + surround_arr[p][sfill5(b.board_idx[27])] + surround_arr[p][sfill5(b.board_idx[37])] + 
        surround_arr[p][sfill4(b.board_idx[17])] + surround_arr[p][sfill4(b.board_idx[25])] + surround_arr[p][sfill4(b.board_idx[28])] + surround_arr[p][sfill4(b.board_idx[36])] + 
        surround_arr[p][sfill3(b.board_idx[18])] + surround_arr[p][sfill3(b.board_idx[24])] + surround_arr[p][sfill3(b.board_idx[29])] + surround_arr[p][sfill3(b.board_idx[35])] + 
        surround_arr[p][sfill2(b.board_idx[19])] + surround_arr[p][sfill2(b.board_idx[23])] + surround_arr[p][sfill2(b.board_idx[30])] + surround_arr[p][sfill2(b.board_idx[34])] + 
        surround_arr[p][sfill1(b.board_idx[20])] + surround_arr[p][sfill1(b.board_idx[22])] + surround_arr[p][sfill1(b.board_idx[31])] + surround_arr[p][sfill1(b.board_idx[33])] + 
        surround_arr[p][b.board_idx[21]] + surround_arr[p][b.board_idx[32]];
}

