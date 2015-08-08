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
    def __init__(self, parent, csm):
        """
        Constructor
        """
        self.csm = csm

        wx.Dialog.__init__(self, parent, -1)

        self._current_az_offset()
        self._current_el_offset()
        self._new_az_offset()
        self._new_el_offset()

        line = wx.StaticLine(self, -1, style=wx.LI_HORIZONTAL)

        main_sizer = wx.BoxSizer(wx.VERTICAL)
        main_sizer.Add(self.current_az_sizer, 0, wx.EXPAND, 5)
        main_sizer.Add(self.current_el_sizer, 0, wx.EXPAND, 5)
        main_sizer.Add(line, 0, wx.ALL, 2)
        main_sizer.Add(self.new_az_sizer, 0, wx.EXPAND, 5)
        main_sizer.Add(self.new_el_sizer, 0, wx.EXPAND, 5)

        self.btns = self.CreateSeparatedButtonSizer(wx.OK | wx.CANCEL)
        main_sizer.Add(self.btns, 1, wx.EXPAND, 5)

        self.SetSizer(main_sizer)
        self.Layout()
        main_sizer.Fit(self)

        self.Centre(wx.BOTH)

    def _current_az_offset(self):
        self.current_az_sizer = wx.BoxSizer(wx.HORIZONTAL)

        self.curr_az_lab = wx.StaticText(self, -1, 'Current AZ offset:')
        self.curr_az_val = wx.StaticText(self, -1, str(self.csm.azimuth))

        self.current_az_sizer.Add(self.curr_az_lab, 0, wx.ALL, 5)
        self.current_az_sizer.Add(self.curr_az_val, 0, wx.ALL, 5)

    def _current_el_offset(self):
        self.current_el_sizer = wx.BoxSizer(wx.HORIZONTAL)

        self.curr_el_lab = wx.StaticText(self, -1, 'Current EL offset:')
        self.curr_el_val = wx.StaticText(self, -1, str(self.csm.elevation))

        self.current_el_sizer.Add(self.curr_el_lab, 0, wx.ALL, 5)
        self.current_el_sizer.Add(self.curr_el_val, 0, wx.ALL, 5)

    def _new_az_offset(self):
        self.new_az_sizer = wx.BoxSizer(wx.HORIZONTAL)

        self.new_az_lab = wx.StaticText(self, -1, 'Set AZ offset:')
        self.new_az_val = wx.TextCtrl(self, -1, wx.EmptyString, style=wx.TE_LEFT)

        self.new_az_sizer.Add(self.new_az_lab, 0, wx.ALL, 5)
        self.new_az_sizer.Add(self.new_az_val, 0, wx.ALL, 5)

    def _new_el_offset(self):
        self.new_el_sizer = wx.BoxSizer(wx.HORIZONTAL)

        self.new_el_lab = wx.StaticText(self, -1, 'Set EL offset:')
        self.new_el_val = wx.TextCtrl(self, -1, wx.EmptyString, style=wx.TE_LEFT)

        self.new_el_sizer.Add(self.new_el_lab, 0, wx.ALL, 5)
        self.new_el_sizer.Add(self.new_el_val, 0, wx.ALL, 5)

    def GetAzValue(self):
        return self.new_az_val.GetValue()

    def GetElValue(self):
        return self.new_el_val.GetValue()