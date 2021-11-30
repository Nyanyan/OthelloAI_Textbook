#pragma once
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "board.hpp"

using namespace std;

unordered_map<board, int, board::hash> book;

inline void book_init() {
    ifstream ifs("book/book.txt");
    if (ifs.fail()){
        cerr << "book file not exist" << endl;
        exit(1);
    }
    string line;
    board b;
    int i, y, x;
    int n = 0;
    const int first_board[n_board_idx] = {6560, 6560, 6560, 6425, 6371, 6560, 6560, 6560, 6560, 6560, 6560, 6425, 6371, 6560, 6560, 6560, 6560, 6560, 6560, 6560, 6398, 6452, 6398, 6560, 6560, 6560, 6560, 6560, 6560, 6560, 6560, 6479, 6344, 6479, 6560, 6560, 6560, 6560};
    while (true) {
        getline(ifs, line);
        if (ifs.eof())
            break;
        ++n;

        // 4方向で同じ盤面を作る もっと高速な書き方はあるがわかりにくいのでとりあえずこれを使う
        b.player = 0;
        for (i = 0; i < n_board_idx; ++i)
            b.board_idx[i] = first_board[i];
        for (i = 0; i < (int)line.size() - 2; i += 2) {
            x = (int)(line[i]) - (int)'a';
            y = (int)(line[i + 1]) - (int)'1';
            b = b.move(y * hw + x);
        }
        x = (int)(line[line.size() - 2]) - (int)'a';
        y = (int)(line[line.size() - 1]) - (int)'1';
        book[b] = y * hw + x;

        b.player = 0;
        for (i = 0; i < n_board_idx; ++i)
            b.board_idx[i] = first_board[i];
        for (i = 0; i < (int)line.size() - 2; i += 2) {
            x = (int)(line[i]) - (int)'a';
            y = (int)(line[i + 1]) - (int)'1';
            b = b.move(x * hw + y);
        }
        x = (int)(line[line.size() - 2]) - (int)'a';
        y = (int)(line[line.size() - 1]) - (int)'1';
        book[b] = x * hw + y;

        b.player = 0;
        for (i = 0; i < n_board_idx; ++i)
            b.board_idx[i] = first_board[i];
        for (i = 0; i < (int)line.size() - 2; i += 2) {
            x = (int)(line[i]) - (int)'a';
            y = (int)(line[i + 1]) - (int)'1';
            b = b.move((hw - 1 - y) * hw + hw - 1 - x);
        }
        x = (int)(line[line.size() - 2]) - (int)'a';
        y = (int)(line[line.size() - 1]) - (int)'1';
        book[b] = (hw - 1 - y) * hw + hw - 1 - x;

        b.player = 0;
        for (i = 0; i < n_board_idx; ++i)
            b.board_idx[i] = first_board[i];
        for (i = 0; i < (int)line.size() - 2; i += 2) {
            x = (int)(line[i]) - (int)'a';
            y = (int)(line[i + 1]) - (int)'1';
            b = b.move((hw - 1 - x) * hw + hw - 1 - y);
        }
        x = (int)(line[line.size() - 2]) - (int)'a';
        y = (int)(line[line.size() - 1]) - (int)'1';
        book[b] = (hw - 1 - x) * hw + hw - 1 - y;
    }
    cerr << n << " boards initialized" << endl;
}

int get_book(board b) {
    // 定石がヒットしなかった
    if (book.find(b) == book.end())
        return -1;
    return book[b];
}