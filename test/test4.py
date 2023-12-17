import tkinter as tk

root = tk.Tk()

# A label that only occupies one row
label1 = tk.Label(root, text="Label 1", bg="red")
label1.grid(row=0, column=0)

# A label that occupies two rows
label2 = tk.Label(root, text="Label 2", bg="green")
label2.grid(row=0, column=1, rowspan=2)

# Another label in the second row
label3 = tk.Label(root, text="Label 3", bg="blue")
label3.grid(row=1, column=0)

root.mainloop()
