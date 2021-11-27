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

#define n_patterns 3 // 使うパターンの種類

int count_arr[n_line];              // count_arr[ボードのインデックス] = そのインデックスでの黒石数 - 白石数
int count_all_arr[n_line];          // count_all_arr[ボードのインデックス] = そのインデックスでの黒石数 + 白石数
int mobility_arr[2][n_line];          // mobility_arr[プレイヤー][ボードのインデックス] = そのインデックスでプレイヤーが着手可能な位置
int surround_arr[2][n_line];        // surround_arr[プレイヤー][ボードのインデックス] = そのインデックスでプレイヤーが空マスに接している数

// 着手可能数と囲み度合いの前計算
inline void evaluate_init1() {
    int idx, i, place, b, w;
    for (idx = 0; idx < n_line; ++idx) {
        b = create_one_color(idx, 0);
        w = create_one_color(idx, 1);
        count_arr[idx] = 0;
        count_all_arr[idx] = 0;
        reverse_board[idx] = 0;
        mobility_arr[black][idx] = 0;
        mobility_arr[white][idx] = 0;
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
                ++mobility_arr[black][idx];
            if (legal_arr[white][idx][place])
                ++mobility_arr[white][idx];
        }
    }
}

inline void evaluate_init2() {
    ifstream ifs("evaluation/models/mpodel.txt");
    if (ifs.fail()){
        cerr << "evaluation file not exist" << endl;
        exit(1);
    }
    string line;
    for (pattern_idx = 0; pattern_idx < n_patterns; ++pattern_idx){
        for (i = 0; i < pattern_sizes[pattern_idx] * 2; ++i){
            for (j = 0; j < n_dense0; ++j){
                getline(ifs, line);
                dense0[j][i] = stof(line);
            }
        }
        for (i = 0; i < n_dense0; ++i){
            getline(ifs, line);
            bias0[i] = stof(line);
        }
        for (i = 0; i < n_dense0; ++i){
            for (j = 0; j < n_dense1; ++j){
                getline(ifs, line);
                dense1[j][i] = stof(line);
            }
        }
        for (i = 0; i < n_dense1; ++i){
            getline(ifs, line);
            bias1[i] = stof(line);
        }
        for (i = 0; i < n_dense1; ++i){
            getline(ifs, line);
            dense2[i] = stof(line);
        }
        getline(ifs, line);
        bias2 = stof(line);
        pre_evaluation(pattern_idx, phase_idx, pattern_idx, pattern_sizes[pattern_idx], dense0, bias0, dense1, bias1, dense2, bias2);
    }
    for (i = 0; i < n_add_input; ++i){
        for (j = 0; j < n_add_dense0; ++j){
            getline(ifs, line);
            add_dense0[j][i] = stof(line);
        }
    }
    for (i = 0; i < n_add_dense0; ++i){
        getline(ifs, line);
        add_bias0[i] = stof(line);
    }
    for (i = 0; i < n_add_dense0; ++i){
        for (j = 0; j < n_add_dense1; ++j){
            getline(ifs, line);
            add_dense1[j][i] = stof(line);
        }
    }
    for (i = 0; i < n_add_dense1; ++i){
        getline(ifs, line);
        add_bias1[i] = stof(line);
    }
    pre_evaluation_add(phase_idx, add_dense0, add_bias0, add_dense1, add_bias1);
    for (i = 0; i < n_all_input; ++i){
        getline(ifs, line);
        all_dense[phase_idx][i] = stof(line);
    }
    getline(ifs, line);
    all_bias[phase_idx] = stof(line);
}

// 初期化
inline void evaluate_init() {
    evaluate_init1();
    evaluate_init2();
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

