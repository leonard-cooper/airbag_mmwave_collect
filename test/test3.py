import tkinter as tk

class MyFrame(tk.Frame):
    def __init__(self, master=None, **kwargs):
        # Set default attributes
        defaults = {
            "bg": "green",
        }
        # Update defaults with any keyword arguments passed
        defaults.update(kwargs)
        # Call the superclass constructor with updated defaults
        super().__init__(master, **defaults)

# Create the main application window
root = tk.Tk()

# Create an instance of MyFrame
my_frame = MyFrame(root, width=200, height=100)
my_frame.pack(padx=20, pady=20)

# Start the Tkinter event loop
root.mainloop()
