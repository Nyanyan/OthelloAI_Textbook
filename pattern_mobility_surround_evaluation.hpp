#pragma once
#include <iostream>
#include <fstream>
#include "board.hpp"

using namespace std;

#define sc_w 6400 // 評価値の絶対値が取る最大値

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
const int pattern_sizes[n_patterns] = {8, 10, 10}; // パターンごとのマスの数
// モデルの設計パラメータ
#define n_dense0 16
#define n_dense1 16
#define n_add_input 3
#define n_add_dense0 8
#define n_all_input 4
#define max_mobility 30
#define max_surround 50
#define max_evaluate_idx 59049

int mobility_arr[2][n_line];    // mobility_arr[プレイヤー][ボードのインデックス] = そのインデックスでプレイヤーが着手可能な位置
int surround_arr[2][n_line];    // surround_arr[プレイヤー][ボードのインデックス] = そのインデックスでプレイヤーが空マスに接している数

// モデルの前計算した値と生のパラメータ
double pattern_arr[n_patterns][max_evaluate_idx];
double add_arr[max_mobility * 2 + 1][max_surround + 1][max_surround + 1];
double final_dense[n_all_input];
double final_bias;

// 着手可能数と囲み度合いの前計算
inline void evaluate_init1() {
    int idx, place, b, w;
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

// 活性化関数
inline double leaky_relu(double x){
    return max(0.01 * x, x);
}

// パターン評価の推論
inline double predict_pattern(int pattern_size, double in_arr[], double dense0[n_dense0][20], double bias0[n_dense0], double dense1[n_dense1][n_dense0], double bias1[n_dense1], double dense2[n_dense1], double bias2){
    double hidden0[16], hidden1;
    int i, j;
    for (i = 0; i < n_dense0; ++i){
        hidden0[i] = bias0[i];
        for (j = 0; j < pattern_size * 2; ++j)
            hidden0[i] += in_arr[j] * dense0[i][j];
        hidden0[i] = leaky_relu(hidden0[i]);
    }
    double res = bias2;
    for (i = 0; i < n_dense1; ++i){
        hidden1 = bias1[i];
        for (j = 0; j < n_dense0; ++j)
            hidden1 += hidden0[j] * dense1[i][j];
        hidden1 = leaky_relu(hidden1);
        res += hidden1 * dense2[i];
    }
    res = leaky_relu(res);
    return res;
}

// パターンの左右反転に使う
inline int calc_pop(int a, int b, int s){
    return (a / pow3[s - 1 - b]) % 3;
}

// パターンの左右反転
inline int calc_rev_idx(int pattern_idx, int pattern_size, int idx){
    int res = 0;
    if (pattern_idx <= 1){
        for (int i = 0; i < pattern_size; ++i)
            res += pow3[i] * calc_pop(idx, i, pattern_size);
    } else if (pattern_idx == 2){
        res += p39 * calc_pop(idx, 0, pattern_size);
        res += p38 * calc_pop(idx, 4, pattern_size);
        res += p37 * calc_pop(idx, 7, pattern_size);
        res += p36 * calc_pop(idx, 9, pattern_size);
        res += p35 * calc_pop(idx, 1, pattern_size);
        res += p34 * calc_pop(idx, 5, pattern_size);
        res += p33 * calc_pop(idx, 8, pattern_size);
        res += p32 * calc_pop(idx, 2, pattern_size);
        res += p31 * calc_pop(idx, 6, pattern_size);
        res += calc_pop(idx, 3, pattern_size);
    }
    return res;
}

// パターン評価の前計算
inline void pre_evaluation_pattern(int pattern_idx, int evaluate_idx, int pattern_size, double dense0[n_dense0][20], double bias0[n_dense0], double dense1[n_dense1][n_dense0], double bias1[n_dense1], double dense2[n_dense1], double bias2){
    int digit, idx, i;
    double arr[20], tmp_pattern_arr[max_evaluate_idx];
    for (idx = 0; idx < pow3[pattern_size]; ++idx){
        for (i = 0; i < pattern_size; ++i){
            digit = (idx / pow3[pattern_size - 1 - i]) % 3;
            if (digit == 0){
                arr[i] = 1.0;
                arr[pattern_size + i] = 0.0;
            } else if (digit == 1){
                arr[i] = 0.0;
                arr[pattern_size + i] = 1.0;
            } else{
                arr[i] = 0.0;
                arr[pattern_size + i] = 0.0;
            }
        }
        pattern_arr[evaluate_idx][idx] = predict_pattern(pattern_size, arr, dense0, bias0, dense1, bias1, dense2, bias2);
        tmp_pattern_arr[calc_rev_idx(pattern_idx, pattern_size, idx)] = pattern_arr[evaluate_idx][idx];
    }
    for (idx = 0; idx < pow3[pattern_size]; ++idx)
        pattern_arr[evaluate_idx][idx] += tmp_pattern_arr[idx];
}

// 追加パラメータの推論
inline double predict_add(int mobility, int sur0, int sur1, double dense0[n_add_dense0][n_add_input], double bias0[n_add_dense0], double dense1[n_add_dense0], double bias1){
    double hidden0[n_add_dense0], in_arr[n_add_input];
    int i, j;
    in_arr[0] = (double)mobility / 30.0;
    in_arr[1] = ((double)sur0 - 15.0) / 15.0;
    in_arr[2] = ((double)sur1 - 15.0) / 15.0;
    for (i = 0; i < n_add_dense0; ++i){
        hidden0[i] = bias0[i];
        for (j = 0; j < n_add_input; ++j)
            hidden0[i] += in_arr[j] * dense0[i][j];
        hidden0[i] = leaky_relu(hidden0[i]);
    }
    double res = bias1;
    for (j = 0; j < n_add_dense0; ++j)
        res += hidden0[j] * dense1[j];
    res = leaky_relu(res);
    return res;
}

// 追加パラメータの前計算
inline void pre_evaluation_add(double dense0[n_add_dense0][n_add_input], double bias0[n_add_dense0], double dense1[n_add_dense0], double bias1){
    int mobility, sur0, sur1;
    for (mobility = -max_mobility; mobility <= max_mobility; ++mobility){
        for (sur0 = 0; sur0 <= max_surround; ++sur0){
            for (sur1 = 0; sur1 <= max_surround; ++sur1)
                add_arr[mobility + max_mobility][sur0][sur1] = predict_add(mobility, sur0, sur1, dense0, bias0, dense1, bias1);
        }
    }
}

// 機械学習したモデルの読み込み
inline void evaluate_init2() {
    ifstream ifs("evaluation/models/model.txt");
    if (ifs.fail()){
        cerr << "evaluation file not exist" << endl;
        exit(1);
    }
    string line;
    int i, j, pattern_idx;

    // モデルのパラメータを格納する
    double dense0[n_dense0][20];
    double bias0[n_dense0];
    double dense1[n_dense1][n_dense0];
    double bias1[n_dense1];
    double dense2[n_dense1];
    double bias2;
    double add_dense0[n_add_dense0][n_add_input];
    double add_bias0[n_add_dense0];
    double add_dense1[n_add_dense0];
    double add_bias1;

    // パターンのパラメータを得て前計算をする
    for (pattern_idx = 0; pattern_idx < n_patterns; ++pattern_idx){
        for (i = 0; i < n_dense0; ++i){
            for (j = 0; j < pattern_sizes[pattern_idx] * 2; ++j){
                getline(ifs, line);
                dense0[i][j] = stof(line);
            }
        }
        for (i = 0; i < n_dense0; ++i){
            getline(ifs, line);
            bias0[i] = stof(line);
        }
        for (i = 0; i < n_dense1; ++i){
            for (j = 0; j < n_dense0; ++j){
                getline(ifs, line);
                dense1[i][j] = stof(line);
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
        pre_evaluation_pattern(pattern_idx, pattern_idx, pattern_sizes[pattern_idx], dense0, bias0, dense1, bias1, dense2, bias2);
    }

    // 追加入力のパラメータを得て前計算をする
    for (i = 0; i < n_add_dense0; ++i){
        for (j = 0; j < n_add_input; ++j){
            getline(ifs, line);
            add_dense0[i][j] = stof(line);
        }
    }
    for (i = 0; i < n_add_dense0; ++i){
        getline(ifs, line);
        add_bias0[i] = stof(line);
    }
    for (i = 0; i < n_add_dense0; ++i){
        getline(ifs, line);
        add_dense1[i] = stof(line);
    }
    getline(ifs, line);
    add_bias1 = stof(line);
    pre_evaluation_add(add_dense0, add_bias0, add_dense1, add_bias1);

    // 最後の層のパラメータを得る
    for (i = 0; i < n_all_input; ++i){
        getline(ifs, line);
        final_dense[i] = stof(line);
    }
    getline(ifs, line);
    final_bias = stof(line);
}

// 初期化
inline void evaluate_init() {
    evaluate_init1();
    evaluate_init2();
}

// 着手可能数 黒の手番なら正の値、白の手番では負の値にする 世界1位AIの手動for展開の名残があります
inline int calc_mobility(const board b){
    return (b.player ? -1 : 1) * (
        mobility_arr[b.player][b.board_idx[0]] + mobility_arr[b.player][b.board_idx[1]] + mobility_arr[b.player][b.board_idx[2]] + mobility_arr[b.player][b.board_idx[3]] + 
        mobility_arr[b.player][b.board_idx[4]] + mobility_arr[b.player][b.board_idx[5]] + mobility_arr[b.player][b.board_idx[6]] + mobility_arr[b.player][b.board_idx[7]] + 
        mobility_arr[b.player][b.board_idx[8]] + mobility_arr[b.player][b.board_idx[9]] + mobility_arr[b.player][b.board_idx[10]] + mobility_arr[b.player][b.board_idx[11]] + 
        mobility_arr[b.player][b.board_idx[12]] + mobility_arr[b.player][b.board_idx[13]] + mobility_arr[b.player][b.board_idx[14]] + mobility_arr[b.player][b.board_idx[15]] + 
        mobility_arr[b.player][b.board_idx[16] - p35 + 1] + mobility_arr[b.player][b.board_idx[26] - p35 + 1] + mobility_arr[b.player][b.board_idx[27] - p35 + 1] + mobility_arr[b.player][b.board_idx[37] - p35 + 1] + 
        mobility_arr[b.player][b.board_idx[17] - p34 + 1] + mobility_arr[b.player][b.board_idx[25] - p34 + 1] + mobility_arr[b.player][b.board_idx[28] - p34 + 1] + mobility_arr[b.player][b.board_idx[36] - p34 + 1] + 
        mobility_arr[b.player][b.board_idx[18] - p33 + 1] + mobility_arr[b.player][b.board_idx[24] - p33 + 1] + mobility_arr[b.player][b.board_idx[29] - p33 + 1] + mobility_arr[b.player][b.board_idx[35] - p33 + 1] + 
        mobility_arr[b.player][b.board_idx[19] - p32 + 1] + mobility_arr[b.player][b.board_idx[23] - p32 + 1] + mobility_arr[b.player][b.board_idx[30] - p32 + 1] + mobility_arr[b.player][b.board_idx[34] - p32 + 1] + 
        mobility_arr[b.player][b.board_idx[20] - p31 + 1] + mobility_arr[b.player][b.board_idx[22] - p31 + 1] + mobility_arr[b.player][b.board_idx[31] - p31 + 1] + mobility_arr[b.player][b.board_idx[33] - p31 + 1] + 
        mobility_arr[b.player][b.board_idx[21]] + mobility_arr[b.player][b.board_idx[32]]);
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

inline double edge_2x(const int b[], int x, int y){
    return pattern_arr[1][pop_digit[b[x]][1] * p39 + b[y] * p31 + pop_digit[b[x]][6]];
}

inline double triangle0(const int b[], int w, int x, int y, int z){
    return pattern_arr[2][b[w] / p34 * p36 + b[x] / p35 * p33 + b[y] / p36 * p31 + b[z] / p37];
}

inline double triangle1(const int b[], int w, int x, int y, int z){
    return pattern_arr[2][reverse_board[b[w]] / p34 * p36 + reverse_board[b[x]] / p35 * p33 + reverse_board[b[y]] / p36 * p31 + reverse_board[b[z]] / p37];
}

// パターン評価部分
inline double calc_pattern(const board b){
    return
        final_dense[0] * (pattern_arr[0][b.board_idx[21]] + pattern_arr[0][b.board_idx[32]]) + 
        final_dense[1] * (edge_2x(b.board_idx, 1, 0) + edge_2x(b.board_idx, 6, 7) + edge_2x(b.board_idx, 9, 8) + edge_2x(b.board_idx, 14, 15)) + 
        final_dense[2] * (triangle0(b.board_idx, 0, 1, 2, 3) + triangle0(b.board_idx, 7, 6, 5, 4) + triangle0(b.board_idx, 15, 14, 13, 12) + triangle1(b.board_idx, 15, 14, 13, 12));
}

// 評価関数本体
inline int evaluate(const board b){
    int mobility, sur0, sur1;
    mobility = min(max_mobility * 2, max(0, max_mobility + calc_mobility(b)));
    sur0 = min(max_surround, calc_surround(b, 0));
    sur1 = min(max_surround, calc_surround(b, 1));
    double res = (b.player ? -1.0 : 1.0) * (final_bias + calc_pattern(b) + final_dense[3] * add_arr[mobility][sur0][sur1]);
    return (int)(max(-1.0, min(1.0, res)) * sc_w);
}
