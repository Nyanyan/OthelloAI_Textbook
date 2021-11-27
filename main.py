# -*- coding: utf-8 -*-

# AIとの連携に使うライブラリのインポート
import subprocess

# コマンドライン引数を使うために使うライブラリ
import sys

# オセロクラス
from othello_py import *

if len(sys.argv) == 1 or sys.argv[1] == 'gui':
    
    # GUIを作るためのライブラリのインポート
    import tkinter
    
    # AIの手番の選択
    try:
        ai_player = int(input('AIの手番 0: 黒(先手) 1: 白(後手) : '))
    except:
        print('0か1を入力してください')
        exit()
    if ai_player != 0 and ai_player != 1:
        print('0か1を入力してください')
        exit()
    
    ai_exe = subprocess.Popen('./ai.out'.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    ai_exe.stdin.write((str(ai_player) + '\n').encode('utf-8'))
    ai_exe.stdin.flush()
    
    # 定数
    offset_y = 10
    offset_x = 10
    rect_size = 60
    circle_offset = 3
    
    o = othello()
    legal_buttons = []

    # GUI部分
    app = tkinter.Tk()
    app.geometry('500x700')
    app.title('オセロAIの教科書 サンプルプログラム')
    canvas = tkinter.Canvas(app, width=500, height = 700)
    pixel_virtual = tkinter.PhotoImage(width=1, height=1)
    
    # 盤面の作成
    for y in range(hw):
        for x in range(hw):
            canvas.create_rectangle(offset_x + rect_size * x, offset_y + rect_size * y, offset_x + rect_size * (x + 1), offset_y + rect_size * (y + 1), outline='black', width=2, fill='#16a085')
    
    # 石数表示
    stone_str = tkinter.StringVar()
    stone_str.set('*● 2 - 2 ○ ')
    stone_label = tkinter.Label(canvas, textvariable=stone_str, font=('', 50))
    stone_label.place(x=250, y=600, anchor=tkinter.CENTER)
    
    def ai():
        global clicked
        grid_str = ''
        for i in range(hw):
            for j in range(hw):
                grid_str += '0' if o.grid[i][j] == 0 else '1' if o.grid[i][j] == 1 else '.'
        grid_str += '\n'
        print(grid_str)
        ai_exe.stdin.write(grid_str.encode('utf-8'))
        ai_exe.stdin.flush()
        y, x = [int(elem) for elem in ai_exe.stdout.readline().decode().split()]
        print(y, x)
        clicked = True
        o.move(y, x)
        if not o.check_legal():
            o.player = 1 - o.player
            if not o.check_legal():
                o.print_info()
                ai_exe.kill()
                o.player = -1
                print('終局しました')
        s = ''
        if o.player == 0:
            s += '*'
        else:
            s += ' '
        s += '● '
        s += str(o.n_stones[0])
        s += ' - '
        s += str(o.n_stones[1])
        s += ' ○'
        if o.player == 1:
            s += '*'
        else:
            s += ' '
        stone_str.set(s)
        o.print_info()
        show_grid()
    
    def get_coord(event):
        global clicked
        y = int(event.widget.cget('text')[0])
        x = int(event.widget.cget('text')[2])
        print(y, x)
        clicked = True
        o.move(y, x)
        if not o.check_legal():
            o.player = 1 - o.player
            if not o.check_legal():
                o.print_info()
                ai_exe.kill()
                o.player = -1
                print('終局しました')
        s = ''
        if o.player == 0:
            s += '*'
        else:
            s += ' '
        s += '● '
        s += str(o.n_stones[0])
        s += ' - '
        s += str(o.n_stones[1])
        s += ' ○'
        if o.player == 1:
            s += '*'
        else:
            s += ' '
        stone_str.set(s)
        o.print_info()
        show_grid()
    
    def show_grid():
        global clicked, legal_buttons
        for button in legal_buttons:
            button.place_forget()
        legal_buttons = []
        for y in range(hw):
            for x in range(hw):
                try:
                    canvas.delete(str(y) + '_' + str(x))
                except:
                    pass
                if o.grid[y][x] == vacant:
                    continue
                color = ''
                if o.grid[y][x] == black:
                    color = 'black'
                elif o.grid[y][x] == white:
                    color = 'white'
                elif o.grid[y][x] == legal:
                    end_flag = False
                    if o.player != ai_player:
                        color = '#3498db'
                        legal_buttons.append(tkinter.Button(canvas, image=pixel_virtual, width=rect_size - circle_offset * 2, height=rect_size - circle_offset * 2, bg=color, text=str(y) + '_' + str(x)))
                        legal_buttons[-1].bind('<ButtonPress>', get_coord)
                        legal_buttons[-1].place(y=offset_y + rect_size * y, x=offset_x + rect_size * x)
                    continue
                canvas.create_oval(offset_x + rect_size * x + circle_offset, offset_y + rect_size * y + circle_offset, offset_x + rect_size * (x + 1) - circle_offset, offset_y + rect_size * (y + 1) - circle_offset, outline='black', width=2, fill=color, tag=str(y) + '_' + str(x))
        if o.player == ai_player:
            app.after(10, ai)
    
    canvas.place(y=0, x=0)
    o.check_legal()
    show_grid()
    app.mainloop()

elif sys.argv[1] == 'cui':
    
    # AIの手番の選択
    try:
        ai_player = int(input('AIの手番 0: 黒(先手) 1: 白(後手) : '))
    except:
        print('0か1を入力してください')
        exit()
    if ai_player != 0 and ai_player != 1:
        print('0か1を入力してください')
        exit()
    
    ai_exe = subprocess.Popen('./ai.out'.split(), stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    ai_exe.stdin.write((str(ai_player) + '\n').encode('utf-8'))
    ai_exe.stdin.flush()
    
    o = othello()
    
    def ai():
        grid_str = ''
        for i in range(hw):
            for j in range(hw):
                grid_str += '0' if o.grid[i][j] == 0 else '1' if o.grid[i][j] == 1 else '.'
        grid_str += '\n'
        print(grid_str)
        ai_exe.stdin.write(grid_str.encode('utf-8'))
        ai_exe.stdin.flush()
        y, x = [int(elem) for elem in ai_exe.stdout.readline().decode().split()]
        print(y, x)
        o.move(y, x)
    
    while True:
        
        # 合法手生成とパス判定
        if not o.check_legal():
            o.player = 1 - o.player
            
            # 終局
            if not o.check_legal():
                break
        
        o.print_info()
        if o.player == ai_player:
            ai()
        else:
            o.move_stdin()
else:
    message = '''
使い方

【画面表示をする場合】
    python main.py gui
    または
    python main.py

【コマンドラインで遊ぶ場合】
    python main.py cui
    
    '''
    print(message)