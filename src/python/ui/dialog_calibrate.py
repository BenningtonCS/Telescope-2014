# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Dialog for srt calibration

-------------------------
author: erick daniszewski
"""
import wx


class CalibrateDialog(wx.Dialog):
    """
    Custom dialog for calibration
    """
    def __init__(self, parent):
        """
        Constructor
        """
        wx.Dialog.__init__(self, parent, -1)

        self.Centre(wx.BOTH)
