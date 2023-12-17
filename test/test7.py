import serial
import tkinter as tk
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg

# 配置串口参数
serialPort = 'COM9'
baudRate = 115200

# 尝试打开串口
try:
    ser = serial.Serial(serialPort, baudRate, timeout=1)
except serial.SerialException as e:
    print(f"Error opening serial port {serialPort}: {e}")
    exit()

# 创建Tkinter窗口
root = tk.Tk()
root.title("Real-time Serial Data Plot")

# 创建绘图Figure和Axes
fig = Figure(figsize=(6, 4), dpi=100)
ax = fig.add_subplot(111)
xdata = list(range(100))
ydata = [0] * 100
line, = ax.plot(xdata, ydata, 'r-')

# 嵌入Figure到Tkinter窗口
canvas = FigureCanvasTkAgg(fig, master=root)
canvas_widget = canvas.get_tk_widget()
canvas_widget.pack(fill=tk.BOTH, expand=True)

def update_plot():
    if ser.in_waiting > 0:
        try:
            data = ser.read(ser.in_waiting)
            int_data = int(data.decode('utf-8').rstrip())
            ydata.append(int_data)
            ydata.pop(0)
            line.set_data(xdata, ydata)
            ax.relim()
            ax.autoscale_view()
            canvas.draw()
        except ValueError:
            print("Received non-integer data")
        except Exception as e:
            print(f"An error occurred: {e}")
    root.after(50, update_plot)

# 启动周期性更新
update_plot()

# 运行Tkinter主循环
root.mainloop()

# 关闭串口
ser.close()
