# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Dialog for recording data

-------------------------
author: erick daniszewski
"""
import wx


class RecordDialog(wx.Dialog):
    """
    Custom dialog for data recording
    """
    def __init__(self, parent):
        """
        Constructor
        """
        wx.Dialog.__init__(self, parent, -1)

        self.Centre(wx.BOTH)
