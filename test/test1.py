import tkinter as tk

root = tk.Tk()

frame = tk.Frame(root, borderwidth=1, relief="solid")
frame.pack(padx=20, pady=20)

label = tk.Label(frame, text="This is inside the frame", borderwidth=1, relief="solid")
label.pack(padx=10, pady=10)

root.mainloop()
