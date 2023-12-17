import tkinter as tk

root = tk.Tk()

button1 = tk.Button(root, text="Flat", relief="flat")
button1.pack(padx=20, pady=20)

button2 = tk.Button(root, text="Raised", relief="raised")
button2.pack(padx=20, pady=20)

button3 = tk.Button(root, text="Sunken", relief="sunken")
button3.pack(padx=20, pady=20)

button4 = tk.Button(root, text="Groove", relief="groove")
button4.pack(padx=20, pady=20)

button5 = tk.Button(root, text="Ridge", relief="ridge")
button5.pack(padx=20, pady=20)

button6 = tk.Button(root, text="Solid", relief="solid")
button6.pack(padx=20, pady=20)

root.mainloop()
