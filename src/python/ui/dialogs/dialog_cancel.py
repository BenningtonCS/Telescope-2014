# -*- coding: utf-8 -*-
"""
Custom subclass of wx.Dialog for canceling actions

-------------------------
author: erick daniszewski
"""
import wx


class CancelDialog(wx.Dialog):
    """
    Custom dialog for action cancellations
    """
    def __init__(self, parent, csm):
        """
        Constructor
        """
        wx.Dialog.__init__(self, parent, -1)

        self.message_panel = wx.Panel(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)

        self.message = wx.StaticText(self.message_panel, -1, u"Cancel all current actions?")
        self.checkbox = wx.CheckBox(self.message_panel, -1, u"Do not show this message again.")

        text_sizer = wx.BoxSizer(wx.VERTICAL)
        text_sizer.Add(self.message, 0, wx.ALL, 5)
        text_sizer.AddSpacer((1, 10), 1, wx.EXPAND, 5)
        text_sizer.Add(self.checkbox, 0, wx.ALL, 5)
        self.message_panel.SetSizer(text_sizer)
        self.message_panel.Layout()
        text_sizer.Fit(self.message_panel)

        main_sizer = wx.BoxSizer(wx.VERTICAL)
        main_sizer.Add(self.message_panel, 1, wx.EXPAND | wx.ALL, 5)

        self.btns = self.CreateSeparatedButtonSizer(wx.OK | wx.CANCEL)
        main_sizer.Add(self.btns, 1, wx.EXPAND, 5)

        self.SetSizer(main_sizer)
        self.Layout()
        main_sizer.Fit(self)

        self.Centre(wx.BOTH)

    def GetCheckboxValue(self):
        return self.checkbox.GetValue()
