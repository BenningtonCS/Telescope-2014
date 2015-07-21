# -*- coding: utf-8 -*-
"""
Base class for any controller to be defined.
"""


class BaseSerialController(object):
    """
    Effective interface for any serial controller
    """
    def __init__(self):
        pass

    def set(self, h_vals, v_vals):
        """ Override in derived class """
        raise NotImplementedError()

    def stop(self):
        """ Override in derived class """
        raise NotImplementedError()

    def status(self):
        """ Override in derived class """
        raise NotImplementedError()
