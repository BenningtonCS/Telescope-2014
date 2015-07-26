# -*- coding: utf-8 -*-
"""
Supported command keywords

-------------------------
author: erick daniszewski
"""

__all__ = ['KEYWORDS', 'is_valid_keyword']


# _keys stores the supported keywords which the command parser and the
# command executor can operate with. To support additional keywords,
# simply add the keyword to the list below, and add the appropriate
# execution path in the command executor.
_keys = [
    'sourcename',
    'mode',
    'radec',
    'azel',
    'galactic',
    'offset',
    'stow',
    'calibrate',
    'noisecal',
    'record',
    'freq',
    'roff',
    'wait'
]

# creates a unique value for each keyword in the _keys list
_values = range(len(_keys))

# key each keyword in the _keys list to a unique value in the _values list
KEYWORDS = dict(zip(_keys, _values))


# --------------
# Static Methods
# --------------

def is_valid_keyword(keyword):
    """
    Check if a given string is a valid supported command keyword

    :param keyword:
    :return:
    """
    return KEYWORDS.__contains__(keyword)
