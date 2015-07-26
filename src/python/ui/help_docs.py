# -*- coding: utf-8 -*-
"""
Help documentation for various commands.

-------------------------
author: erick daniszewski
"""

# FIXME: these should be global somewhere, not defined in multiple places
window_names = ['graphs', 'log', 'skymap', 'webcam', 'status', 'cmd_input']
commands = ['SHOW', 'HIDE', 'CLEAR', 'HELP', 'LIST']
keywords = ['commands', 'windows']

# help message for 'show' command
show_help = [
    '---------------------------------------------',
    'Desc.:\tShows the specified window in the UI.',
    'Usage:\tSHOW <window-name>',
    'Details:\tValid window names: ' + ', '.join(window_names),
    '---------------------------------------------'
]

# help message for 'hide' command
hide_help = [
    '---------------------------------------------',
    'Desc.:\tHides teh specified window in the UI.',
    'Usage:\tHIDE <window-name>',
    'Details:\tValid window names: ' + ', '.join(window_names),
    '---------------------------------------------'
]

# help message for 'help' command
help_help = [
    '---------------------------------------------',
    'Desc.:\tShows the usage information for a specified command.',
    'Usage:\tHELP <command>',
    'Details:\tValid command names: ' + ', '.join(commands),
    '---------------------------------------------'
]

# help message for 'clear' command
clear_help = [
    '---------------------------------------------',
    'Desc.:\tClear the UI log and command input areas.',
    'Usage:\tCLEAR',
    '---------------------------------------------'
]

# help message for 'list' command
list_help = [
    '---------------------------------------------',
    'Desc.:\tList specified keywords supported by the SRT system.',
    'Usage:\tLIST <keyword>',
    'Details:\tValid keywords: ' + ', '.join(keywords),
    '---------------------------------------------'
]
