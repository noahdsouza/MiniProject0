'''
TKinter interface for controlling the mode of a haptic paddle
Elecanisms 2020
'''

import tkinter as tk

class paddleGUI(tk.Tk):

    def __init__(self):
        tk.Tk.__init__(self)
        self.winfo_toplevel().title("Haptic Paddle Control")
        self.configure(bg='black')
        
