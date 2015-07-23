"""

"""
import wx

from frame_main import MainFrame


class SRTFrame(MainFrame):
    """

    """
    def __init__(self, parent):
        MainFrame.__init__(self, parent)


class SRT(wx.App):
    """

    """
    def OnInit(self):
        self.main_frame = SRTFrame(None)
        self.main_frame.Show()
        self.SetTopWindow(self.main_frame)
        return True


app = SRT()
app.MainLoop()
