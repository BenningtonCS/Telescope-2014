# -*- coding: utf-8 -*-
"""
Interpret and process commands from the UI command input.

-------------------------
author: erick daniszewski
"""
from ui_logging import UILogger
from ui_utils import show_window, hide_window

import help_docs


class CommandInterpreter(object):
    """
    Interprets, validates, and executes commands passed in via the UI commnand input.
    """
    def __init__(self, parent, log_out):
        self.parent = parent
        self.log = UILogger(log_out)

        self.windows = {
            'graphs'    : self.parent.graph_container,
            'log'       : self.parent.log_box,
            'skymap'    : self.parent.skymap,
            'webcam'    : self.parent.webcam_container,
            'status'    : self.parent.status_window,
            'cmd_input' : self.parent.cmd_input
        }

        self.commands = [
            'SHOW', 'HIDE', 'CLEAR', 'HELP', 'LIST'
        ]

    def execute(self, command):
        """
        Execute a given command.

        If the command is invalid or an argument to the command is invalid, a warning
        is logged to the UI log window and no further action on the command is taken.

        :param command: string containing the commands and args, space delimited
        :type command: str
        :return: None
        """
        cmd_array = filter(None, command.split(' '))

        if len(cmd_array) > 0:
            if cmd_array[0].upper() in self.commands:
                cmd, args = cmd_array[0].upper(), cmd_array[1:]

                if 'SHOW' == cmd:
                    self._cmd_show(args)

                elif 'HIDE' == cmd:
                    self._cmd_hide(args)

                elif 'CLEAR' == cmd:
                    self._cmd_clear()

                elif 'HELP' == cmd:
                    self._cmd_help(args)

                elif 'LIST' == cmd:
                    self._cmd_list(args)

            else:
                self.log.log('command not recognized: {}'.format(cmd_array[0]))

    def _cmd_show(self, args):
        """
        Process a 'SHOW' command

        :param args: any arguments for the given command
        :type args: list
        :return: None
        """
        if not args:
            self.log.warn('no arguments given for command: SHOW')
            return

        for arg in args:
            try:
                window = self.windows[arg.lower()]
                show_window(window)
                self.log.log('showing window: {}'.format(arg.lower()))
            except KeyError:
                self.log.warn('invalid argument for SHOW command: {}.'.format(arg.lower()))

    def _cmd_hide(self, args):
        """
        Process a 'HIDE' command

        :param args: any arguments for the given command
        :type args: list
        :return: None
        """
        if not args:
            self.log.warn('no arguments given for command: HIDE')
            return

        for arg in args:
            try:
                window = self.windows[arg.lower()]
                hide_window(window)
                self.log.log('hiding window: {}'.format(arg.lower()))
            except KeyError:
                self.log.warn('invalid argument for HIDE command: {}.'.format(arg.lower()))

    def _cmd_clear(self):
        """
        Process a 'CLEAR' command

        :return: None
        """
        self.parent.clear_tool_selected(None)

    def _cmd_list(self, args):
        """
        Process a 'LIST' command

        :param args: any arguments for the given command
        :type args: list
        :return: None
        """
        if not args:
            self.log.warn('no arguments given for command: LIST')
            return

        arg = args[0].lower()

        if arg == 'commands':
            message = 'current supported commands:\n\t' + ', '.join(self.commands)
            self.log.log(message)

        elif arg == 'windows':
            message = 'valid window names:\n\t' + ', '.join(self.windows.keys())
            self.log.log(message)

    def _cmd_help(self, args):
        """
        Process a 'HELP' command

        :param args: any arguments for the given command
        :type args: list
        :return: None
        """
        if not args:
            self.log.warn('no arguments given for command: HELP')
            return

        command = args[0].upper()

        if 'SHOW' == command:
            for msg in help_docs.show_help:
                self.log.log(msg)

        elif 'HIDE' == command:
            for msg in help_docs.hide_help:
                self.log.log(msg)

        elif 'CLEAR' == command:
            for msg in help_docs.clear_help:
                self.log.log(msg)

        elif 'HELP' == command:
            for msg in help_docs.help_help:
                self.log.log(msg)

        elif 'LIST' == command:
            for msg in help_docs.list_help:
                self.log.log(msg)

        else:
            self.log.warn('no help information available for command: {}'.format(command))
