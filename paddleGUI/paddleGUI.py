'''
TKinter interface for controlling the mode of a haptic paddle
Elecanisms 2020
'''

import tkinter as tk
import paddle

class paddleGUI:

    def __init__(self):
        self.dev = paddle.paddle()
        if self.dev.dev is not None:
            self.update_job = None
            self.root = tk.Tk()
            self.root.title('Haptic Paddle GUI')
            self.root.protocol('WM_DELETE_WINDOW', self.shut_down)
            fm = tk.Frame(self.root)
            tk.Button(fm, text = 'SPRING', command = self.dev.spring).pack(side = tk.LEFT)
            tk.Button(fm, text = 'DAMPER', command = self.dev.damper).pack(side = tk.LEFT)
            tk.Button(fm, text = 'TEXTURE', command = self.dev.texture).pack(side = tk.LEFT)
            tk.Button(fm, text = 'WALL', command = self.dev.wall).pack(side = tk.LEFT)
            fm.pack(side = tk.TOP)
            dutyslider = tk.Scale(self.root, from_=0, to=15, orient=tk.HORIZONTAL, showvalue=tk.FALSE, command=self.set_duty_callback)
            dutyslider.set(25)
            dutyslider.pack(side = tk.TOP)
            self.angle_status = tk.Label(self.root, text = 'Angle: ?')
            self.angle_status.pack(side = tk.TOP)
            self.mode_status = tk.Label(self.root, text = 'Mode: ?')
            self.mode_status.pack(side = tk.TOP)
            # self.sw1_status = tk.Label(self.root, text = 'SW1 is currently ?')
            # self.sw1_status.pack(side = tk.TOP)
            # self.sw2_status = tk.Label(self.root, text = 'SW2 is currently ?')
            # self.sw2_status.pack(side = tk.TOP)
            # self.sw3_status = tk.Label(self.root, text = 'SW3 is currently ?')
            # self.sw3_status.pack(side = tk.TOP)
            # self.a0_status = tk.Label(self.root, text = 'A0 is currently ????')
            # self.a0_status.pack(side = tk.TOP)
            self.update_status()

    def set_duty_callback(self, value):
        self.dev.set_duty(float(value)) # TODO CHANGE TO WHATEVER THE SLIDER NEEDS FOR TEXTURE

    def update_status(self):
        self.angle_status.configure(text = 'Angle: {!s}'.format(str(self.dev.get_angle()))) # TODO change to reading angle
        # self.mode_status.configure(text = 'Mode: {!s}'.format(self.dev._______())) # TODO change to reading mode
        # self.sw1_status.configure(text = 'SW1 is currently {!s}'.format(self.dev.read_sw1()))
        # self.sw2_status.configure(text = 'SW2 is currently {!s}'.format(self.dev.read_sw2()))
        # self.sw3_status.configure(text = 'SW3 is currently {!s}'.format(self.dev.read_sw3()))
        # self.a0_status.configure(text = 'A0 is currently {:04d}'.format(self.dev.read_a0()))
        self.update_job = self.root.after(50, self.update_status)

    def shut_down(self):
        self.root.after_cancel(self.update_job)
        self.root.destroy()
        self.dev.close()

if __name__=='__main__':
    gui = paddleGUI()
    gui.root.mainloop()
