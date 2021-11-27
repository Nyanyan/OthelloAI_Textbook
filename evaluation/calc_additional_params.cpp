#include <iostream>
#include "board.hpp"
#include "mobility_surround_evaluation.hpp"

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

int main() {
    init();
    int arr[64];
    board b;
    int ai_player;
    while (true) {
        cin >> ai_player;
        input_board(arr);
        b.translate_from_arr(arr, ai_player);
        cout << calc_canput(b) << " " << calc_surround(b, black) << " " << calc_surround(b, white) << endl;
    }
    return 0;
}