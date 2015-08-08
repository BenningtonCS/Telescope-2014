# -*- coding: utf-8 -*-
"""
Entry point for the UI

-------------------------
author: erick daniszewski
"""
import wx

from ui.windows.window_main import MainWindow


class SRTWindow(MainWindow):
    """

    """
    def __init__(self, parent):
        MainWindow.__init__(self, parent)


class SRT(wx.App):
    """

    """
    def OnInit(self):
        self.main_frame = SRTWindow(None)
        self.main_frame.Show()
        self.SetTopWindow(self.main_frame)
        return True


app = SRT()
app.MainLoop()
