# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Dialog for setting frequency

-------------------------
author: erick daniszewski
"""
import wx


class FrequencyDialog(wx.TextEntryDialog):
    """
    Custom dialog for setting frequency
    """
    def __init__(self, parent, csm):
        """
        Constructor
        """
        message = 'Current Frequency:\t{}\n\nNew Frequency:'.format(csm.freq)
        title = "Frequency"

        wx.TextEntryDialog.__init__(self, parent, message, title)

        self.Centre(wx.BOTH)
