import numpy as np
# 加载先前保存的数组
loaded_data = np.load('../data/12121-230611_220858.npy', allow_pickle=True)

# 打印加载的数据
print(loaded_data)