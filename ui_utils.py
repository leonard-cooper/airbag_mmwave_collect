import tkinter.ttk



def UpdateHR(ui, HR):
    HRLablel = tkinter.ttk.Label(ui.labels['HR:'])
    HRLablel.place(relx=0, rely=0, relwidth=1)
    HRLablel['text'] = str(HR) + ' BPM'
    # HRLablel['font'] = ('宋体', 10, 'bold')
    HRLablel['justify'] = 'left'
    ui.root.update()


def UpdateAcc(app, acc):
    ratio_max = round(acc, 3)
    accprint = tkinter.ttk.Label(app.labels['ACC:'])
    accprint.place(relx=0, rely=0, relwidth=1)
    accprint['text'] = ratio_max
    # accprint['font'] = ('宋体', 10, 'bold')
    accprint['justify'] = 'left'
    app.root.update()


