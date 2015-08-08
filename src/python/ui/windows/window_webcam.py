"""

"""
import wx


class Webcam(wx.Frame):
    """

    """
    def __init__(self, parent):
        wx.Frame.__init__(self, parent, id=wx.ID_ANY, title=u"Webcam", pos=wx.DefaultPosition,
                          size=wx.Size(400, 450), style=wx.DEFAULT_FRAME_STYLE)

        self._create_webcam_panel()
        self._create_webcam_controls()

        webcam_sizer = wx.BoxSizer(wx.VERTICAL)
        webcam_sizer.Add(self.webcam_panel, 1, wx.EXPAND, 0)
        webcam_sizer.Add(self.webcam_control, 1, wx.EXPAND | wx.ALL, 2)

        self.SetSizer(webcam_sizer)
        self.Layout()
        webcam_sizer.Fit(self)

    def _create_webcam_panel(self):
        """

        :return:
        """
        self.webcam_panel = wx.Panel(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)
        self.webcam_panel.SetBackgroundColour(wx.Colour(142, 142, 142))

    def _create_webcam_controls(self):
        """

        :return:
        """
        self.webcam_control = wx.Panel(self, wx.ID_ANY, wx.DefaultPosition,
                                               wx.Size(-1, 30), wx.RAISED_BORDER | wx.TAB_TRAVERSAL)
        self.webcam_control.SetMinSize(wx.Size(-1, 30))
        self.webcam_control.SetMaxSize(wx.Size(-1, 30))

        webcam_crtl_sizer = wx.BoxSizer(wx.HORIZONTAL)

        self.m_webcam_toggle = wx.ToggleButton(self.webcam_control, wx.ID_ANY, u"Enable", wx.DefaultPosition,
                                               wx.DefaultSize, 0)

        self.m_bp_capture = wx.BitmapButton(self.webcam_control, wx.ID_ANY,
                                            wx.Bitmap(u".resources/img/camera.png", wx.BITMAP_TYPE_ANY),
                                            wx.DefaultPosition, wx.DefaultSize, wx.BU_AUTODRAW)

        self.m_button_webcam_hide = wx.Button(self.webcam_control, wx.ID_ANY, u"Hide", wx.DefaultPosition,
                                              wx.DefaultSize, wx.BU_EXACTFIT)

        webcam_crtl_sizer.Add(self.m_webcam_toggle, 0, wx.ALIGN_LEFT | wx.ALL, 5)
        webcam_crtl_sizer.AddSpacer((0, 0), 1, wx.EXPAND, 5)
        webcam_crtl_sizer.Add(self.m_bp_capture, 0, wx.ALIGN_CENTER_HORIZONTAL, 5)
        webcam_crtl_sizer.AddSpacer((0, 0), 1, wx.EXPAND, 5)
        webcam_crtl_sizer.Add(self.m_button_webcam_hide, 0, wx.ALIGN_RIGHT | wx.ALL, 5)

        self.webcam_control.SetSizer(webcam_crtl_sizer)
        self.webcam_control.Layout()
