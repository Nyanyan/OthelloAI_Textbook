#include <iostream>
#include "board.hpp"
#include "cell_evaluation.hpp"

using namespace std;

inline void init(){
    board_init();
}

int main(){
    init();
    int arr[64] = {
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 1, 0, 2, 2, 2,
        2, 2, 2, 0, 1, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2,
        2, 2, 2, 2, 2, 2, 2, 2
    };
    board b;
    b.translate_from_arr(arr, 1);
    cerr << evaluate(b) << endl;
    return 0;
}