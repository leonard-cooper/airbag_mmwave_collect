import tkinter as tk
from tkinter import Frame, Radiobutton

from PIL import Image, ImageTk

from utils.stop_recording import stop_recording
from recording import RecordingProcessor
from utils.real_time_pressure import real_time_pressure
from tkinter import messagebox

import numpy as np

class MyEntry(tk.Entry):
    def __init__(self, master=None, **kwargs):
        kwargs['font'] = ("Calibri", 15)
        super().__init__(master, **kwargs)


class MyButton(tk.Button):
    def __init__(self, master=None, **kwargs):
        # Set default attributes
        defaults = {
            "width": 9,
            "height": 1,
            "bg": "blue",
            "padx": 5,
            "pady": 2
        }
        # Update defaults with any keyword arguments passed
        defaults.update(kwargs)
        # Call the superclass constructor with updated defaults
        super().__init__(master, **defaults)


class MyFrame(tk.Frame):
    def __init__(self, master=None, **kwargs):
        # Set default attributes
        defaults = {
            "bg": "green",
            "highlightbackground": "red",
            "highlightthickness": 2,
        }
        # Update defaults with any keyword arguments passed
        defaults.update(kwargs)
        # Call the superclass constructor with updated defaults
        super().__init__(master, **defaults)


class MyLabel(tk.Label):
    def __init__(self, master=None, **kwargs):
        kwargs['font'] = ('宋体', 15)
        kwargs['bg'] = "red"
        super().__init__(master, **kwargs)


