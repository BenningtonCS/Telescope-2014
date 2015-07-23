"""

"""
from ui_logging import Level
from ui_utils import show_window, hide_window


class CommandInterpreter(object):
    """

    """
    def __init__(self, parent, log_out=None):
        self.parent = parent
        self.log = log_out

        self.windows = {
            'graphs'    : self.parent.graph_container,
            'log'       : self.parent.log_box,
            'skymap'    : self.parent.skymap,
            'webcam'    : self.parent.webcam_container,
            'status'    : self.parent.status_window,
            'cmd_input' : self.parent.cmd_input
        }

        self.commands = [
            'SHOW', 'HIDE'
        ]

    def execute(self, command):
        """

        :param command:
        :return:
        """
        cmd_array = filter(None, command.split(' '))

        if len(cmd_array) > 0:
            if cmd_array[0].upper() in self.commands:
                cmd, args = cmd_array[0].upper(), cmd_array[1:]

                if 'SHOW' == cmd:
                    self.cmd_show(args)

                elif 'HIDE' == cmd:
                    self.cmd_hide(args)

            else:
                self.__log('command not recognized: {}'.format(cmd_array[0]))

    def cmd_show(self, args):
        """

        :param args:
        :type args: list
        :return:
        """
        for arg in args:
            try:
                window = self.windows[arg.lower()]
                show_window(window)
                self.__log('showing window: {}'.format(arg.lower()))
            except KeyError:
                self.__error('invalid argument for SHOW command: {}. see \'HELP SHOW\''.format(arg.lower()))

    def cmd_hide(self, args):
        """

        :param args:
        :return:
        """
        for arg in args:
            try:
                window = self.windows[arg.lower()]
                hide_window(window)
                self.__log('hiding window: {}'.format(arg.lower()))
            except KeyError:
                self.__error('invalid argument for HIDE command: {}. see \'HELP HIDE\''.format(arg.lower()))

    def __log(self, message):
        """

        :param message:
        :return:
        """
        self.log.write('[LOG] {}\n'.format(message), Level.LOG)

    def __info(self, message):
        """

        :param message:
        :return:
        """
        self.log.write('[INFO] {}\n'.format(message), Level.INFO)

    def __warn(self, message):
        """

        :param message:
        :return:
        """
        self.log.write('[WARN] {}\n'.format(message), Level.WARN)

    def __error(self, message):
        """

        :param message:
        :return:
        """
        self.log.write('[ERROR] {}\n'.format(message), Level.ERROR)
