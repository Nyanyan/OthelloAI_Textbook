from tqdm import tqdm
from othello_py import *

def digit(n, r):
    n = str(n)
    l = len(n)
    for _ in range(r - l):
        n = '0' + n
    return n

def translate_coord(coord):
    x = chr(ord('a') + coord % hw)
    y = str(coord // hw + 1)
    return x + y

# 棋譜から扱いやすいデータを作る
records = []
for num in range(20):
    with open('self_play/' + digit(num, 7) + '.txt', 'r') as f:
        records.extend(list(f.read().splitlines()))
record_all = {}
for record in tqdm(records):
    o = othello()
    for i in range(0, len(record), 2):
        if not o.check_legal():
            o.player = 1 - o.player
            o.check_legal()
        x = ord(record[i]) - ord('a')
        y = int(record[i + 1]) - 1
        o.move(y, x)
    result = o.n_stones[0] - o.n_stones[1]
    n_vacant = hw2 - (o.n_stones[0] + o.n_stones[1])
    if result > 0:
        result += n_vacant
    elif result < 0:
        result -= n_vacant
    r = ''
    for i in range(0, len(record), 2):
        r += record[i] + record[i + 1]
        if r in record_all:
            record_all[r][0] += 1
            record_all[r][1] += result
        else:
            record_all[r] = [1, result]

book = {}

max_ln = 45

num_threshold1 = 4

inf = 100000000

def calc_value(r, player):
    if r in record_all:
        if record_all[r][0] < num_threshold1:
            return -inf
        val = record_all[r][1] / record_all[r][0]
        if player == 1:
            return -val
        return val
    return -inf

def create_book(record, player):
    if len(record) > max_ln:
        return
    policy = -1
    max_val = -inf
    for i in range(hw2):
        r = record + translate_coord(i)
        val = calc_value(r, player)
        if max_val < val:
            max_val = val
            policy = i
    if policy != -1:
        book[record] = translate_coord(policy)
        for i in range(hw2):
            r = record + translate_coord(policy) + translate_coord(i)
            if r in record_all:
                create_book(r, player)


book = {}
create_book('f5', 1)
print(len(book))
create_book('f5d6', 0)
create_book('f5f6', 0)
create_book('f5f4', 0)
print(len(book))

with open('book.txt', 'w') as f:
    f.write('f5\n')
    for r in book.keys():
        f.write(r + book[r] + '\n')