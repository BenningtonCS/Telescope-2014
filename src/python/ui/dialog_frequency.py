# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Dialog for setting frequency

-------------------------
author: erick daniszewski
"""
import wx


class FrequencyDialog(wx.Dialog):
    """
    Custom dialog for setting frequency
    """
    def __init__(self, parent):
        """
        Constructor
        """
        wx.Dialog.__init__(self, parent, -1)

        self.Centre(wx.BOTH)
