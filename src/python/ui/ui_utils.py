"""

"""
import wx


def static_text_gen(parent, text):
    """

    :param parent:
    :param text:
    :type text: unicode
    :return:
    """
    return wx.StaticText(parent, wx.ID_ANY, text, wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_LEFT)


def radiobutton_gen(parent, text):
    """

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

    :param parent:
    :param label:
    :param tooltip:
    :return:
    """
    return wx.MenuItem(parent, wx.ID_ANY, label, tooltip, wx.ITEM_NORMAL)


def checked_menu_item(parent, label, tooltip):
    """

    :param parent:
    :param label:
    :param tooltip:
    :param checked:
    :return:
    """
    return wx.MenuItem(parent, wx.ID_ANY, label, tooltip, wx.ITEM_CHECK)


def status_panel(parent, size):
    """

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

    :param parent:
    :param value:
    :return:
    """
    text =  wx.StaticText(parent, wx.ID_ANY, value, wx.DefaultPosition, wx.DefaultSize, wx.ALIGN_LEFT | wx.ST_NO_AUTORESIZE)

    text.Wrap(-1)
    text.SetFont(wx.Font(10, 70, 90, 90, False, wx.EmptyString))

    return text


def show_window(window):
    """

    :param window:
    :return:
    """
    window.Show()
    window.Enable()
    window.GetParent().Layout()


def hide_window(window):
    """

    :param window:
    :return:
    """
    window.Hide()
    window.Disable()
    window.GetParent().Layout()


def handle_window_update(event, window):
    """

    :param event:
    :param window:
    :return:
    """
    if event.IsChecked():
        show_window(window)
    else:
        hide_window(window)