class UI:
    def __init__(self):
        self.img_png = None
        self.root = None
        self.frames = {}
        self.buttons = {}
        self.labels = {}
        self.entries = {}
        self.setup_root_window()
        self.create_widgets()
        self.map_buttons_to_functions()

    def setup_root_window(self):
        self.root = tk.Tk()
        self.root.title('mmwave Recording System')
        self.root.geometry(f"{self.root.winfo_screenwidth()}x{self.root.winfo_screenheight()}")
        self.root.config(background="#008B8B")
        # self.root.config(background="white")

    def create_widgets(self):
        self.frames['root'] = Frame(self.root)
        self.frames['root'].pack(padx=0, pady=0)
        self.create_logo_frame()
        self.create_user_info_frame()
        self.create_action_buttons_frame()
        self.create_predict_frame()
        self.create_mmwave_frame()
        self.create_pressure_frame()

    def create_logo_frame(self):
        self.frames['logo'] = Frame(self.frames['root'], bg='pink')
        self.frames['logo'].grid(row=0, column=0, padx=0, rowspan=2)

        img_open = Image.open("pics/logo.png")
        img_open = img_open.resize((100, 100))
        self.img_png = ImageTk.PhotoImage(img_open)
        label_img = tk.Label(self.frames['logo'], image=self.img_png, bg="black")
        label_img.pack()

    def create_user_info_frame(self):
        self.frames['user_info'] = Frame(self.frames['root'], bg='yellow')
        self.frames['user_info'].grid(row=0, column=1, padx=10, pady=2)

        # Label texts for row 0 and row 1
        label_texts_row_0 = ["NAME", "HEIGHT(cm)", "WEIGHT(kg)", "AGE"]
        label_texts_row_1 = ["SBP", "DBP", "HR", "GENDER"]

        # Create widgets for row 0
        for index, text in enumerate(label_texts_row_0):
            label = MyLabel(self.frames['user_info'], text=text + "：")
            label.grid(row=0, column=index * 2, padx=15)

            entry = MyEntry(self.frames['user_info'], bd=2, width=10)
            entry.grid(row=0, column=index * 2 + 1)
            self.entries[text] = entry

        # Create widgets for row 1
        for index, text in enumerate(label_texts_row_1):
            label = MyLabel(self.frames['user_info'], text=text + "：")
            label.grid(row=1, column=index * 2, padx=15)

            if text == "GENDER":
                GenderFrame = Frame(self.frames['user_info'], bd=2, width=10)
                GenderFrame.grid(row=1, column=index * 2 + 1)

                self.v = tk.StringVar()
                self.v.set('1')  # 设置默认选择值

                Radiobutton(GenderFrame, text='MALE', value='1', variable=self.v, bg="white").pack(
                    side='left')
                Radiobutton(GenderFrame, text='FEMALE', value='0', variable=self.v, bg="white").pack(
                    side='left')
            else:
                entry = MyEntry(self.frames['user_info'], bd=2, width=10)
                entry.grid(row=1, column=index * 2 + 1)
                self.entries[text] = entry

    def create_action_buttons_frame(self):
        self.frames['actions'] = Frame(self.frames['root'], bg='black')
        self.frames['actions'].grid(row=1, column=1, padx=15, pady=2)

        button_texts = ['SAVE INFO', 'CLEAR INFO', 'PRERECORD', 'RECORD', 'RTMMWAVE', 'STOP']

        # Create a dictionary to store the buttons
        self.buttons = {}

        for index, text in enumerate(button_texts):
            # Create and place the button
            button = MyButton(self.frames['actions'], text=text)
            button.grid(row=0, column=index * 2)

            # Store the button in the dictionary with the text as the key
            self.buttons[text] = button

            # Create and place the label, if it's not the last button
            if index < len(button_texts) - 1:
                label = tk.Label(self.frames['actions'], text="   ", bg="white")
                label.grid(row=0, column=index * 2 + 1)

    def create_predict_frame(self):
        self.frames['predict'] = Frame(self.frames['root'], bg='red')
        self.frames['predict'].grid(row=2, column=1, padx=10, pady=2)

        label_configs = [  # 更改变量名以避免混淆
            {"text": "HR:"},
            {"width": 20, "height": 1},
            {"text": "ACC:"},
            {"width": 20, "height": 1},
            {"text": "Pulse Tense:"},
            {"width": 20, "height": 1}
        ]
        text = None
        for index, config in enumerate(label_configs):  # 使用 config 变量
            if index % 2 == 0:
                text = config['text']
            label = MyLabel(self.frames['predict'], **config)
            label.grid(row=0, column=index + 1)
            if index % 2 == 1:
                self.labels[text] = label

    def create_mmwave_frame(self):
        self.frames['plot'] = Frame(self.root, bg='blue')
        self.frames['plot'].pack(padx=10, pady=5)

        self.progress_bar_label = tk.Label(self.frames['plot'], text="Progress", bg="#8A2BE2")
        self.progress_bar_label.grid(row=0, column=0)
        self.progress_bar_label1 = tk.Label(self.frames['plot'], width=160, bg="#8A2BE2")
        self.progress_bar_label1.grid(row=0, column=1, columnspan=8, sticky='w')

        self.label = tk.Label(self.frames['plot'], text="No signal", font=('default', 40),
                              width=44, height=2, bg="red", highlightbackground="grey", highlightthickness=0.7)
        self.label.grid(row=3, columnspan=12)

    def create_pressure_frame(self):
        self.frames['pressure'] = Frame(self.root, bg='green')
        self.frames['pressure'].pack(padx=10, pady=5)

        # self.label = tk.Label(self.frames['pressure'], text="No signal", font=('default', 40),
        #                       width=44, height=9, bg="white", highlightbackground="grey", highlightthickness=0.7)
        # self.label.grid(row=5, columnspan=12, pady=6)
        real_time_pressure(self.frames['pressure'], self.root, 'COM7')

    def map_buttons_to_functions(self):
        self.buttons['RECORD'].bind("<Button-1>", RecordingProcessor(self).record)
        self.buttons['SAVE INFO'].bind("<Button-1>", self.save_data)
        self.buttons['CLEAR INFO'].bind("<Button-1>", self.delete_data)
        self.buttons['PRERECORD'].bind("<Button-1>", RecordingProcessor(self).prerecord)
        self.buttons['STOP'].bind("<Button-1>", stop_recording)
        self.buttons['RTMMWAVE'].bind("<Button-1>", RecordingProcessor(self).RealTimeMMWAVE)
    def clear_entries(self, **kwargs):
        keys_to_clear = kwargs.get('keys', None)

        if keys_to_clear is None:
            # 清空全部
            for entry in self.entries.values():
                entry.delete(0, tk.END)
        else:
            # 只清空指定的entry
            for key in keys_to_clear:
                if key in self.entries:
                    self.entries[key].delete(0, tk.END)

    def delete_data(self, _):
        self.clear_entries()

    def save_data(self, _):
        entries = self.entries
        test = entries['NAME'].get()
        height = entries['HEIGHT(cm)'].get()
        weight = entries['WEIGHT(kg)'].get()
        gender = self.v.get()
        age = entries['AGE'].get()
        sbp = entries['SBP'].get()
        dbp = entries['DBP'].get()
        hr = entries['HR'].get()

        if test == '':
            messagebox.showinfo('Message', 'username can not be empty')
        else:
            data_list = {'test': test, 'height': height, 'weight': weight, 'gender': gender,
                         'age': age, 'sbp': sbp, 'dbp': dbp, 'hr': hr}
            np.save("./data/" + test + "-person_info" + ".npy", data_list)
            messagebox.showinfo('Message', 'Info saved')

