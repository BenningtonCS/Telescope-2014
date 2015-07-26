# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Dialog for setting offsets

-------------------------
author: erick daniszewski
"""
import wx


class OffsetDialog(wx.Dialog):
    """
    Custom dialog for setting offsets
    """
    def __init__(self, parent):
        """
        Constructor
        """
        wx.Dialog.__init__(self, parent, -1)

        self.Centre(wx.BOTH)
