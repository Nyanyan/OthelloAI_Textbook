#include <iostream>
#include "board.hpp"
#include "cell_evaluation.hpp"

using namespace std;

#define inf 100000000 // 大きな値

// 初期化
inline void init() {
    board_init();
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

// 1手読みの探索
int search(board b) {
    int coord, max_score = -inf, res = -1, score;
    for (coord = 0; coord < hw2; ++coord) {
        if (b.legal(coord)) {
            score = -evaluate(b.move(coord));
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
        policy = search(b);
        cout << policy / hw << " " << policy % hw << endl;
    }
    return 0;
}