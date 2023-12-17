def stop_recording(_):
    StopFlag.flag = True


class StopFlag:
    flag = False
