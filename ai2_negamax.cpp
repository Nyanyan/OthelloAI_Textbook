#include <iostream>
#include "board.hpp"
#include "cell_evaluation.hpp"

using namespace std;

#define inf 100000000 // 大きな値

// 初期化
inline void init() {
    board_init();
    evaluate_init();
}

// 標準入力からボードの状態を配列に受け取る
inline void input_board(int arr[]) {
    char elem;
    for (int i = 0; i < hw2; ++i) {
        cin >> elem;
        if (elem == '0')
            arr[i] = black;
        else if (elem == '1')
            arr[i] = white;
        else
            arr[i] = vacant;
    }
}

// negamax法
int nega_max(board b, int depth, bool passed) {
    // 葉ノードでは評価関数を実行する
    if (depth == 0)
        return evaluate(b);
    
    // 葉ノードでなければ子ノード全部に対して再帰する
    int coord, max_score = -inf;
    for (coord = 0; coord < hw2; ++coord) {
        if (b.legal(coord))
            max_score = max(max_score, -nega_max(b.move(coord), depth - 1, false));
    }
    
    // パスの処理 手番を交代して同じ深さで再帰する
    if (max_score == -inf) {
        // 2回連続パスなら評価関数を実行
        if (passed)
            return evaluate(b);
        b.player = 1 - b.player;
        return -nega_max(b, depth, true);
    }
    return max_score;
}

// depth手読みの探索
int search(board b, int depth) {
    int coord, max_score = -inf, res = -1, score;
    for (coord = 0; coord < hw2; ++coord) {
        if (b.legal(coord)) {
            score = -nega_max(b.move(coord), depth - 1, false);
            if (max_score < score) {
                max_score = score;
                res = coord;
            }
        }
    }
    return res;
}

int main() {
    init();
    int arr[64];
    board b;
    int ai_player, policy;
    cin >> ai_player;
    while (true) {
        input_board(arr);
        b.translate_from_arr(arr, ai_player);
        cerr << evaluate(b) << endl;
        b.print();
        policy = search(b, 4);
        cout << policy / hw << " " << policy % hw << endl;
    }
    return 0;
}