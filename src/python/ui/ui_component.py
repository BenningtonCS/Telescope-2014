import wx
from cv2 import cvtColor, COLOR_BGR2RGB, VideoCapture, imwrite

import matplotlib

matplotlib.use('WXAgg')
from matplotlib.figure import Figure
from matplotlib.backends.backend_wxagg import \
    FigureCanvasWxAgg as FigCanvas
import numpy as np
import random
import pylab

import time
import datetime


class WebcamPanel(wx.Panel):
    def __init__(self, parent, id, pos, size, travers):
        wx.Panel.__init__(self, parent, id, pos, size, travers)

        self.primary_sizer = None
        self.is_active = False
        self.capture = None
        self.frame = None
        self.draw_disabled_panel()

    def enable(self):
        pass

    def disable(self):
        pass

    def draw_enabled_panel(self):
        self.fps = 5
        self._enable_active_state()

        ret, self.frame = self.capture.read()

        height, width = self.frame.shape[:2]
        self.frame = cvtColor(self.frame, COLOR_BGR2RGB)

        self.bmp = wx.BitmapFromBuffer(width, height, self.frame)

        self.timer = wx.Timer(self)
        self.timer.Start(1000. / self.fps)
        self.is_active = True

        self.Bind(wx.EVT_PAINT, self.on_paint)
        self.Bind(wx.EVT_TIMER, self.next_frame)

    def draw_disabled_panel(self):
        # Disable Active
        self._disable_active_state()

        if not self.primary_sizer:
            # Update View
            self.disable_el = self.get_disabled_text_element()

            self.primary_sizer = wx.BoxSizer(wx.VERTICAL)
            self.primary_sizer.AddSpacer((0, 0), 1, wx.EXPAND, 5)
            self.primary_sizer.Add(self.disable_el, 0, wx.ALIGN_CENTER_VERTICAL | wx.ALL | wx.EXPAND, 2)
            self.primary_sizer.AddSpacer((0, 0), 1, wx.EXPAND, 5)

            self.SetSizer(self.primary_sizer)
            self.Layout()
            self.primary_sizer.Fit(self)

    def get_disabled_text_element(self):
        el = wx.StaticText(self, wx.ID_ANY, u"webcam is disabled", wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_CENTRE)
        el.Wrap(-1)
        el.SetForegroundColour(wx.Colour(190, 190, 190))
        return el

    def _enable_active_state(self):
        self.primary_sizer.ShowItems(False)
        self.capture = VideoCapture(0)

    def _disable_active_state(self):
        if self.is_active:
            self.Unbind(wx.EVT_PAINT, handler=self.on_paint)
            self.Unbind(wx.EVT_TIMER, handler=self.next_frame)
            self.frame = None
            self.capture.release()
            self.timer.Destroy()
            self.bmp.Destroy()
            self.Layout()

        if self.primary_sizer:
            self.SetForegroundColour(wx.Colour(140, 140, 140))
            self.primary_sizer.ShowItems(True)

    def save_image(self, filename=None):
        if not filename:
            tstamp = datetime.datetime.fromtimestamp(time.time()).strftime('%Y-%m-%d-%H-%M-%S')
            filename = 'capture_{}{}'.format(tstamp, '.jpg')

        if self.frame is not None:
            imwrite(filename, self.frame)

    def on_paint(self, event):
        dc = wx.BufferedPaintDC(self)
        dc.DrawBitmap(self.bmp, 0, 0)

    def next_frame(self, event):
        ret, self.frame = self.capture.read()
        if ret:
            self.frame = cvtColor(self.frame, COLOR_BGR2RGB)
            self.bmp.CopyFromBuffer(self.frame)
            self.Refresh()


class DataGen(object):
    """ A silly class that generates pseudo-random data for
        display in the plot.
    """

    def __init__(self, init=50):
        self.data = self.init = init

    def next(self):
        self._recalc_data()
        return self.data

    def _recalc_data(self):
        delta = random.uniform(-0.5, 0.5)
        r = random.random()

        if r > 0.9:
            self.data += delta * 15
        elif r > 0.8:
            # attraction to the initial value
            delta += (0.5 if self.init > self.data else -0.5)
            self.data += delta
        else:
            self.data += delta


