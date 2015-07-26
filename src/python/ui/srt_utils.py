# -*- coding: utf-8 -*-
"""
Various utility functions.

-------------------------
author: erick daniszewski
"""
from random import randint
from math import sqrt, log


def gauss():
    """
    A gaussian function. This is a direct port of the main#gauss() function in the
    MIT Haystack STRn ver. 3 source code.

    FIXME: Python's Lib/random.py has a function: gauss(mu, sigma), where 'mu' is the mean
    and 'sigma' is the standard deviation. Would this be better to leverage rather than
    writing our own?

    :return:
    """
    v1, r = 0.0, 0.0

    while r > 1.0 or r == 0.0:
        # Here, randint() has a max value of 32767 because the C rand() fn returns an
        # integer in the range of 0 and RAND_MAX, where RAND_MAX is a constant whose
        # default value may vary between implementations but it is granted to be at
        # least 32767.
        v1 = 2.0 * (randint(0, 32767) / 2147483648.0) - 1.0
        v2 = 2.0 * (randint(0, 32767) / 2147483648.0) - 1.0
        r = (v1 ** 2) + (v2 ** 2)

    fac = sqrt(-2.0 * log(r) / r)
    return v1 * fac
