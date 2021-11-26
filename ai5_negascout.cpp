#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include "board.hpp"
#include "cell_evaluation.hpp"

using namespace std;

#define inf 100000000               // 大きな値
#define cache_hit_bonus 1000        // 前回の探索で枝刈りされなかったノードへのボーナス

unordered_map<board, int, board::hash> transpose_table_upper;          // 現在の探索結果を入れる置換表(上限): 同じ局面に当たった時用
unordered_map<board, int, board::hash> transpose_table_lower;          // 現在の探索結果を入れる置換表(下限): 同じ局面に当たった時用
unordered_map<board, int, board::hash> former_transpose_table_upper;   // 前回の探索結果が入る置換表(上限): move orderingに使う
unordered_map<board, int, board::hash> former_transpose_table_lower;   // 前回の探索結果が入る置換表(下限): move orderingに使う

unsigned long long visited_nodes; // 訪問ノード数

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

// move ordering用評価値の計算
inline int calc_move_ordering_value(const board b) {
    int res;
    if (former_transpose_table_upper.find(b) != former_transpose_table_upper.end()) {
        // 前回の探索で上限値が格納されていた場合
        res = cache_hit_bonus - former_transpose_table_upper[b];
    } else if (former_transpose_table_lower.find(b) != former_transpose_table_lower.end()) {
        // 前回の探索で下限値が格納されていた場合
        res = cache_hit_bonus - former_transpose_table_lower[b];
    } else {
        // 前回の探索で枝刈りされた
        res = -evaluate(b);
    }
    return res;
}

// move orderingと置換表つきnegaalpha法 null windows searchに使う
int nega_alpha_transpose(board b, int depth, bool passed, int alpha, int beta) {
    ++visited_nodes;

    // 葉ノードでは評価関数を実行する
    if (depth == 0)
        return evaluate(b);
    
    // 置換表から上限値と下限値があれば取得
    int u = inf, l = -inf;
    if (transpose_table_upper.find(b) != transpose_table_upper.end())
        u = transpose_table_upper[b];
    if (transpose_table_lower.find(b) != transpose_table_lower.end())
        l = transpose_table_lower[b];
    
    // u==l、つまりもうminimax値が求まっていれば探索終了
    if (u == l)
        return u;
    
    // 置換表の値を使って探索窓を狭められる場合は狭める
    alpha = max(alpha, l);
    beta = min(beta, u);
    
    // 葉ノードでなければ子ノードを列挙
    int coord, g, max_score = -inf, canput = 0;
    vector<board> child_nodes;
    for (coord = 0; coord < hw2; ++coord) {
        if (b.legal(coord)) {
            child_nodes.push_back(b.move(coord));
            child_nodes[canput].value = calc_move_ordering_value(child_nodes[canput]);
            ++canput;
        }
    }
    
    // パスの処理 手番を交代して同じ深さで再帰する
    if (canput == 0) {
        // 2回連続パスなら評価関数を実行
        if (passed)
            return evaluate(b);
        b.player = 1 - b.player;
        return -nega_alpha_transpose(b, depth, true, -beta, -alpha);
    }
    
    // move ordering実行
    if (canput >= 2)
        sort(child_nodes.begin(), child_nodes.end());
    
    // 探索
    for (const board& nb: child_nodes) {
        g = -nega_alpha_transpose(nb, depth - 1, false, -beta, -alpha);
        if (g >= beta) { // 興味の範囲よりもminimax値が上のときは枝刈り fail high
            if (g > l) {
                // 置換表の下限値に登録
                transpose_table_lower[b] = g;
            }
            return g;
        }
        alpha = max(alpha, g);
        max_score = max(max_score, g);
    }
    
    if (max_score < alpha) {
        // 置換表の下限値に登録 fail low
        transpose_table_upper[b] = max_score;
    } else {
        // minimax値が求まった
        transpose_table_upper[b] = max_score;
        transpose_table_lower[b] = max_score;
    }
    return max_score;
}


