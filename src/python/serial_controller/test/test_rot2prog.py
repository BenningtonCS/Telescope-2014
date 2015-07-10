# -*- coding: utf-8 -*-
"""
Test cases for the rot2prog controller.

The test cases are run, connecting to port /dev/ttyUSB0, to
check for the correct packet responses from the controller.
See rot2prog.py for definition of the packets.
"""
import logging
import time

from ..rot2prog import *


# Initialize the logging facilities
log = logging.getLogger(__name__)
handler = logging.FileHandler('rot2prog_test.log')
frmt = logging.Formatter('%(asctime)s [%(levelname)s] > %(message)s')
handler.setFormatter(frmt)
log.addHandler(handler)

# Set log level to debug
log.setLevel(logging.DEBUG)


r2p = Rot2ProgController('/dev/ttyUSB0')

log.debug('================================')
log.debug('Controller Setup')
log.debug('--------------------------------')
log.debug('port: \t\t{}'.format(r2p.port))
log.debug('baud: \t\t{}'.format(r2p.baudrate))
log.debug('bytesize: \t{}'.format(r2p.bytesize))
log.debug('parity: \t\t{}'.format(r2p.parity))
log.debug('stopbits: \t{}'.format(r2p.stopbits))
log.debug('resolution: \t{}'.format(r2p.resol))
log.debug('================================')
log.debug('')

## Hardcoded packet commands, for reference
# Set Cmd
packet = [BYTE_S] + [1, 2, 3, 4] + [r2p.resol] + [5, 6, 7, 8] + [r2p.resol] + [BYTE_SET] + [BYTE_E]
log.debug('SET COMMAND PACKET \t:: {}'.format(packet))

# Stop Cmd
packet = [BYTE_S] + [BYTE_I] * 10 + [BYTE_STOP] + [BYTE_E]
log.debug('STOP COMMAND PACKET \t:: {}'.format(packet))

# Status Cmd
packet = [BYTE_S] + [BYTE_I] * 10 + [BYTE_STAT] + [BYTE_E]
log.debug('STAT COMMAND PACKET \t:: {}'.format(packet))
log.debug('')



## Now, getting down to the real business...
## wrapping it all in a try block, incase something bad happens
## .. like not finding the serial port, so it can be logged

try:
    # Check telescope status
    initial_response = r2p.status()
    log.debug('Initial Stat Response: {}'.format(initial_response))

    # Stop the telescope (should already be stopped, but this is to
    # check the response value
    response = r2p.stop()
    log.debug('Initial Stop Response: {}'.format(response))

    # Now, set the telescope to some location... here, setting it to
    # AZ: 123.5, EL: 77.0
    # note that the set command does not return a response
    r2p.set([0x30, 0x39, 0x36, 0x37], [0x30, 0x38, 0x37, 0x34])
    log.debug('Setting telescope to az:123.5, el:77.0')

    # Now, lets see if we can get some output while the telescope is changing location
    for x in range(10):
        response = r2p.status()
        log.debug('status: {}'.format(response))
        time.sleep(0.25)

    # whether it got there or not, we are stopping the telescope.
    response = r2p.stop()
    log.debug('STOPPING response: {}'.format(response))

    # get the status once more, because why not
    response = r2p.status()
    log.debug('stopped status: {}'.format(response))

    # Wait a bit, then check the status again to make sure it did actually stop.
    time.sleep(5)
    response = r2p.status()
    log.debug('stopped status: {}'.format(response))


    ## attempt to set the telescope back to the starting position
    hvals = initial_response[1:5]
    vvals = initial_response[6:10]
    r2p.set(hvals, vvals)

except Exception:
    log.exception('Exception thrown:')


log.debug('')
log.debug('================================')
log.debug('------   END LOG SESSION  ------')
log.debug('================================')