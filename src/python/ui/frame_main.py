# -*- coding: utf-8 -*-
"""

-------------------------
author: erick daniszewski
"""
import os

import wx

from configuration import ConfigStateManager

from frame_properties import Properties
from dialog_cancel import CancelDialog
from dialog_calibrate import CalibrateDialog
from dialog_offset import OffsetDialog
from dialog_record import RecordDialog
from dialog_azel import AzelDialog
from dialog_beamswitch import BeamswitchDialog
from dialog_npoint import NpointDialog
from dialog_frequency import FrequencyDialog
from command_interpreter import CommandInterpreter
from window_log import LogWindow
from ui_logging import UILogger

from ui_utils import checked_menu_item, norm_menu_item, status_panel, status_label, status_value, \
    handle_window_update, hide_window


class MainFrame(wx.Frame):
    def __init__(self, parent):

        # Get an instance of a configuration state manager and load in the existing configurations
        self.csm = ConfigStateManager()
        self.csm.load_session_state()

        wx.Frame.__init__(self, parent, id=wx.ID_ANY, size=wx.Size(self.csm.app_width, self.csm.app_height),
                          title=u"srt control", pos=wx.DefaultPosition, style=wx.DEFAULT_FRAME_STYLE | wx.TAB_TRAVERSAL)

        self.SetSizeHintsSz(wx.DefaultSize, wx.DefaultSize)

        # Create the splitter window
        self.splitter_window = wx.SplitterWindow(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.SP_3DSASH | wx.SP_LIVE_UPDATE)
        self.splitter_window.Bind(wx.EVT_IDLE, self.splitter_window_on_idle)

        self.top_panel = wx.Panel(self.splitter_window, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)
        self.bottom_panel = wx.Panel(self.splitter_window, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)

        # Create Application windows
        self._create_logging_window()
        self._create_cmd_input_form()
        self._create_status_window()
        self._create_webcam_window()
        self._create_skymap_window()
        self._create_graph_window()

        # Add elements to the top panel of the splitter window
        top_panel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        top_panel_sizer.Add(self.graph_container, 1, wx.EXPAND, 5)
        top_panel_sizer.Add(self.status_window, 1, wx.ALL | wx.EXPAND, 5)

        self.top_panel.SetSizer(top_panel_sizer)
        self.top_panel.Layout()
        top_panel_sizer.Fit(self.top_panel)

        # Add elements to the bottom panel of the splitter window
        bottom_panel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        bottom_panel_sizer.Add(self.skymap, 1, wx.EXPAND | wx.ALL, 5)
        bottom_panel_sizer.Add(self.webcam_container, 1, wx.EXPAND | wx.ALL, 5)

        self.bottom_panel.SetSizer(bottom_panel_sizer)
        self.bottom_panel.Layout()
        bottom_panel_sizer.Fit(self.bottom_panel)

        # Add top and bottom panes to splitter window
        self.splitter_window.SplitHorizontally(self.top_panel, self.bottom_panel, 0)
        splitter_wrapper_sizer = wx.BoxSizer(wx.VERTICAL)
        splitter_wrapper_sizer.Add(self.splitter_window, 1, wx.EXPAND, 2)

        # Add elements to the main sizer
        main_sizer = wx.BoxSizer(wx.VERTICAL)
        main_sizer.Add(self.log_box, 0, wx.ALL | wx.EXPAND, 2)
        main_sizer.Add(self.cmd_input, 0, wx.ALL | wx.EXPAND, 2)
        main_sizer.Add(splitter_wrapper_sizer, 1, wx.EXPAND, 5)

        self.SetSizer(main_sizer)
        self.Layout()

        # Create application menu bar
        self._create_menubar()
        self.SetMenuBar(self.menubar)

        # Create application toolbar
        self._create_toolbar()
        self._create_status_bar()

        self.Centre(wx.BOTH)

        # Connect application events
        self._connect_events()

        # Get instance of command interpreter
        self.cmd_interpreter = CommandInterpreter(self, self.log_box)

    def __del__(self):
        """
        Before deleting the class, save the session state to file.

        :return:
        """
        self.csm.save_session_state()

    def _create_status_bar(self):
        """

        :return:
        """
        self.status_bar = self.CreateStatusBar(1, wx.ST_SIZEGRIP, wx.ID_ANY)

    def _create_toolbar(self):
        """

        :return:
        """
        self.toolbar = self.CreateToolBar(wx.TB_HORIZONTAL | wx.TB_TEXT, wx.ID_ANY)

        self.toolid_clear = wx.NewId()
        self.toolid_cancel = wx.NewId()
        self.toolid_stow = wx.NewId()
        self.toolid_azel = wx.NewId()
        self.toolid_npoint = wx.NewId()
        self.toolid_beamsw = wx.NewId()
        self.toolid_freq = wx.NewId()
        self.toolid_offset = wx.NewId()
        self.toolid_record = wx.NewId()
        self.toolid_command = wx.NewId()
        self.toolid_cal = wx.NewId()
        self.toolid_config = wx.NewId()

        self.__add_label_tool(self.toolid_clear, u"Clear", u".resources/img/CLEAR.png", u"Clear the screen")
        self.__add_label_tool(self.toolid_cancel, u"Cancel", u".resources/img/CANCEL.png", u"Cancel current actions")
        self.toolbar.AddSeparator()
        self.__add_label_tool(self.toolid_stow, u"Stow", u".resources/img/STOW.png", u"Stow the telescope")
        self.__add_label_tool(self.toolid_azel, u"AzEl", u".resources/img/AZEL.png", u"Set Azimuth and Elevation")
        self.__add_label_tool(self.toolid_npoint, u"NPoint", u".resources/img/NPOINT.png", u"set npoint")
        self.__add_label_tool(self.toolid_beamsw, u"BeamSW", u".resources/img/BEAMSW.png", u"set beamsw")
        self.__add_label_tool(self.toolid_freq, u"Frequency", u".resources/img/FREQ.png", u"set frequency")
        self.__add_label_tool(self.toolid_offset, u"Offset", u".resources/img/OFFSET.png", u"set offset")
        self.__add_label_tool(self.toolid_record, u"Record", u".resources/img/REC.png", u"record data")
        self.__add_label_tool(self.toolid_command, u"Command", u".resources/img/CMD.png", u"execute command file")
        self.__add_label_tool(self.toolid_cal, u"Calibrate", u".resources/img/CAL.png", u"calibrate telescope")
        self.toolbar.AddSeparator()
        self.__add_label_tool(self.toolid_config, u"Configure", u".resources/img/CONFIG.png", u"Configure SRT")

        self.toolbar.Realize()

    def __add_label_tool(self, _id, title, bitmap_url, tooltip):
        """

        :param _id:
        :param title:
        :type title: unicode
        :param bitmap_url:
        :type bitmap_url: unicode
        :param tooltip:
        :type tooltip: unicode
        :return:
        """
        self.toolbar.AddLabelTool(_id, title, wx.Bitmap(bitmap_url, wx.BITMAP_TYPE_ANY),
                                  wx.NullBitmap, wx.ITEM_NORMAL, tooltip, tooltip)

    def _create_menubar(self):
        """

        :return:
        """
        self.menubar = wx.MenuBar(0)

        self.__menu_file()
        self.__menu_view()
        self.__menu_command()
        self.__menu_data()
        self.__menu_config()
        self.__menu_help()

    def __menu_file(self):
        self.menu_file = wx.Menu()

        self.m_file_quit = norm_menu_item(self.menu_file, u"Quit" + u"\t" + u"CTRL+Q", u"quit the srtn control software")

        self.menu_file.AppendItem(self.m_file_quit)

        self.menubar.Append(self.menu_file, u"File")

    def __menu_view(self):
        self.menu_view = wx.Menu()

        self.__menu_view_windows()
        self.__menu_view_status()
        self.__menu_view_theme()

        self.menubar.Append(self.menu_view, u"View")

    def __menu_view_windows(self):
        self.m_view_windows = wx.Menu()

        self.m_view_windows_log = checked_menu_item(self.m_view_windows, u"Log Output", u"Show/Hide the Log Window")
        self.m_view_windows_skyview = checked_menu_item(self.m_view_windows, u"Sky View", u"Show/Hide the Sky Map Window")
        self.m_view_windows_webcam = checked_menu_item(self.m_view_windows, u"Webcam", u"Show/Hide the Webcam Window")
        self.m_view_windows_status = checked_menu_item(self.m_view_windows, u"Status Area", u"Show/Hide the Status Area")
        self.m_view_windows_graph = checked_menu_item(self.m_view_windows, u"Graphs", u"Show/View Graphs")
        self.m_view_windows_cmd_input = checked_menu_item(self.m_view_windows, u"Command Input", u"Show/Hide the Command Input field")

        self.m_view_windows.AppendItem(self.m_view_windows_log)
        self.m_view_windows.AppendItem(self.m_view_windows_skyview)
        self.m_view_windows.AppendItem(self.m_view_windows_webcam)
        self.m_view_windows.AppendItem(self.m_view_windows_status)
        self.m_view_windows.AppendItem(self.m_view_windows_graph)
        self.m_view_windows.AppendItem(self.m_view_windows_cmd_input)

        self.m_view_windows_log.Check(self.csm.w_view_log)
        self.m_view_windows_skyview.Check(self.csm.w_view_sky)
        self.m_view_windows_webcam.Check(self.csm.w_view_webcam)
        self.m_view_windows_status.Check(self.csm.w_view_status)
        self.m_view_windows_graph.Check(self.csm.w_view_graphs)
        self.m_view_windows_cmd_input.Check(self.csm.w_view_cmd_input)

        self.menu_view.AppendSubMenu(self.m_view_windows, u"Windows")

    def __menu_view_status(self):
        self.m_view_status = wx.Menu()

        self.m_view_status_location = checked_menu_item(self.m_view_status, u"Location", wx.EmptyString)
        self.m_view_status_cmd = checked_menu_item(self.m_view_status, u"Cmd", wx.EmptyString)
        self.m_view_status_azel = checked_menu_item(self.m_view_status, u"AzEl", wx.EmptyString)
        self.m_view_status_offsets = checked_menu_item(self.m_view_status, u"Offsets", wx.EmptyString)
        self.m_view_status_ra = checked_menu_item(self.m_view_status, u"RA", wx.EmptyString)
        self.m_view_status_time = checked_menu_item(self.m_view_status, u"Time", wx.EmptyString)
        self.m_view_status_source = checked_menu_item(self.m_view_status, u"Source", wx.EmptyString)
        self.m_view_status_vlsr = checked_menu_item(self.m_view_status, u"VLSR", wx.EmptyString)
        self.m_view_status_integ = checked_menu_item(self.m_view_status, u"Integral", wx.EmptyString)
        self.m_view_status_freq = checked_menu_item(self.m_view_status, u"Frequency", wx.EmptyString)
        self.m_view_status_if = checked_menu_item(self.m_view_status, u"I.F.", wx.EmptyString)
        self.m_view_status_bw = checked_menu_item(self.m_view_status, u"BW", wx.EmptyString)
        self.m_view_status_resol = checked_menu_item(self.m_view_status, u"Resolution", wx.EmptyString)

        self.m_view_status.AppendItem(self.m_view_status_location)
        self.m_view_status.AppendItem(self.m_view_status_cmd)
        self.m_view_status.AppendItem(self.m_view_status_azel)
        self.m_view_status.AppendItem(self.m_view_status_offsets)
        self.m_view_status.AppendItem(self.m_view_status_ra)
        self.m_view_status.AppendItem(self.m_view_status_time)
        self.m_view_status.AppendItem(self.m_view_status_source)
        self.m_view_status.AppendItem(self.m_view_status_vlsr)
        self.m_view_status.AppendItem(self.m_view_status_integ)
        self.m_view_status.AppendItem(self.m_view_status_freq)
        self.m_view_status.AppendItem(self.m_view_status_if)
        self.m_view_status.AppendItem(self.m_view_status_bw)
        self.m_view_status.AppendItem(self.m_view_status_resol)

        self.m_view_status_location.Check(self.csm.s_view_location)
        self.m_view_status_cmd.Check(self.csm.s_view_command)
        self.m_view_status_azel.Check(self.csm.s_view_azel)
        self.m_view_status_offsets.Check(self.csm.s_view_offsets)
        self.m_view_status_ra.Check(self.csm.s_view_ra)
        self.m_view_status_time.Check(self.csm.s_view_time)
        self.m_view_status_source.Check(self.csm.s_view_source)
        self.m_view_status_vlsr.Check(self.csm.s_view_vlsr)
        self.m_view_status_integ.Check(self.csm.s_view_integ)
        self.m_view_status_freq.Check(self.csm.s_view_freq)
        self.m_view_status_if.Check(self.csm.s_view_if)
        self.m_view_status_bw.Check(self.csm.s_view_bw)
        self.m_view_status_resol.Check(self.csm.s_view_resol)

        self.menu_view.AppendSubMenu(self.m_view_status, u"Status")

    def __menu_view_theme(self):
        self.m_view_theme = wx.Menu()

        self.m_view_theme_default = checked_menu_item(self.m_view_theme, u"Default", wx.EmptyString)
        self.m_view_theme_night = checked_menu_item(self.m_view_theme, u"Night Mode", wx.EmptyString)

        self.m_view_theme.AppendItem(self.m_view_theme_default)
        self.m_view_theme.AppendItem(self.m_view_theme_night)

        self.m_view_theme_default.Check(self.csm.t_view_default)
        self.m_view_theme_night.Check(self.csm.t_view_night)

        self.menu_view.AppendSubMenu(self.m_view_theme, u"Themes")

    def __menu_command(self):
        self.m_command = wx.Menu()

        self.m_command_halt = norm_menu_item(self.m_command, u"Halt" + u"\t" + u"CTRL+H", wx.EmptyString)
        self.m_command_stow = norm_menu_item(self.m_command, u"Stow", wx.EmptyString)
        self.m_command_azel = norm_menu_item(self.m_command, u"AzEl", wx.EmptyString)
        self.m_command_npoint = norm_menu_item(self.m_command, u"NPoint", wx.EmptyString)
        self.m_command_beamsw = norm_menu_item(self.m_command, u"Beam Switch", wx.EmptyString)
        self.m_command_freq = norm_menu_item(self.m_command, u"Frequency", wx.EmptyString)
        self.m_command_offset = norm_menu_item(self.m_command, u"Offset", wx.EmptyString)
        self.m_command_cal = norm_menu_item(self.m_command, u"Calibrate", wx.EmptyString)

        self.m_command.AppendItem(self.m_command_halt)
        self.m_command.AppendSeparator()
        self.m_command.AppendItem(self.m_command_stow)
        self.m_command.AppendItem(self.m_command_azel)
        self.m_command.AppendItem(self.m_command_npoint)
        self.m_command.AppendItem(self.m_command_beamsw)
        self.m_command.AppendItem(self.m_command_freq)
        self.m_command.AppendItem(self.m_command_offset)
        self.m_command.AppendItem(self.m_command_cal)

        self.menubar.Append(self.m_command, u"Command")

    def __menu_data(self):
        self.m_data = wx.Menu()

        self.m_data_clear = norm_menu_item(self.m_data, u"Clear", wx.EmptyString)
        self.m_data_record = norm_menu_item(self.m_data, u"Record" + u"\t" + u"CTRL+R", wx.EmptyString)

        self.m_data.AppendItem(self.m_data_clear)
        self.m_data.AppendItem(self.m_data_record)
        self.m_data.AppendSeparator()

        self.__menu_data_ouput()

        self.m_data.AppendSubMenu(self.m_data_outformat, u"Output Format")

        self.menubar.Append(self.m_data, u"Data")

    def __menu_data_ouput(self):
        self.m_data_outformat = wx.Menu()

        self.m_data_outformat_CSV = wx.MenuItem(self.m_data_outformat, wx.ID_ANY, u"CSV", wx.EmptyString, wx.ITEM_RADIO)
        self.m_data_outformat_JSON = wx.MenuItem(self.m_data_outformat, wx.ID_ANY, u"JSON", wx.EmptyString, wx.ITEM_RADIO)
        self.m_data_outformat_XML = wx.MenuItem(self.m_data_outformat, wx.ID_ANY, u"XML", wx.EmptyString, wx.ITEM_RADIO)

        self.m_data_outformat.AppendItem(self.m_data_outformat_CSV)
        self.m_data_outformat.AppendItem(self.m_data_outformat_JSON)
        self.m_data_outformat.AppendItem(self.m_data_outformat_XML)

    def __menu_config(self):
        self.m_config = wx.Menu()

        self.m_config_editconfig = norm_menu_item(self.m_config, u"Edit Configurations...", wx.EmptyString)
        self.m_config_resetconfig = norm_menu_item(self.m_config, 'Reset to default', wx.EmptyString)

        self.__menu_config_update()

        self.m_config.AppendItem(self.m_config_editconfig)
        self.m_config.AppendItem(self.m_config_resetconfig)
        self.m_config.AppendSubMenu(self.m_config_update, u"Update")

        self.menubar.Append(self.m_config, u"Configure")

    def __menu_config_update(self):
        self.m_config_update = wx.Menu()

        self.__menu_config_update_location()

        self.m_config_update.AppendSubMenu(self.m_config_update_location, u"Location")

    def __menu_config_update_location(self):
        self.m_config_update_location = wx.Menu()

        self.m_config_update_location_auto = norm_menu_item(self.m_config_update_location, u"Auto Update", wx.EmptyString)
        self.m_config_update_location_manual = norm_menu_item(self.m_config_update_location, u"Manual...", wx.EmptyString)

        self.m_config_update_location.AppendItem(self.m_config_update_location_auto)
        self.m_config_update_location.AppendItem(self.m_config_update_location_manual)

    def __menu_help(self):
        self.m_help = wx.Menu()

        self.m_help_updates = norm_menu_item(self.m_help, u"Check for Updates...", wx.EmptyString)
        self.m_help_documentation = norm_menu_item(self.m_help, u"Documentation...", wx.EmptyString)

        self.m_help.AppendItem(self.m_help_updates)
        self.m_help.AppendItem(self.m_help_documentation)

        self.menubar.Append(self.m_help, u"Help")

    def _create_status_window(self):
        """

        :return:
        """
        self.status_window = wx.ScrolledWindow(self.top_panel, wx.ID_ANY, wx.DefaultPosition, wx.Size(225, -1), wx.VSCROLL)

        self.status_window.SetScrollRate(5, 5)
        self.status_window.SetMinSize(wx.Size(200, 200))
        self.status_window.SetMaxSize(wx.Size(225, 250))

        status = wx.StaticBoxSizer(wx.StaticBox(self.status_window, wx.ID_ANY, u"SRT Status"), wx.VERTICAL)

        # Location
        # --------------------------------------------------
        self.m_pstatus_location = status_panel(self.status_window, wx.Size(-1, 12))
        self.status_location_lab = status_label(self.m_pstatus_location, u"location:")
        self.status_location_val = status_value(self.m_pstatus_location, str(self.csm.location))

        loc_sizer = wx.BoxSizer(wx.HORIZONTAL)
        loc_sizer.Add(self.status_location_lab, 0, wx.LEFT | wx.RIGHT, 2)
        loc_sizer.Add(self.status_location_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_location.SetSizer(loc_sizer)
        self.m_pstatus_location.Layout()
        status.Add(self.m_pstatus_location, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_location.Show() if self.csm.s_view_location else self.m_pstatus_location.Hide()

        # Command
        # --------------------------------------------------
        self.m_pstatus_cmd = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_cmd_lab = status_label(self.m_pstatus_cmd, u"cmd:")
        self.status_cmd_val = status_value(self.m_pstatus_cmd, u"--")

        cmd_sizer = wx.BoxSizer(wx.HORIZONTAL)
        cmd_sizer.Add(self.status_cmd_lab, 0, wx.LEFT | wx.RIGHT, 2)
        cmd_sizer.Add(self.status_cmd_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_cmd.SetSizer(cmd_sizer)
        self.m_pstatus_cmd.Layout()
        status.Add(self.m_pstatus_cmd, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_cmd.Show() if self.csm.s_view_command else self.m_pstatus_cmd.Hide()

        # AzEl
        # --------------------------------------------------
        self.m_pstatus_azel = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_azel_lab = status_label(self.m_pstatus_azel, u"azel:")
        self.status_azel_val = status_value(self.m_pstatus_azel, str(self.csm.azimuth) + ' ' + str(self.csm.elevation))

        azel_sizer = wx.BoxSizer(wx.HORIZONTAL)
        azel_sizer.Add(self.status_azel_lab, 0, wx.LEFT | wx.RIGHT, 2)
        azel_sizer.Add(self.status_azel_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_azel.SetSizer(azel_sizer)
        self.m_pstatus_azel.Layout()
        status.Add(self.m_pstatus_azel, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_azel.Show() if self.csm.s_view_azel else self.m_pstatus_azel.Hide()

        # Offsets
        # --------------------------------------------------
        self.m_pstatus_offset = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_offset_lab = status_label(self.m_pstatus_offset, u"offsets:")
        self.status_offset_val = status_value(self.m_pstatus_offset, u"-- ")

        offset_sizer = wx.BoxSizer(wx.HORIZONTAL)
        offset_sizer.Add(self.status_offset_lab, 0, wx.LEFT | wx.RIGHT, 2)
        offset_sizer.Add(self.status_offset_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_offset.SetSizer(offset_sizer)
        self.m_pstatus_offset.Layout()
        status.Add(self.m_pstatus_offset, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_offset.Show() if self.csm.s_view_offsets else self.m_pstatus_offset.Hide()

        # RA
        # --------------------------------------------------
        self.m_pstatus_ra = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_ra_lab = status_label(self.m_pstatus_ra, u"ra:")
        self.status_ra_val = status_value(self.m_pstatus_ra, u"-- ")

        ra_sizer = wx.BoxSizer(wx.HORIZONTAL)
        ra_sizer.Add(self.status_ra_lab, 0, wx.LEFT | wx.RIGHT, 2)
        ra_sizer.Add(self.status_ra_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_ra.SetSizer(ra_sizer)
        self.m_pstatus_ra.Layout()
        status.Add(self.m_pstatus_ra, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_ra.Show() if self.csm.s_view_ra else self.m_pstatus_ra.Hide()

        # Time
        # --------------------------------------------------
        self.m_pstatus_time = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_time_lab = status_label(self.m_pstatus_time, u"time:")
        self.status_time_val = status_value(self.m_pstatus_time, u"-- ")

        time_sizer = wx.BoxSizer(wx.HORIZONTAL)
        time_sizer.Add(self.status_time_lab, 0, wx.LEFT | wx.RIGHT, 2)
        time_sizer.Add(self.status_time_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_time.SetSizer(time_sizer)
        self.m_pstatus_time.Layout()
        status.Add(self.m_pstatus_time, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_time.Show() if self.csm.s_view_time else self.m_pstatus_time.Hide()

        # Source
        # --------------------------------------------------
        self.m_pstatus_source = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_source_lab = status_label(self.m_pstatus_source, u"source:")
        self.status_source_val = status_value(self.m_pstatus_source, u"-- ")

        source_sizer = wx.BoxSizer(wx.HORIZONTAL)
        source_sizer.Add(self.status_source_lab, 0, wx.LEFT | wx.RIGHT, 2)
        source_sizer.Add(self.status_source_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_source.SetSizer(source_sizer)
        self.m_pstatus_source.Layout()
        status.Add(self.m_pstatus_source, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_source.Show() if self.csm.s_view_source else self.m_pstatus_source.Hide()

        # Vlsr
        # --------------------------------------------------
        self.m_pstatus_vlsr = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_vlsr_lab = status_label(self.m_pstatus_vlsr, u"vlsr:")
        self.status_vlsr_val = status_value(self.m_pstatus_vlsr, u"-- ")

        vlsr_sizer = wx.BoxSizer(wx.HORIZONTAL)
        vlsr_sizer.Add(self.status_vlsr_lab, 0, wx.LEFT | wx.RIGHT, 2)
        vlsr_sizer.Add(self.status_vlsr_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_vlsr.SetSizer(vlsr_sizer)
        self.m_pstatus_vlsr.Layout()
        status.Add(self.m_pstatus_vlsr, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_vlsr.Show() if self.csm.s_view_vlsr else self.m_pstatus_vlsr.Hide()

        # Integ
        # --------------------------------------------------
        self.m_pstatus_integ = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_integ_lab = status_label(self.m_pstatus_integ, u"integ:")
        self.status_integ_val = status_value(self.m_pstatus_integ, u"-- ")

        integ_sizer = wx.BoxSizer(wx.HORIZONTAL)
        integ_sizer.Add(self.status_integ_lab, 0, wx.LEFT | wx.RIGHT, 2)
        integ_sizer.Add(self.status_integ_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_integ.SetSizer(integ_sizer)
        self.m_pstatus_integ.Layout()
        status.Add(self.m_pstatus_integ, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_integ.Show() if self.csm.s_view_integ else self.m_pstatus_integ.Hide()

        # Frequency
        # --------------------------------------------------
        self.m_pstatus_freq = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_freq_lab = status_label(self.m_pstatus_freq, u"freq:")
        self.status_freq_val = status_value(self.m_pstatus_freq, u"-- ")

        freq_sizer = wx.BoxSizer(wx.HORIZONTAL)
        freq_sizer.Add(self.status_freq_lab, 0, wx.LEFT | wx.RIGHT, 2)
        freq_sizer.Add(self.status_freq_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_freq.SetSizer(freq_sizer)
        self.m_pstatus_freq.Layout()
        status.Add(self.m_pstatus_freq, 1, wx.ALL | wx.EXPAND, 2)

        self.m_pstatus_freq.Show() if self.csm.s_view_freq else self.m_pstatus_freq.Hide()

        # I.F.
        # --------------------------------------------------
        self.m_pstatus_if = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_if_lab = status_label(self.m_pstatus_if, u"if:")
        self.status_if_val = status_value(self.m_pstatus_if, u"-- ")

        if_sizer = wx.BoxSizer(wx.HORIZONTAL)
        if_sizer.Add(self.status_if_lab, 0, wx.LEFT | wx.RIGHT, 2)
        if_sizer.Add(self.status_if_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_if.SetSizer(if_sizer)
        self.m_pstatus_if.Layout()
        status.Add(self.m_pstatus_if, 1, wx.EXPAND | wx.ALL, 2)

        self.m_pstatus_if.Show() if self.csm.s_view_if else self.m_pstatus_if.Hide()

        # BW
        # --------------------------------------------------
        self.m_pstatus_bw = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_bw_lab = status_label(self.m_pstatus_bw, u"bw:")
        self.status_bw_val = status_value(self.m_pstatus_bw, u"-- ")

        bw_sizer = wx.BoxSizer(wx.HORIZONTAL)
        bw_sizer.Add(self.status_bw_lab, 0, wx.LEFT | wx.RIGHT, 2)
        bw_sizer.Add(self.status_bw_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_bw.SetSizer(bw_sizer)
        self.m_pstatus_bw.Layout()
        status.Add(self.m_pstatus_bw, 1, wx.EXPAND | wx.ALL, 2)

        self.m_pstatus_bw.Show() if self.csm.s_view_bw else self.m_pstatus_bw.Hide()

        # Resolution
        # --------------------------------------------------
        self.m_pstatus_resol = status_panel(self.status_window, wx.Size(-1, 10))
        self.status_resol_lab = status_label(self.m_pstatus_resol, u"resol:")
        self.status_resol_val = status_value(self.m_pstatus_resol, u"-- ")

        resol_sizer = wx.BoxSizer(wx.HORIZONTAL)
        resol_sizer.Add(self.status_resol_lab, 0, wx.LEFT | wx.RIGHT, 2)
        resol_sizer.Add(self.status_resol_val, 0, wx.LEFT | wx.RIGHT, 2)

        self.m_pstatus_resol.SetSizer(resol_sizer)
        self.m_pstatus_resol.Layout()
        status.Add(self.m_pstatus_resol, 1, wx.EXPAND | wx.ALL, 2)

        self.m_pstatus_resol.Show() if self.csm.s_view_resol else self.m_pstatus_resol.Hide()

        self.status_window.SetSizer(status)
        self.status_window.Layout()

    def _create_graph_window(self):
        """

        :return:
        """
        self.graph_container = wx.Panel(self.top_panel, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)
        graphs_sizer = wx.BoxSizer(wx.HORIZONTAL)

        self.m_panel5 = wx.Panel(self.top_panel, wx.ID_ANY, wx.DefaultPosition, wx.Size(-1, -1), wx.TAB_TRAVERSAL)
        graphs_sizer.Add(self.m_panel5, 1, wx.EXPAND | wx.ALL, 5)

        side_graphs_sizer = wx.BoxSizer(wx.VERTICAL)

        self.m_panel21 = wx.Panel(self.top_panel, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)
        side_graphs_sizer.Add(self.m_panel21, 1, wx.EXPAND | wx.ALL, 5)

        self.m_panel22 = wx.Panel(self.top_panel, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)
        side_graphs_sizer.Add(self.m_panel22, 1, wx.EXPAND | wx.ALL, 5)

        self.m_panel23 = wx.Panel(self.top_panel, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)
        side_graphs_sizer.Add(self.m_panel23, 1, wx.EXPAND | wx.ALL, 5)

        graphs_sizer.Add(side_graphs_sizer, 1, wx.EXPAND, 5)

        self.graph_container.SetSizer(graphs_sizer)
        self.graph_container.Layout()
        graphs_sizer.Fit(self.graph_container)

    def _create_webcam_window(self):
        """

        :return:
        """
        self.webcam_container = wx.Panel(self.bottom_panel, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)

        # Webcam Panel
        self.webcam_panel = wx.Panel(self.webcam_container, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)
        self.webcam_panel.SetBackgroundColour(wx.Colour(142, 142, 142))

        # Webcam Control Band
        self.m_panel_webcam_control = wx.Panel(self.webcam_container, wx.ID_ANY, wx.DefaultPosition,
                                               wx.Size(-1, 30), wx.RAISED_BORDER | wx.TAB_TRAVERSAL)
        self.m_panel_webcam_control.SetMinSize(wx.Size(-1, 30))
        self.m_panel_webcam_control.SetMaxSize(wx.Size(-1, 30))

        webcam_crtl_sizer = wx.BoxSizer(wx.HORIZONTAL)

        self.m_webcam_toggle = wx.ToggleButton(self.m_panel_webcam_control, wx.ID_ANY, u"Enable", wx.DefaultPosition,
                                               wx.DefaultSize, 0)

        self.m_bp_capture = wx.BitmapButton(self.m_panel_webcam_control, wx.ID_ANY,
                                            wx.Bitmap(u".resources/img/camera.png", wx.BITMAP_TYPE_ANY),
                                            wx.DefaultPosition, wx.DefaultSize, wx.BU_AUTODRAW)

        self.m_button_webcam_hide = wx.Button(self.m_panel_webcam_control, wx.ID_ANY, u"Hide", wx.DefaultPosition,
                                              wx.DefaultSize, wx.BU_EXACTFIT)

        webcam_crtl_sizer.Add(self.m_webcam_toggle, 0, wx.ALIGN_LEFT | wx.ALL, 5)
        webcam_crtl_sizer.AddSpacer((0, 0), 1, wx.EXPAND, 5)
        webcam_crtl_sizer.Add(self.m_bp_capture, 0, wx.ALIGN_CENTER_HORIZONTAL, 5)
        webcam_crtl_sizer.AddSpacer((0, 0), 1, wx.EXPAND, 5)
        webcam_crtl_sizer.Add(self.m_button_webcam_hide, 0, wx.ALIGN_RIGHT | wx.ALL, 5)

        self.m_panel_webcam_control.SetSizer(webcam_crtl_sizer)
        self.m_panel_webcam_control.Layout()

        webcam_sizer = wx.BoxSizer(wx.VERTICAL)
        webcam_sizer.Add(self.webcam_panel, 1, wx.EXPAND, 0)
        webcam_sizer.Add(self.m_panel_webcam_control, 1, wx.EXPAND | wx.ALL, 2)

        self.webcam_container.SetSizer(webcam_sizer)
        self.webcam_container.Layout()
        webcam_sizer.Fit(self.webcam_container)

    def _create_skymap_window(self):
        """

        :return:
        """
        self.skymap = wx.Panel(self.bottom_panel, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)

    def _create_logging_window(self):
        """

        :return:
        """
        self.log_box = LogWindow(self)
        self.log_box.Enable(False)

        self.logger = UILogger(self.log_box)

        if self.csm.w_view_log:
            self.log_box.Show()
        else:
            self.log_box.Hide()

    def _create_cmd_input_form(self):
        """

        :return:
        """
        self.cmd_input = wx.TextCtrl(self, wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize,
                                     wx.TE_LEFT | wx.TE_PROCESS_ENTER)
        self.cmd_input.SetToolTipString(u"enter commands")

        if self.csm.w_view_cmd_input:
            self.cmd_input.Show()
        else:
            self.cmd_input.Hide()

    def _connect_events(self):
        """

        :return:
        """
        wx.EVT_MENU(self, self.m_file_quit.GetId(), self.on_application_quit)
        wx.EVT_MENU(self, self.m_config_editconfig.GetId(), self.edit_config)
        wx.EVT_MENU(self, self.m_config_resetconfig.GetId(), self.reset_config)

        self.cmd_input.Bind(wx.EVT_TEXT_ENTER, self.process_text_input)
        self.m_webcam_toggle.Bind(wx.EVT_TOGGLEBUTTON, self.toggle_webcam)
        self.m_bp_capture.Bind(wx.EVT_BUTTON, self.capture_webcam_image)
        self.m_button_webcam_hide.Bind(wx.EVT_BUTTON, self.hide_webcam_panel)

        self.Bind(wx.EVT_MENU, self.view_window_log, id=self.m_view_windows_log.GetId())
        self.Bind(wx.EVT_MENU, self.view_window_skyview, id=self.m_view_windows_skyview.GetId())
        self.Bind(wx.EVT_MENU, self.view_window_webcam, id=self.m_view_windows_webcam.GetId())
        self.Bind(wx.EVT_MENU, self.view_window_status, id=self.m_view_windows_status.GetId())
        self.Bind(wx.EVT_MENU, self.view_window_graph, id=self.m_view_windows_graph.GetId())
        self.Bind(wx.EVT_MENU, self.view_window_cmd_input, id=self.m_view_windows_cmd_input.GetId())

        self.Bind(wx.EVT_MENU, self.view_status_location, id=self.m_view_status_location.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_cmd, id=self.m_view_status_cmd.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_azel, id=self.m_view_status_azel.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_offsets, id=self.m_view_status_offsets.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_ra, id=self.m_view_status_ra.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_time, id=self.m_view_status_time.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_source, id=self.m_view_status_source.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_vlsr, id=self.m_view_status_vlsr.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_integ, id=self.m_view_status_integ.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_freq, id=self.m_view_status_freq.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_if, id=self.m_view_status_if.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_bw, id=self.m_view_status_bw.GetId())
        self.Bind(wx.EVT_MENU, self.view_status_resol, id=self.m_view_status_resol.GetId())

        self.Bind(wx.EVT_MENU, self.set_theme_default, id=self.m_view_theme_default.GetId())
        self.Bind(wx.EVT_MENU, self.set_theme_night, id=self.m_view_theme_night.GetId())

        self.Bind(wx.EVT_TOOL, self.clear_tool_selected, id=self.toolid_clear)
        self.Bind(wx.EVT_TOOL, self.cancel_tool_selected, id=self.toolid_cancel)
        self.Bind(wx.EVT_TOOL, self.stow_tool_selected, id=self.toolid_stow)
        self.Bind(wx.EVT_TOOL, self.azel_tool_selected, id=self.toolid_azel)
        self.Bind(wx.EVT_TOOL, self.npoint_tool_selected, id=self.toolid_npoint)
        self.Bind(wx.EVT_TOOL, self.beamsw_tool_selected, id=self.toolid_beamsw)
        self.Bind(wx.EVT_TOOL, self.freq_tool_select, id=self.toolid_freq)
        self.Bind(wx.EVT_TOOL, self.offset_tool_select, id=self.toolid_offset)
        self.Bind(wx.EVT_TOOL, self.record_tool_select, id=self.toolid_record)
        self.Bind(wx.EVT_TOOL, self.cmdfl_tool_select, id=self.toolid_command)
        self.Bind(wx.EVT_TOOL, self.cal_tool_select, id=self.toolid_cal)
        self.Bind(wx.EVT_TOOL, self.config_tool_select, id=self.toolid_config)

    def on_application_quit(self, event):
        """

        :return:
        """
        self.Close(True)

    def process_text_input(self, event):
        """

        :param event:
        :return:
        """
        text_input = event.GetEventObject()
        input_val = str(text_input.GetValue())
        text_input.Clear()

        self.cmd_interpreter.execute(input_val)

    def toggle_webcam(self, event):
        """

        :param event:
        :return:
        """
        toggle = event.GetEventObject()

        # webcam enabled
        if toggle.GetValue():
            toggle.SetLabel("Disable")
            # self.webcam_panel

        # webcam disabled
        else:
            toggle.SetLabel("Enable")
            # self.webcam_panel

    def capture_webcam_image(self, event):
        event.Skip()

    def hide_webcam_panel(self, event):
        """

        :param event:
        :return:
        """
        if self.m_webcam_toggle.GetValue():
            # TODO: if the webcam is on when it is being hidden, turn it off..
            pass

        hide_window(self.webcam_container)

        if self.m_view_windows_webcam.IsChecked():
            self.m_view_windows_webcam.Toggle()

    def view_window_log(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.log_box)

    def view_window_skyview(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.skymap)

    def view_window_webcam(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.webcam_container)

    def view_window_status(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.status_window)

    def view_window_graph(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.graph_container)

    def view_window_cmd_input(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.cmd_input)

    def view_status_location(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_location)
        self.csm.s_view_location = str(event.IsChecked())

    def view_status_cmd(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_cmd)
        self.csm.s_view_command = str(event.IsChecked())

    def view_status_azel(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_azel)
        self.csm.s_view_azel = str(event.IsChecked())

    def view_status_offsets(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_offset)
        self.csm.s_view_offsets = str(event.IsChecked())

    def view_status_ra(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_ra)
        self.csm.s_view_ra = str(event.IsChecked())

    def view_status_time(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_time)
        self.csm.s_view_time = str(event.IsChecked())

    def view_status_source(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_source)
        self.csm.s_view_source = str(event.IsChecked())

    def view_status_vlsr(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_vlsr)
        self.csm.s_view_vlsr = str(event.IsChecked())

    def view_status_integ(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_integ)
        self.csm.s_view_integ = str(event.IsChecked())

    def view_status_freq(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_freq)
        self.csm.s_view_freq = str(event.IsChecked())

    def view_status_if(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_if)
        self.csm.s_view_if = str(event.IsChecked())

    def view_status_bw(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_bw)
        self.csm.s_view_bw = str(event.IsChecked())

    def view_status_resol(self, event):
        """

        :param event:
        :return:
        """
        handle_window_update(event, self.m_pstatus_resol)
        self.csm.s_view_resol = str(event.IsChecked())

    def set_theme_default(self, event):
        """

        :param event:
        :return:
        """
        color = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BACKGROUND)
        tcolor = wx.Colour(255, 255, 255)

        self.SetBackgroundColour(color)
        for child in self.GetChildren():
            if isinstance(child, wx.TextCtrl):
                child.SetBackgroundColour(tcolor)
            else:
                child.SetBackgroundColour(color)
        self.csm.t_view_default = str(event.IsChecked())

    def set_theme_night(self, event):
        """

        :param event:
        :return:
        """
        color = wx.Colour(50, 50, 50)
        tcolor = wx.Colour(100, 100, 100)

        self.SetBackgroundColour(color)
        for child in self.GetChildren():
            if isinstance(child, wx.TextCtrl):
                child.SetBackgroundColour(tcolor)
            else:
                child.SetBackgroundColour(color)
        self.csm.t_view_night = str(event.IsChecked())

    def clear_tool_selected(self, event):
        """

        :param event:
        :return:
        """
        self.cmd_input.Clear()
        self.log_box.ClearAll()

    def cancel_tool_selected(self, event):
        if self.csm.show_cancel_dialog:
            cancel = CancelDialog(self, self.csm)
            if cancel.ShowModal() == wx.ID_OK:
                self.csm.show_cancel_dialog = not cancel.GetCheckboxValue()
                # TODO: logic for canceling all current actions
            cancel.Destroy()
        else:
            pass
            # TODO: logic for canceling all current actions

    def stow_tool_selected(self, event):
        event.Skip()

    def azel_tool_selected(self, event):
        """

        :param event:
        :return:
        """
        azel = AzelDialog(None, self.csm)
        if azel.ShowModal() == wx.ID_OK:
            pass
        azel.Destroy()

    def npoint_tool_selected(self, event):
        """

        :param event:
        :return:
        """
        npoint = NpointDialog(None)
        if npoint.ShowModal() == wx.ID_OK:
            pass
        npoint.Destroy()

    def beamsw_tool_selected(self, event):
        """

        :param event:
        :return:
        """
        bsw = BeamswitchDialog(None)
        if bsw.ShowModal() == wx.ID_OK:
            pass
        bsw.Destroy()

    def freq_tool_select(self, event):
        """

        :param event:
        :return:
        """
        freq = FrequencyDialog(None, self.csm)
        if freq.ShowModal() == wx.ID_OK:
            try:
                new_freq = freq.GetValue().strip(' ')
                if new_freq is not None and new_freq != '':
                    self.csm.freq = float(new_freq)
                    self.logger.info('Frequency set to: {}'.format(new_freq))
            except ValueError:
                self.logger.error('Invalid frequency value given: "{}". (must be an int or float)'.format(new_freq))
        freq.Destroy()

    def offset_tool_select(self, event):
        """

        :param event:
        :return:
        """
        offset = OffsetDialog(None)
        if offset.ShowModal() == wx.ID_OK:
            pass
        offset.Destroy()

    def record_tool_select(self, event):
        """

        :param event:
        :return:
        """
        rec = RecordDialog(None)
        if rec.ShowModal() == wx.ID_OK:
            pass
        rec.Destroy()

    def cmdfl_tool_select(self, event):
        """

        :param event:
        :return:
        """
        filename = wx.FileSelector(default_path=os.getcwd())

    def cal_tool_select(self, event):
        """

        :param event:
        :return:
        """
        cal = CalibrateDialog(None)
        if cal.ShowModal() == wx.ID_OK:
            pass
        cal.Destroy()

    def config_tool_select(self, event):
        """

        :param event:
        :return:
        """
        self.edit_config(event)

    def splitter_window_on_idle(self, event):
        """

        :param event:
        :return:
        """
        self.splitter_window.SetSashPosition(0)
        self.splitter_window.Unbind(wx.EVT_IDLE)

    def edit_config(self, event):
        """

        :return:
        """
        # TODO Disable main window before creation, Enable after del so
        # cannot modify main window while editing properties
        properties_window = Properties(None, self.csm)
        properties_window.Show()

    def reset_config(self, event):
        """

        :return:
        """
        self.csm.reset()
        self.logger.info('Reset configurations to default.')

