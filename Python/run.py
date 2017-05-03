#!/usr/bin env python

from tkinter import Tk, Frame, BOTH
import tkinter
tk = tkinter
import pandas as pd
from collections import OrderedDict

import matplotlib
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2TkAgg
from matplotlib.figure import Figure

N_tubes = 1
sample_names = ["sample_"+str(i+1) for i in range(N_tubes)]


class Instrument(object):
    def __init__(self, port):
        super(Instrument, self).__init__()
        self.port = port
        self.running = False

    def _send_msg(self, msg):
        pass

    def _receive_msg(self):
        pass

    def send_stop(self):
        pass

    def send_start(self, params=None):
        pass


class RunData(object):
    def __init__(self, names):
        self.names = names
        self.up_df = pd.DataFrame(columns=names)
        self.dw_df = pd.DataFrame(columns=names)

    def add_data(self, time, sense, readings):
        dat = pd.Series({n: r for n, r in zip(self.names, readings)})
        dat.name = time
        if sense == 0:
            self.dw_df.append(dat)
        else:
            self.up_df.append(dat)

    def plot(self):
        fig = Figure(figsize=(5,5), dpi=100)
        ax = fig.add_subplot(111)
        #ax.plot(self.dw_df.index, self.dw_df[sample])
        ax.plot([1,2,3,4,5,6,7,8],[5,6,1,3,8,9,3,5])
        return fig

    def write_data(self, path):
        df = pd.concat(self.up_df, self.dw_df)
        df = df.sort_index()
        df.to_csv(path, sep="\t")


class MainWindow(Frame):

    def __init__(self, parent):
        Frame.__init__(self, parent, background="white")

        self.parent = parent

        self.initUI()

    def initUI(self):

        self.pack(fill=BOTH, expand=1)
        self.configure(bg="#fff")
        self.columnconfigure(0, weight=1)
        self.columnconfigure(1, weight=5)
        self.rowconfigure(1, weight=1)

        image = tkinter.PhotoImage(file='../../logoer/Visco_logo_1.gif')
        self.header = tkinter.Label(self, image=image)
        self.header.img = image
        #label.grid(sticky="nsew")

        '''self.header = tkinter.Label(
            self,
            text="Visconetics",
            font=("Times", 50),
            justify=tkinter.CENTER,
            bg="#ddd"
        )'''
        self.header.grid(row=0, column=0, columnspan=2, sticky="nsew")
        ExpandingFrame(self, bg="black", height=2).grid(row=0, column=0, columnspan=2, sticky="sew")

class ExpandingFrame(Frame):
    def __init__(self, parent, *args, **kwargs):
        super(ExpandingFrame, self).__init__(parent, *args, **kwargs)
        self.pack(fill=BOTH, expand=1)

class PlotFrame(Frame):
    def __init__(self, parent):
        super(PlotFrame, self).__init__(parent, background="red")
        self.parent = parent
        self.configure(bg="#eee")
        self.pack(fill=BOTH, expand=1)

        #self.canvas = tkinter.Canvas(self)
        #self.canvas.grid(sticky="nsew")

        #self.update_plot()

    def update_plot(self, fig):
        canvas = FigureCanvasTkAgg(fig, self)
        canvas.show()
        canvas.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

        toolbar = NavigationToolbar2TkAgg(canvas, self)
        toolbar.update()
        canvas._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        # image = tkinter.PhotoImage(file='tmp/example.gif')
        # label = tkinter.Label(self, image=image)
        # label.img = image
        # label.grid(sticky="nsew")

class ControlPanel(Frame):
    def __init__(self, parent):
        super(ControlPanel, self).__init__(parent, width=200)
        self.parent = parent

        tkinter.Label(self, text="Controls", font=("Calibri", 20)).grid(row=0)
        self.populate_buttons()

    def populate_buttons(self):
        buttonwidth = 10
        start = tkinter.Button(self, text="Start", width=buttonwidth)
        start.grid(row=1)

        stop = tkinter.Button(self, text="Stop", width=buttonwidth)
        stop.grid(row=2)

        export = tkinter.Button(self, text="Export", width=buttonwidth)
        export.grid(row=3)

data = RunData(sample_names)

def make_app():
    root = Tk()
    root.geometry("700x500+300+300")
    root.title("Visconetics")
    root.minsize(700, 500)
    app = MainWindow(root)
    control_panel = ControlPanel(app)
    control_panel.grid(column=0, row=1, sticky="n")
    ExpandingFrame(app, width=2, bg="black").grid(column=0, row=1, sticky="nse")

    fig = data.plot()

    plot = PlotFrame(app)
    plot.pack(fill=BOTH, expand=1)
    plot.grid(column=1, row=1, sticky="nsew")

    plot.update_plot(fig)

    return root

def main():

     # Container for the DataFrame
    intstrument = Instrument("/dev/tty1234") # Connection to the instrument

    root = make_app()

    root.mainloop()


if __name__ == '__main__':
    main()
    print("Closing...")
