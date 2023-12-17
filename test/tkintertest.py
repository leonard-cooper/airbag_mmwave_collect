import tkinter as tk
from tkinter.font import Font

window = tk.Tk()

# Create a Font object with a specific size
custom_font = Font(size=6)

label = tk.Label(
    text='2023 12 12 Leonard Cooper',
    width='30',
    height='1',
    font=custom_font)
label.pack()

window.mainloop()