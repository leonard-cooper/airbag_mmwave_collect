# Xiaomi_Infineon_project

#### 项目目标
在57GHZ-64GHZ频段内，利用英飞凌BGT60TR13芯片实现如下功能：完成算法仿真，优化，测试，并可有效排除窗帘等环境因素干扰。使用英飞凌开发板或者自己设计开发板，MCU 请评估ARM-M4、M7、或者国产同等价位MCU，将算法在MCU内实现，完成固件控制，算法实现，进行测试，达到研究指标。

#### 具体目标
1. 利用mmWave实现设备控制，实现在近距离（10-30厘米）的手势识别，进行隔空操控（单击、双击、左划、右划、顺时针旋转、逆时针旋转）；在远距离（大于3米）的手势识别（左挥手、右挥手、敲击、前推、后拉）。
<br>目标：基于mmWave实现新用户10-30厘米内手势识别准确率95%以上，3米以上手势识别准确率90%以上。标准模式下，开发板整机平均功耗小于1W。
2. 利用mmWave实现特定场景下的姿态识别，包括人体的动态及状态的识别，包含跑步、走路、站立、跳跃、坐、跌倒、躺等基本姿态，并根据个体特征及动作习惯识别不同个体
<br>考核指标： 基于mmWave实现新用户2-4米内人体姿态识别准确率90%以上，已标注用户个体识别率85%以上。标准模式下，开发板整机平均功耗小于1W。
3. 利用mmWave实现生理监测，包括近距离心率呼吸检测（0.5-1米）、远距离呼吸检测（2.5米），实现对呼吸和心率异常状态的识别。对2米内的睡眠场景，进行睡眠时间分析（入睡/起床等），睡眠分期（清醒、REM、浅睡、深睡）。
<br>考核指标：基于mmWave实现新用户呼吸检测、心率检测准确率90%以上。睡眠时间分析误差小于20min，睡眠分期准确率达到85%以上，标准模式下，开发板整机平均功耗小于1W。
4. 利用mmWave实现特定场景下的存在检测，包括运动时的存在检测，静坐办公的存在检测，以及睡眠状态下的存在检测，包含走、坐、躺、睡眠等基本场景，将心率呼吸检测引入到存在检测算法中。
<br>考核指标： 基于mmWave实现新用户0.3-5米以内人体存在检测，识别准确率95%以上。标准模式下，开发板整机平均功耗小于1W。
5. 利用mmWave实现轨迹追踪，可以对1-5个人运动轨迹进行追踪，并快速刷新每个人的运动轨迹，可在人员运动重叠时有效区分不同人的轨迹。
<br>考核指标： 基于mmWave实现新用户1-5米以内，1-5个人的轨迹追踪；轨迹追踪测距精度正负15cm，测角精度正负15°。标准模式下，延时不超过2s，开发板整机平均功耗小于1W。
6. 利用mmWave实现移动设备避障，可以检测到50厘米以内的所有小型障碍物（比如拖鞋/桌角/USB线/钢笔等），进行障碍物提前预警。主要关注毫米波传感器在自身处于运动状态下的算法性能。<br>考核指标： 基于mmWave实现50厘米内的所有家庭障碍物检测，识别准确率高于90%。标准模式下，开发板整机平均功耗小于1W。
