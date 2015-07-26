# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Dialog for setting azimuth and elevation.

-------------------------
author: erick daniszewski
"""
import wx


class AzelDialog(wx.Dialog):
    """
    Custom dialog for setting azimuth and elevation
    """
    def __init__(self, parent):
        """
        Constructor
        """
        wx.Dialog.__init__(self, parent, -1)

        self.Centre(wx.BOTH)
