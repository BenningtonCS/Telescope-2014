import wx
from srt_ui import MainFrame, Properties
from ui_component import WebcamPanel


class PrimaryFrame(MainFrame):
    def __init__(self, parent):
        MainFrame.__init__(self, parent)

        self.webcam_disabled = None
        self.webcam_enabled = None

    # # HELPER FNs
    def show_window(self, window):
        window.Show()
        window.Enable()
        window.GetParent().Layout()

    def hide_window(self, window):
        window.Hide()
        window.Disable()
        window.GetParent().Layout()

    def handle_window_update(self, event, window):
        if event.IsChecked():
            self.show_window(window)
        else:
            self.hide_window(window)

    ## OVERRIDE EVENTS ##

    def process_text_input(self, event):
        text_input = event.GetEventObject()
        data = str(text_input.GetValue())
        text_input.Clear()
        self.m_log_box.AppendText(data + '\n')

    def view_window_graph(self, event):
        event.Skip()

    def view_window_log(self, event):
        if event.IsChecked():
            self.show_window(self.m_log_box)
        else:
            self.hide_window(self.m_log_box)

    def view_window_skyview(self, event):
        if event.IsChecked():
            self.show_window(self.m_richText12)
        else:
            self.hide_window(self.m_richText12)

    def view_window_status(self, event):
        if event.IsChecked():
            self.show_window(self.m_status_window)
        else:
            self.hide_window(self.m_status_window)

    def view_window_webcam(self, event):
        if event.IsChecked():
            self.show_window(self.m_panel_webcam_container)
        else:
            self.hide_window(self.m_panel_webcam_container)

    def toggle_webcam( self, event ):
        toggle = event.GetEventObject()

        # webcam enabled
        if toggle.GetValue():
            toggle.SetLabel("Disable")
            self.m_panel_webcam.draw_enabled_panel()

        # webcam disabled
        else:
            toggle.SetLabel("Enable")
            self.m_panel_webcam.draw_disabled_panel()

    def hide_webcam_panel( self, event ):
        if self.m_webcam_toggle.GetValue():
            # TODO: if the webcam is on when it is being hidden, turn it off..
            pass

        self.hide_window(self.m_panel_webcam_container)

        if self.m_view_windows_webcam.IsChecked():
            self.m_view_windows_webcam.Toggle()

    def capture_webcam_image( self, event ):
        self.m_panel_webcam.save_image()

    ##
    ##

    def view_status_location(self, event):
        self.handle_window_update(event, self.m_pstatus_location)

    def view_status_azel(self, event):
        self.handle_window_update(event, self.m_pstatus_azel)

    def view_status_cmd(self, event):
        self.handle_window_update(event, self.m_pstatus_cmd)

    def view_status_freq( self, event ):
        self.handle_window_update(event, self.m_pstatus_freq)

    def view_status_if( self, event ):
        self.handle_window_update(event, self.m_pstatus_if)

    def view_status_integ( self, event ):
        self.handle_window_update(event, self.m_pstatus_integ)

    def view_status_offsets( self, event ):
        self.handle_window_update(event, self.m_pstatus_offset)

    def view_status_ra( self, event ):
        self.handle_window_update(event, self.m_pstatus_ra)

    def view_status_resol( self, event ):
        self.handle_window_update(event, self.m_pstatus_resol)

    def view_status_source( self, event ):
        self.handle_window_update(event, self.m_pstatus_source)

    def view_status_time( self, event ):
        self.handle_window_update(event, self.m_pstatus_time)

    def view_status_vlsr( self, event ):
        self.handle_window_update(event, self.m_pstatus_vlsr)

    def view_status_bw( self, event ):
        self.handle_window_update(event, self.m_pstatus_bw)

    def set_theme_default(self, event):
        color = wx.SystemSettings.GetColour(wx.SYS_COLOUR_BACKGROUND)
        tcolor = wx.Colour(255, 255, 255)

        self.SetBackgroundColour(color)
        for child in self.GetChildren():
            if isinstance(child, wx.TextCtrl):
                child.SetBackgroundColour(tcolor)
            else:
                child.SetBackgroundColour(color)

    def set_theme_night(self, event):
        color = wx.Colour(50, 50, 50)
        tcolor = wx.Colour(100, 100, 100)

        self.SetBackgroundColour(color)
        for child in self.GetChildren():
            if isinstance(child, wx.TextCtrl):
                child.SetBackgroundColour(tcolor)
            else:
                child.SetBackgroundColour(color)


    '''
    def config_tool_select( self, event ):
        # Disable main window before creation, Enable after del so
        # cannot modify main window while editing properties
        properties_window = Properties(None)
        properties_window.Show()

    def clear_tool_selected( self, event ):
        self.m_textCtrl2.Clear()
    '''


class SRT(wx.App):
    def OnInit(self):
        self.m_frame = PrimaryFrame(None)
        self.m_frame.Show()
        self.SetTopWindow(self.m_frame)
        return True


app = SRT(0)
app.MainLoop()
