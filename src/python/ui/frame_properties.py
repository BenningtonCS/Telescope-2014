# -*- coding: utf-8 -*-
"""

-------------------------
author: erick daniszewski
"""
import wx

from ui_utils import static_text_gen, radiobutton_gen


class Properties(wx.Frame):
    def __init__(self, parent, config_state_manager):
        wx.Frame.__init__(self, parent, id=wx.ID_ANY, title=u"Properties", pos=wx.DefaultPosition,
                          size=wx.Size(400, 550), style=wx.DEFAULT_FRAME_STYLE | wx.TAB_TRAVERSAL)

        self.csm = config_state_manager

        self.SetSizeHintsSz(wx.Size(400, 550), wx.Size(400, 550))
        main_sizer = wx.BoxSizer(wx.VERTICAL)

        self._create_status_bar()
        self._create_properties_window()
        self._create_dialog_buttons()

        main_sizer.Add(self.scroll_window, 1, wx.ALL | wx.EXPAND, 5)
        main_sizer.Add(self.button_sizer, 1, wx.ALIGN_BOTTOM | wx.EXPAND, 5)

        self.SetSizer(main_sizer)
        self.Layout()

        self._connect_events()

        self.Centre(wx.BOTH)

    def _create_status_bar(self):
        """

        :return:
        """
        self.status_bar = self.CreateStatusBar(1, wx.ST_SIZEGRIP, wx.ID_ANY)

    def _create_properties_window(self):
        """

        :return:
        """
        self.scroll_window = wx.ScrolledWindow(self, wx.ID_ANY, wx.DefaultPosition, wx.Size(-1, 350),
                                               wx.HSCROLL | wx.VSCROLL)

        self.scroll_window.SetScrollRate(5, 5)
        self.scroll_window.SetMinSize(wx.Size(-1, 450))
        main_sizer = wx.BoxSizer(wx.VERTICAL)

        self._create_srt_configuration()

        main_sizer.Add(self.srt_config_sizer, 1, wx.EXPAND, 5)

        self.scroll_window.SetSizer(main_sizer)
        self.scroll_window.Layout()

    def _create_dialog_buttons(self):
        """

        :return:
        """
        self.button_sizer = wx.StdDialogButtonSizer()

        self.button_ok = wx.Button(self, wx.ID_OK)
        self.button_apply = wx.Button(self, wx.ID_APPLY)
        self.button_cancel = wx.Button(self, wx.ID_CANCEL)

        self.button_sizer.AddButton(self.button_ok)
        self.button_sizer.AddButton(self.button_apply)
        self.button_sizer.AddButton(self.button_cancel)

        self.button_sizer.Realize()
        self.button_sizer.SetMinSize(wx.Size(-1, 50))

    def _create_srt_configuration(self):
        """

        :return:
        """
        self.srt_config_sizer = wx.StaticBoxSizer(wx.StaticBox(self.scroll_window, wx.ID_ANY, u"SRT Configuration"),
                                                  wx.VERTICAL)

        grid_sizer = wx.GridSizer(15, 2, 0, 0)

        self.text_antenna_sim = static_text_gen(self.scroll_window, u"Simulate Antenna")
        self.text_receiver_sim = static_text_gen(self.scroll_window, u"Simulate Receiver")
        self.text_fft_sim = static_text_gen(self.scroll_window, u"Simulate FFT")
        self.text_freq = static_text_gen(self.scroll_window, u"Frequency")
        self.text_num_freq = static_text_gen(self.scroll_window, u"Num Frequency")
        self.text_cal_mode = static_text_gen(self.scroll_window, u"Calibration Mode")
        self.text_azimuth = static_text_gen(self.scroll_window, u"Azimuth")
        self.text_elevation = static_text_gen(self.scroll_window, u"Elevation")
        self.text_az_lim = static_text_gen(self.scroll_window, u"Azimuth Limits")
        self.text_el_lim = static_text_gen(self.scroll_window, u"Elevation Limits")
        self.text_bandwidth = static_text_gen(self.scroll_window, u"Bandwidth")
        self.text_beamwidth = static_text_gen(self.scroll_window, u"Beamwidth")
        self.text_system_temp = static_text_gen(self.scroll_window, u"System Temp")
        self.text_tcal = static_text_gen(self.scroll_window, u"TCAL")
        self.text_nblock = static_text_gen(self.scroll_window, u"NBLOCK")

        self.text_antenna_sim.Wrap(-1)
        self.text_receiver_sim.Wrap(-1)
        self.text_fft_sim .Wrap(-1)
        self.text_freq.Wrap(-1)
        self.text_num_freq.Wrap(-1)
        self.text_cal_mode.Wrap(-1)
        self.text_azimuth.Wrap(-1)
        self.text_elevation.Wrap(-1)
        self.text_az_lim.Wrap(-1)
        self.text_el_lim.Wrap(-1)
        self.text_bandwidth.Wrap(-1)
        self.text_beamwidth.Wrap(-1)
        self.text_system_temp.Wrap(-1)
        self.text_tcal.Wrap(-1)
        self.text_nblock.Wrap(-1)

        sizer_antenna_sim = radiobutton_gen(self.scroll_window, [u"On", u"Off"])
        sizer_receiver_sim = radiobutton_gen(self.scroll_window, [u"On", u"Off"])
        sizer_fft_sim = radiobutton_gen(self.scroll_window, [u"On", u"Off"])

        self.freq_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.freq), wx.DefaultPosition, wx.DefaultSize, 0)
        self.freq_val.SetMaxLength(10)

        self.num_freq_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.num_freq), wx.DefaultPosition, wx.DefaultSize, 0)
        self.num_freq_val.SetMaxLength(8)

        cal_opt = [u"0", u"1", u"2", u"3", u"20"]
        self.cal_choice = wx.Choice(self.scroll_window, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, cal_opt, 0)
        self.cal_choice.SetSelection(self.csm.cal_mode)

        self.az_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.azimuth), wx.DefaultPosition, wx.DefaultSize, 0)
        self.el_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.elevation), wx.DefaultPosition, wx.DefaultSize, 0)
        self.az_lim_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.az_limits), wx.DefaultPosition, wx.DefaultSize, 0)
        self.el_lim_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.el_limits), wx.DefaultPosition, wx.DefaultSize, 0)
        self.bandw_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.bandwidth), wx.DefaultPosition, wx.DefaultSize, 0)
        self.beamw_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.beamwidth), wx.DefaultPosition, wx.DefaultSize, 0)
        self.stemp_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.sys_temp), wx.DefaultPosition, wx.DefaultSize, 0)
        self.tcal_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.tcal), wx.DefaultPosition, wx.DefaultSize, 0)
        self.nblock_val = wx.TextCtrl(self.scroll_window, wx.ID_ANY, str(self.csm.nblock), wx.DefaultPosition, wx.DefaultSize, 0)

        grid_sizer.Add(self.text_antenna_sim, 0, wx.ALL, 5)
        grid_sizer.Add(sizer_antenna_sim, 1, wx.EXPAND, 5)

        grid_sizer.Add(self.text_receiver_sim, 0, wx.ALL, 5)
        grid_sizer.Add(sizer_receiver_sim, 1, wx.EXPAND, 5)

        grid_sizer.Add(self.text_fft_sim, 0, wx.ALL, 5)
        grid_sizer.Add(sizer_fft_sim, 1, wx.EXPAND, 5)

        grid_sizer.Add(self.text_freq, 0, wx.ALL, 5)
        grid_sizer.Add(self.freq_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_num_freq, 0, wx.ALL, 5)
        grid_sizer.Add(self.num_freq_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_cal_mode, 0, wx.ALL, 5)
        grid_sizer.Add(self.cal_choice, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_azimuth, 0, wx.ALL, 5)
        grid_sizer.Add(self.az_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_elevation, 0, wx.ALL, 5)
        grid_sizer.Add(self.el_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_az_lim, 0, wx.ALL, 5)
        grid_sizer.Add(self.az_lim_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_el_lim, 0, wx.ALL, 5)
        grid_sizer.Add(self.el_lim_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_bandwidth, 0, wx.ALL, 5)
        grid_sizer.Add(self.bandw_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_beamwidth, 0, wx.ALL, 5)
        grid_sizer.Add(self.beamw_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_system_temp, 0, wx.ALL, 5)
        grid_sizer.Add(self.stemp_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_tcal, 0, wx.ALL, 5)
        grid_sizer.Add(self.tcal_val, 0, wx.ALL, 5)

        grid_sizer.Add(self.text_nblock, 0, wx.ALL, 5)
        grid_sizer.Add(self.nblock_val, 0, wx.ALL, 5)

        self.srt_config_sizer.Add(grid_sizer, 1, wx.EXPAND, 5)

    def update_state(self):
        """

        :return:
        """
        # Simulations
        self.csm.simulate_antenna = True  # FIXME
        self.csm.simulate_receiver = True  # FIXME
        self.csm.simulate_fft = True  # FIXME

        # Other
        self.csm.freq = self.freq_val.GetValue()
        self.csm.num_freq = self.num_freq_val.GetValue()
        self.csm.cal_mode = 2  ## FIXME
        self.csm.azimuth = self.az_val.GetValue()
        self.csm.elevation = self.el_val.GetValue()
        self.csm.az_limits = self.az_lim_val.GetValue()
        self.csm.el_limits = self.el_lim_val.GetValue()
        self.csm.bandwidth = self.bandw_val.GetValue()
        self.csm.beamwidth = self.beamw_val.GetValue()
        self.csm.sys_temp = self.stemp_val.GetValue()
        self.csm.tcal = self.tcal_val.GetValue()
        self.csm.nblock = self.nblock_val.GetValue()

    def _connect_events(self):
        """

        :return:
        """
        self.button_ok.Bind(wx.EVT_BUTTON, self.on_button_ok)
        self.button_apply.Bind(wx.EVT_BUTTON, self.on_button_apply)
        self.button_cancel.Bind(wx.EVT_BUTTON, self.on_button_cancel)

    def on_button_ok(self, event):
        """

        :return:
        """
        self.update_state()
        self.Close(True)

    def on_button_apply(self, event):
        """

        :return:
        """
        self.update_state()

    def on_button_cancel(self, event):
        """

        :return:
        """
        self.Close(True)

    def __del__(self):
        pass
