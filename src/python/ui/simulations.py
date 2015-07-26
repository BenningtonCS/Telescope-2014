# -*- coding: utf-8 -*-
"""
Simulate data for antenna motion, receiver output, and fft results.

-------------------------
author: erick daniszewski
"""
from srt_utils import gauss
from math import sqrt, sin, pi


def simulate_receiver(bw):
    """
    Simulate the data from the receiver. This method is a port of the data simulation used in
    the MIT Haystack STRn ver. 3 source code.

    :param bw:
    :return:
    """
    nsam = 0x100000
    av = 5.0

    sim = lambda x: sqrt(av) * gauss() + 127.397 + 0.5 + 0 * sin(2.0 * pi * 0.5 * (x / 2) / bw)
    sim_data = [sim(i) for i in range(nsam)]

    return sim_data
