# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Panel for handling graphs

-------------------------
author: erick daniszewski
"""
import wx
import numpy as np
import matplotlib
matplotlib.use('WXAgg')
from matplotlib.figure import Figure
from matplotlib.backends.backend_wxagg import FigureCanvasWxAgg as FigureCanvas


class GraphPanel(wx.Panel):
    """
    An instance of wx.Panel which contains a graph
    """
    def __init__(self, parent):
        """
        Constructor
        """
        wx.Panel.__init__(self, parent, -1)

        # TODO implement