def pixels_to_inch(pixels):
    return pixels / 96


class GraphPanel(wx.Panel):
    """
    A panel containing a graph
    """

    def __init__(self, parent, id, pos, size, travers):
        wx.Panel.__init__(self, parent, id, pos, size, travers)

        self.datagen = DataGen()
        self.data = [self.datagen.next()]
        self.paused = False

        self.init_plot()

        self.canvas = FigCanvas(self, -1, self.fig)

        self.vbox = wx.BoxSizer(wx.VERTICAL)
        self.vbox.Add(self.canvas, 1, flag=wx.LEFT | wx.TOP | wx.GROW)
        self.SetSizer(self.vbox)
        self.vbox.Fit(self)

        self.redraw_timer = wx.Timer(self)
        self.Bind(wx.EVT_TIMER, self.on_redraw_timer, self.redraw_timer)
        self.redraw_timer.Start(100)


    def init_plot(self):
        self.dpi = 100

        self.fig = Figure((3.0, 3.0), dpi=self.dpi)

        self.axes = self.fig.add_subplot(111)
        self.axes.set_axis_bgcolor('white')
        # self.axes.set_title('Very important random data', size=12)

        pylab.setp(self.axes.get_xticklabels(), fontsize=8)
        pylab.setp(self.axes.get_yticklabels(), fontsize=8)

        # plot the data as a line series, and save the reference
        # to the plotted line series
        #
        self.plot_data = self.axes.plot(self.data, linewidth=1, color=(0, 0, 0))[0]

    def on_redraw_timer(self, event):
        # if paused do not add data, but still redraw the plot
        # (to respond to scale modifications, grid change, etc.)
        #
        if not self.paused:
            self.data.append(self.datagen.next())

        self.draw_plot()

    def draw_plot(self):
        """ Redraws the plot
        """
        # when xmin is on auto, it "follows" xmax to produce a
        # sliding window effect. therefore, xmin is assigned after
        # xmax.
        #

        xmax = len(self.data) if len(self.data) > 50 else 50
        xmin = 0

        # if self.xmax_control.is_auto():
        #    xmax = len(self.data) if len(self.data) > 50 else 50
        #else:
        #    xmax = int(self.xmax_control.manual_value())

        #if self.xmin_control.is_auto():
        #    xmin = xmax - 50
        #else:
        #    xmin = int(self.xmin_control.manual_value())

        # for ymin and ymax, find the minimal and maximal values
        # in the data set and add a mininal margin.
        #
        # note that it's easy to change this scheme to the
        # minimal/maximal value in the current display, and not
        # the whole data set.
        #
        ymax = round(max(self.data), 0) + 1
        ymin = round(min(self.data), 0) - 1

        #if self.ymin_control.is_auto():
        #    ymin = round(min(self.data), 0) - 1
        #else:
        #    ymin = int(self.ymin_control.manual_value())

        #if self.ymax_control.is_auto():
        #    ymax = round(max(self.data), 0) + 1
        #else:
        #    ymax = int(self.ymax_control.manual_value())

        self.axes.set_xbound(lower=xmin, upper=xmax)
        self.axes.set_ybound(lower=ymin, upper=ymax)

        # anecdote: axes.grid assumes b=True if any other flag is
        # given even if b is set to False.
        # so just passing the flag into the first statement won't
        # work.
        #
        #if self.cb_grid.IsChecked():
        #    self.axes.grid(True, color='gray')
        #else:
        #    self.axes.grid(False)
        self.axes.grid(True, color='gray')

        # Using setp here is convenient, because get_xticklabels
        # returns a list over which one needs to explicitly
        # iterate, and setp already handles this.
        #
        pylab.setp(self.axes.get_xticklabels(),
                   visible=True)

        self.plot_data.set_xdata(np.arange(len(self.data)))
        self.plot_data.set_ydata(np.array(self.data))

        self.canvas.draw()
