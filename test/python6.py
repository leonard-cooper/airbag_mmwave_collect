import serial
import time
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# 配置串口参数
serialPort = 'COM9'  # 串口号设置为COM9
baudRate = 115200    # 波特率设置为115200

# 创建串口对象
ser = serial.Serial(serialPort, baudRate, timeout=1)

# 准备绘图
fig, ax = plt.subplots()
xdata = list(range(100))  # x轴数据点固定为最近的100个点
ydata = [0] * 100         # 初始化y轴数据
ln, = plt.plot(xdata, ydata, 'r-', animated=True)

def init():
    ax.set_xlim(0, 100)  # x轴显示范围固定为0到100
    return ln,

def update(frame):
    if ser.in_waiting > 0:
        try:
            data = ser.read(ser.in_waiting)

            int_data = int(data.decode('utf-8').rstrip())
            print(int_data)
            ydata.append(int_data)
            ydata.pop(0)        # 移除最旧的数据点
            ln.set_data(xdata, ydata)
            ax.set_ylim(min(ydata), max(ydata))  # 根据数据动态调整y轴范围
            # ax.relim()          # 重新计算坐标轴的范围
            # ax.autoscale_view() # 根据新的边界自动调整视图
            ax.draw_artist(ax.yaxis)  # 重绘y轴
            fig.canvas.update()  # 更新画布
            fig.canvas.flush_events()  # 刷新事件
        except ValueError:
            print("Received non-integer data")
        except Exception as e:
            print(f"An error occurred: {e}")
    return ln,

ani = FuncAnimation(fig, update, init_func=init, blit=True, interval=50)
plt.show()

ser.close()