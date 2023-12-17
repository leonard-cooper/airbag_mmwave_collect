import serial
import time
import tkinter as tk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from matplotlib.animation import FuncAnimation

# 配置串口参数
serialPort = 'COM9'
baudRate = 115200
ser = serial.Serial(serialPort, baudRate, timeout=1)

# 创建Tkinter窗口
root = tk.Tk()
root.title("Serial Data Plot")

# 准备绘图
fig = Figure()
ax = fig.add_subplot(111)
xdata = list(range(100))
ydata = [0] * 100
line, = ax.plot(xdata, ydata, 'r-')

# 将Matplotlib图像嵌入到Tkinter中
canvas = FigureCanvasTkAgg(fig, master=root)
canvas_widget = canvas.get_tk_widget()
canvas_widget.pack(fill=tk.BOTH, expand=True)

def init():
    ax.set_xlim(0, 100)
    return line,

def update(frame):
    if ser.in_waiting > 0:
        try:
            data = ser.read(ser.in_waiting)
            int_data = int(data.decode('utf-8').rstrip())
            ydata.append(int_data)
            ydata.pop(0)
            line.set_data(xdata, ydata)
            ax.relim()
            ax.autoscale_view()
        except ValueError:
            print("Received non-integer data")
        except Exception as e:
            print(f"An error occurred: {e}")
    return line,

ani = FuncAnimation(fig, update, init_func=init, blit=True, interval=50)

# 运行Tkinter事件循环
root.mainloop()

# 关闭串口
ser.close()
