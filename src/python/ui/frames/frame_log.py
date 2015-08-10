# -*- coding: utf-8 -*-
"""
Custom subclass of wx.stc.StyledTextCtrl to be used as a logging window

-------------------------
author: erick daniszewski
"""
import wx
from wx.stc import StyledTextCtrl

from src.python.ui.ui_logging import Level
from src.python.ui.help_docs import start_msg


class LogWindow(StyledTextCtrl):
    """
    A logging window with styles set for the different supported log levels
    """
    def __init__(self, parent, style=wx.SIMPLE_BORDER):
        """
        Constructor
        """
        StyledTextCtrl.__init__(self, parent, style=style)

        # This list of styles should be parallel with the levels in ui_logging#Levels
        self.log_styles = ['gray', 'blue', 'black', 'green', 'gold', 'red']

        self._styles = [None] * 32
        self._free = 1

        # Write initial message to window
        for item in start_msg:
            self.write(item + '\n', Level.SYSTEM)

    def get_style(self, c='black'):
        """
        Returns a style for a given log level. If the log level is not a
        well-known level, default to the LOG level.
        """
        free = self._free
        if c and isinstance(c, (str, unicode)):
            c = c.lower()
        else:
            c = 'black'

        try:
            style = self._styles.index(c)
            return style

        except ValueError:
            style = free
            self._styles[style] = c
            self.StyleSetForeground(style, wx.NamedColour(c))

            free += 1
            if free > 31:
                free = 0
            self._free = free
            return style

    def get_logging_style(self, level):
        """
        Get the style for the specified log level
        """
        try:
            style = self.log_styles[level]
        except Exception:
            style = self.log_styles[0]

        return style

    def write(self, text, level=None):
        """
        Add the text to the end of the control using a color based on log level.
        """
        color = self.get_logging_style(level)
        style = self.get_style(color)
        text_len = len(text.encode('utf8'))
        end = self.GetLength()
        self.AddText(text)
        self.StartStyling(end, 31)
        self.SetStyling(text_len, style)
        self.EnsureCaretVisible()

    __call__ = write
