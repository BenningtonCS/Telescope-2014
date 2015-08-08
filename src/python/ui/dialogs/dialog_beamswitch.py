# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Dialog for beam switching

-------------------------
author: erick daniszewski
"""
import wx


class BeamswitchDialog(wx.Dialog):
    """
    Custom dialog for beam switching
    """
    def __init__(self, parent):
        """
        Constructor
        """
        wx.Dialog.__init__(self, parent, -1)

        self.Centre(wx.BOTH)
