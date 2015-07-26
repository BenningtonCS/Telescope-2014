# -*- coding: utf-8 -*-
"""
Classes pertaining to UI logging capability and functionality.

-------------------------
author: erick daniszewski
"""


class Level(object):
    """
    Class containing well-known logging levels supported by the UI logging
    """
    SYSTEM, DEBUG, LOG, INFO, WARN, ERROR = range(6)


class UILogger(object):
    """
    UI Logging convenience class which contains methods to log messages to the UI's logging window.
    The supported log output levels (each with their own output format) are found in the
    ui_logging#Level class.

    Unless there is an exceptional case where you should write to the log window directly, using
    this class and its provided methods is highly recommended.
    """
    def __init__(self, log_window):
        """
        Constructor

        :param log_window: the ui logging window to write the log messages to
        :return: None
        """
        self._log = log_window

    def system(self, message):
        """
        Log a message at log level: SYSTEM

        :param message: the message to be logged to the UI log window
        :type message: str
        :return: None
        """
        self._log.write('{}\n'.format(message), Level.SYSTEM)

    def debug(self, message):
        """
        Log a message at log level: DEBUG

        :param message: the message to be logged to the UI log window
        :type message: str
        :return: None
        """
        self._log.write('[DEBUG] {}\n'.format(message), Level.DEBUG)

    def log(self, message):
        """
        Log a message at log level: LOG

        :param message: the message to be logged to the UI log window
        :type message: str
        :return: None
        """
        self._log.write('[LOG] {}\n'.format(message), Level.LOG)

    def info(self, message):
        """
        Log a message at log level: INFO

        :param message: the message to be logged to the UI log window
        :type message: str
        :return: None
        """
        self._log.write('[INFO] {}\n'.format(message), Level.INFO)

    def warn(self, message):
        """
        Log a message at log level: WARN

        :param message: the message to be logged to the UI log window
        :type message: str
        :return: None
        """
        self._log.write('[WARN] {}\n'.format(message), Level.WARN)

    def error(self, message):
        """
        Log a message at log level: ERROR

        :param message: the message to be logged to the UI log window
        :type message: str
        :return: None
        """
        self._log.write('[ERROR] {}\n'.format(message), Level.ERROR)