// negascout法
int nega_scout(board b, int depth, bool passed, int alpha, int beta) {
    ++visited_nodes;

    // 葉ノードでは評価関数を実行する
    if (depth == 0)
        return evaluate(b);

    // 置換表から上限値と下限値があれば取得
    int u = inf, l = -inf;
    if (transpose_table_upper.find(b) != transpose_table_upper.end())
        u = transpose_table_upper[b];
    if (transpose_table_lower.find(b) != transpose_table_lower.end())
        l = transpose_table_lower[b];

    // u==l、つまりもうminimax値が求まっていれば探索終了
    if (u == l)
        return u;

    // 置換表の値を使って探索窓を狭められる場合は狭める
    alpha = max(alpha, l);
    beta = min(beta, u);

    // 葉ノードでなければ子ノードを列挙
    int coord, g, max_score = -inf, canput = 0;
    vector<board> child_nodes;
    for (coord = 0; coord < hw2; ++coord) {
        if (b.legal(coord)) {
            child_nodes.push_back(b.move(coord));
            child_nodes[canput].value = calc_move_ordering_value(child_nodes[canput]);
            ++canput;
        }
    }

    // パスの処理 手番を交代して同じ深さで再帰する
    if (canput == 0) {
        // 2回連続パスなら評価関数を実行
        if (passed)
            return evaluate(b);
        b.player = 1 - b.player;
        return -nega_scout(b, depth, true, -beta, -alpha);
    }

    // move ordering実行
    if (canput >= 2)
        sort(child_nodes.begin(), child_nodes.end());

    // まず最善手候補を通常窓で探索
    g = -nega_scout(child_nodes[0], depth - 1, false, -beta, -alpha);
    if (g >= beta) { // 興味の範囲よりもminimax値が上のときは枝刈り fail high
        if (g > l) {
            // 置換表の下限値に登録
            transpose_table_lower[b] = g;
        }
        return g;
    }
    alpha = max(alpha, g);
    max_score = max(max_score, g);

    // 残りの手をnull window searchを使って高速に探索
    for (int i = 1; i < canput; ++i) {
        // まずはnull window search
        g = -nega_alpha_transpose(child_nodes[i], depth - 1, false, -alpha - 1, -alpha);
        if (g >= beta) { // 興味の範囲よりもminimax値が上のときは枝刈り fail high
            if (g > l) {
                // 置換表の下限値に登録
                transpose_table_lower[b] = g;
            }
            return g;
        }
        if (g > alpha) { // 最善手候補よりも良い手が見つかった場合は再探索
            alpha = g;
            g = -nega_scout(child_nodes[i], depth - 1, false, -beta, -alpha);
            if (g >= beta) { // 興味の範囲よりもminimax値が上のときは枝刈り fail high
                if (g > l) {
                    // 置換表の下限値に登録
                    transpose_table_lower[b] = g;
                }
                return g;
            }
        }
        alpha = max(alpha, g);
        max_score = max(max_score, g);
    }
    if (max_score < alpha) {
        // 置換表の下限値に登録 fail low
        transpose_table_upper[b] = max_score;
    } else {
        // minimax値が求まった
        transpose_table_upper[b] = max_score;
        transpose_table_lower[b] = max_score;
    }
    return max_score;
}

// depth手読みの探索
int search(board b, int depth) {
    visited_nodes = 0;
    transpose_table_upper.clear();
    transpose_table_lower.clear();
    former_transpose_table_upper.clear();
    former_transpose_table_lower.clear();
    // 子ノードを全列挙
    int coord, canput = 0;
    vector<board> child_nodes;
    for (coord = 0; coord < hw2; ++coord) {
        if (b.legal(coord)) {
            child_nodes.push_back(b.move(coord));
            ++canput;
        }
    }
    // 1手ずつ探索を深める
    int search_depth, res, score, alpha, beta, i;
    int start_depth = max(1, depth - 3); // 最初に探索する手数
    for (search_depth = start_depth; search_depth <= depth; ++search_depth) {
        alpha = -inf;
        beta = inf;
        if (canput >= 2) {
            // move orderingのための値を得る
            for (board &nb: child_nodes)
                nb.value = calc_move_ordering_value(nb);
            // move ordering実行
            sort(child_nodes.begin(), child_nodes.end());
        }

        // 最善手候補を通常窓で探索
        score = -nega_scout(child_nodes[0], search_depth - 1, false, -beta, -alpha);
        alpha = score;
        res = child_nodes[0].policy;

        // 残りの手をnull window searchで探索
        for (i = 1; i < canput; ++i) {
            score = -nega_alpha_transpose(child_nodes[i], search_depth - 1, false, -alpha - 1, -alpha);
            // 最善手候補よりも良い手が見つかった
            if (alpha < score) {
                alpha = score;
                score = -nega_scout(child_nodes[i], search_depth - 1, false, -beta, -alpha);
                res = child_nodes[i].policy;
            }
            alpha = max(alpha, score);
        }
        cerr << "searched depth " << search_depth << " policy " << res << " visited nodes " << visited_nodes << endl;
        transpose_table_upper.swap(former_transpose_table_upper);
        transpose_table_upper.clear();
        transpose_table_lower.swap(former_transpose_table_lower);
        transpose_table_lower.clear();
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
        policy = search(b, 10);
        cout << policy / hw << " " << policy % hw << endl;
    }
    return 0;
}