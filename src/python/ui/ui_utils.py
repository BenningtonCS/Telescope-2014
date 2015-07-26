# -*- coding: utf-8 -*-
"""
Utilities for constructing and manipulating UI components

-------------------------
author: erick daniszewski
"""
import wx


def static_text_gen(parent, text):
    """
    Generate a left-aligned instance of wx.StaticText

    :param parent:
    :param text:
    :type text: unicode
    :return:
    """
    return wx.StaticText(parent, wx.ID_ANY, text, wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_LEFT)


def radiobutton_gen(parent, text):
    """
    Generate a sizer containing radio buttons specified by the input 'text' list.

    :param parent:
    :param text:
    :type text: list
    :return:
    """
    sizer = wx.BoxSizer(wx.HORIZONTAL)

    for i, label in enumerate(text):
        if i == 0:
            btn = wx.RadioButton(parent, wx.ID_ANY, label, wx.DefaultPosition, wx.DefaultSize, wx.RB_GROUP)
            btn.SetValue(True)
        else:
            btn = wx.RadioButton(parent, wx.ID_ANY, label, wx.DefaultPosition, wx.DefaultSize, 0)

        sizer.Add(btn, 0, wx.ALL, 5)

    return sizer


def norm_menu_item(parent, label, tooltip):
    """
    Generate an instance of wx.MenuItem with property wx.ITEM_NORMAL

    :param parent:
    :param label:
    :param tooltip:
    :return:
    """
    return wx.MenuItem(parent, wx.ID_ANY, label, tooltip, wx.ITEM_NORMAL)


def checked_menu_item(parent, label, tooltip):
    """
    Generate an instance of wx.MenuItem with property wx.ITEM_CHECK

    :param parent:
    :param label:
    :param tooltip:
    :param checked:
    :return:
    """
    return wx.MenuItem(parent, wx.ID_ANY, label, tooltip, wx.ITEM_CHECK)


def status_panel(parent, size):
    """
    Generate an instance of wx.Panel with a MinSize and MaxSize set

    :param parent:
    :param size:
    :return:
    """
    panel = wx.Panel(parent, wx.ID_ANY, wx.DefaultPosition, size, wx.TAB_TRAVERSAL)

    panel.SetMinSize(size)
    panel.SetMaxSize(size)

    return panel


def status_label(parent, label):
    """
    Generate a wx.StaticText for the label of the Status window of the UI

    :param parent:
    :param label:
    :return:
    """
    text = wx.StaticText(parent, wx.ID_ANY, label, wx.DefaultPosition, wx.Size(50, -1), wx.ALIGN_LEFT | wx.ST_NO_AUTORESIZE)

    text.Wrap(-1)
    text.SetFont(wx.Font(10, 70, 90, 90, False, wx.EmptyString))
    text.SetMinSize(wx.Size(50, -1))
    text.SetMaxSize(wx.Size(50, -1))

    return text


def status_value(parent, value):
    """
    Generate a wx.StaticText for the value of the Status window of the UI

    :param parent:
    :param value:
    :return:
    """
    text = wx.StaticText(parent, wx.ID_ANY, value, wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_LEFT | wx.ST_NO_AUTORESIZE)

    text.Wrap(-1)
    text.SetFont(wx.Font(10, 70, 90, 90, False, wx.EmptyString))

    return text


def show_window(window):
    """
    Show and enable the specified window

    :param window:
    :return:
    """
    window.Show()
    window.Enable()
    window.GetParent().Layout()


def hide_window(window):
    """
    Hide and disable the specified window

    :param window:
    :return:
    """
    window.Hide()
    window.Disable()
    window.GetParent().Layout()


def handle_window_update(event, window):
    """
    Check if an item is checked, and show/hide the specified window accordingly

    :param event:
    :param window:
    :return:
    """
    if event.IsChecked():
        show_window(window)
    else:
        hide_window(window)
