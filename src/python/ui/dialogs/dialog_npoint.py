# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Dialog for npoint

-------------------------
author: erick daniszewski
"""
import wx


class NpointDialog(wx.Dialog):
    """
    Custom dialog for npoint
    """
    def __init__(self, parent):
        """
        Constructor
        """
        wx.Dialog.__init__(self, parent, -1)

        self.Centre(wx.BOTH)
