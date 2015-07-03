# -*- coding: utf-8 -*-
"""
Serial Controller for the SPID Rot2Prog Controller
http://spid.net.pl/en/rot2-2/
"""
import serial

from controller import BaseSerialController


## CONTROLLER SPECIFIC CONSTANTS
BYTE_S = 0x57  # start byte value
BYTE_E = 0x20  # end byte value
BYTE_I = 0x00  # ignore byte value

BYTE_STOP = 0x0F  # stop command byte
BYTE_STAT = 0x1F  # status command byte
BYTE_SET  = 0x2F  # set command byte

RES_ONE   = 0x1  # 1 degree per pulse
RES_HALF  = 0x2  # 0.5 degree per pulse
RES_QUART = 0x4  # 0.25 degree per pulse

CMD_SIZE  = 13  # size, in bytes, of the command packet
RESP_SIZE = 12  # size, in bytes, of the response packet


class Rot2ProgController(object, BaseSerialController):
    """
    Serial Controller for the SPID Rot2Prog.
    documentation from: http://ryeng.name/blog/3

    Protocol:   baudrate :  600 bps
                bytesize :    8 bits (inherited)
                parity   : None      (inherited)
                stopbits :    1      (inherited)

    All commands for the Rot2Prog controller are issued as 13-byte packets.
    Responses are received as 12-byte packets. See below for a description of
    the command and response packets.

    COMMAND PACKET
    ===========================================================================
    Byte :  0    1    2    3    4    5    6    7    8    9    10   11   12
    Field:  S    H1   H2   H3   H4   PH   V1   V2   V3   V4   PV   K    END
    Value: 0x57 0x3? 0x3? 0x3? 0x3? 0x0? 0x3? 0x3? 0x3? 0x3? 0x0? 0x?F 0x20
    ===========================================================================

    S       : Start byte. This is always 0x57 ('W')
    H1-H4   : Azimuth as ASCII characters 0-9
    PH      : Azimuth resolution in pulses per degree (ignored)
    V1-V4   : Elevation as ASCII characters 0-9
    PV      : Elevation resolution in pulses per degree (ignored)
    K       : Command (0x0F=stop, 0x1F=status, 0x2F=set)
    END     : End byte. This is always 0x20 (space)


    Positions are encoded as number of pulses in ASCII numbers '0000'-'9999'.


    RESPONSE PACKET
    ===========================================================================
    Byte :  0    1    2    3    4    5    6    7    8    9    10   11
    Field:  S    H1   H2   H3   H4   PH   V1   V2   V3   V4   PV   END
    Value: 0x57 0x0? 0x0? 0x0? 0x0? 0x0? 0x0? 0x0? 0x0? 0x0? 0x0? 0x20
    ===========================================================================

    S       : Start byte. This is always 0x57 ('W')
    H1-H4   : Azimuth as byte values
    PH      : Azimuth resolution in pulses per degree (controller settings)
    V1-V4   : Elevation as byte values
    PV      : Elevation resolution in pulses per degree (controller settings)
    END     : End byte. This is always 0x20 (space)


    Positions are decoded using the following:

        az = (H1 * 100) + (H2 * 10) + H3 + (H4 / 10) - 360
        el = (V1 * 100) + (V2 * 10) + V3 + (V4 / 10) - 360

    The PH and PV values in the response packet reflect the settings of the rotator
    controller. The Rot2Prog supports the following resolutions (same for az and el)

    -----------------------------
    Deg/pulse    PH      PV
        1       0x01    0x01
      0.5       0x02    0x02
     0.25       0x04    0x04
    -----------------------------
    """
    def __init__(self, port, resol=RES_ONE):
        super(Rot2ProgController, self).__init__()

        self.stopbits = serial.STOPBITS_ONE
        self.bytesize = serial.EIGHTBITS
        self.parity = serial.PARITY_NONE
        self.baudrate = 600
        self.resol = resol
        self.port = port

    def get_serial_conn(self):
        """
        Get a serial connection parameterized for the Rot2Prog controller

        :return:
        """
        return serial.Serial(port=self.port, baudrate=self.baudrate,
                             bytesize=self.bytesize, parity=self.parity,
                             stopbits=self.stopbits)

    def set(self, h_vals, v_vals):
        """
        Set the rotator to a given position.

        The controller does not send a response to this command. Azimuth and
        elevation is calculated as number of pulses, with a +360 degree offset
        (so that negative position can be encoded with positive numbers).

        H = PH * (360 + az)
        V = PV * (360 + el)

        H1-H4 and V1-V4 are these numbers encoded as ASCII
        (0x30-0x39, i.e., '0'-'9').

        :return:
        """
        ser = self.get_serial_conn()

        packet = [BYTE_S] + h_vals + [self.resol] + v_vals + [self.resol] + [BYTE_SET] + [BYTE_E]
        cmd = ''.join(chr(x) for x in packet)

        # TODO: Once logging is in place, validate packet send size
        ser.write(cmd)
        ser.close()

    def stop(self):
        """
        Stop the rotator in the current position.

        The stop command stops the rotator immediately in the current position and
        returns the current position. (The position returned does not seem to be
        entirely correct, often off by a degree or two.)

        The H1-H4, PH, V1-V4 and PV fields are ignored, so only the S, K and END
        fields are used.

        :return:
        """
        ser = self.get_serial_conn()

        packet = [BYTE_S] + [BYTE_I] * 10 + [BYTE_STOP] + [BYTE_E]
        cmd = ''.join(chr(x) for x in packet)

        # TODO: Once logging is in place, validate packet send size
        ser.write(cmd)
        response = ser.read(RESP_SIZE)
        ser.close()

        return response

    def status(self):
        """
        Get the current position of the antenna.

        The H1-H4, PH, V1-V4 and PV fields are ignored, so only the S, K and END
        fields are used.

        :return:
        """
        ser = self.get_serial_conn()

        packet = [BYTE_S] + [BYTE_I] * 10 + [BYTE_STAT] + [BYTE_E]
        cmd = ''.join(chr(x) for x in packet)

        # TODO: Once logging is in place, validate packet send size
        ser.write(cmd)
        response = ser.read(RESP_SIZE)
        ser.close()

        return response

