# this is a spectrum parser for rotation curve surveys specifically -- it prints the column numbers
# of the last spectrum taken before the telescope moved to the next point, and it prints the velocities associated with
# the appropriate red/blueshifted frequency in the second column. None of this would have been possible without
# the help and guidance of E.Daniszewski.

#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Command line tool to format SRT spectrum output data files to a gnuplot-readable format. The
current output format is given by the example below:
freq     pwr1     pwr2     pwr3     pwr4      ...
-----    -----    -----    -----    ----
866.1    467.3    458.2    473.1    470.9     ...
866.2    461.4    465.2    471.9    472.4     ...
 ...      ...      ...      ...      ...
"""
from argparse import ArgumentParser
import time


def spectrum_parse(input_file, output_file):
    """
    Parse out the spectrum data (frequency and pwr)
    :param input_file:
    :param output_file:
    :return:
    """
    # Open the file and extract all the data to a list, split at spaces, and newline stripped
    with open(input_file, 'rb') as f:
        data = [filter(None, line.strip('\n').split(' ')) for line in f.readlines()]

    fstart = float(data[1][1])   # fstart value for the file (assumes single value per file)
    fstop = float(data[1][3])    # fstop value for the file (assumes single value per file)
    spacing = float(data[1][5])  # spacing value for the file (assumes single value per file)

    freq_steps = int(round((fstop - fstart) / spacing))  # get the number of frequency steps
    measure_count = len(data[3])                         # get the number of pwr data points

    # Make sure the number of pwr data points is the same as the expected number of freq steps
    if freq_steps != measure_count:
        raise RuntimeError('Number of frequency steps not equal to number of data points')

    # generate all frequencies
    freqs = [(spacing * i) + fstart for i in range(freq_steps)]

    # get the max string length in the freqs list (this is used for formatting when writing to file)
    max_f = max(len(str(x)) for x in freqs)

    # get the spectrum data from the data list
    spec_data = data[3::4]

    #for HI spectra, generate a velocity corresponding to each frequency in freqs, for doppler shift
    q = 1/1420.406
    c = 299970
    vels = []
    for r in freqs:
        w = 1/r
        vsrc = c*((w-q)/q)
        vels.append(vsrc)

    # write to output file
    with open(output_file, 'w') as f:

        for i, freq in enumerate(freqs):
            # get all power values associated with the current frequency
            freq_pwrs = [str(spec[i]) for spec in spec_data]
            # prepend the frequency to the found powers
            freq_pwrs.insert(0, str(freq))
            # ADD VELOCITIES TO COLUMN AFTER FREQ
            freq_pwrs.insert(1, str(vels[i]))
            # set the format spacing to 15 (arbitrarily) to make even columns
            spacing_size = ['15'] * len(freq_pwrs)
            formatter_string = '{:<' + '}{:<'.join(spacing_size) + '}\n'

            # write out line of data to file
            f.write(formatter_string.format(*freq_pwrs))

    # This function produces of list of numbers that correspond to the column numbers
    # of the spectra you want to plot for rotation curve
    def extract_values(data):
        """
        Extract data values from file

        :param data:
        :return:
        """

        # create an empty list that holds all the data lines that hold "source" data
        tmp = []
        for i, line in enumerate(data):
            if i % 4 == 0:
                tmp.append(line)

        # compare the source on the current line to the source on the previous line
        # if no match, append the index number of the previous source to the list values
        values = []
        for i, line in enumerate(tmp):
            current_value = line[21]
            prev_value = tmp[i-1][21]
            if str(current_value) != str(prev_value):
                values.append(tmp.index(line))

        # add 1 to every number in values to get the correct column numbers in the parsed file
        nvalues = []
        for x in values:
            x = x + 2
            nvalues.append(x)


        print nvalues

    extract_values(data)

if __name__ == "__main__":
    # -----------------------
    # Argument Parser Setup
    # -----------------------
    description = 'parser to format srtn spectrum data into a gnuplot-readable form'
    in_help = 'name of the file to parse'
    out_help = 'name of the output file. if unspecified, the file will be named in the format: YYYY_MM_DD.hh-mm-ss.txt'

    dlft_out = 'spec_{}.txt'.format(time.strftime("%Y_%m_%d.%H-%M-%S"))

    # Initialize instance of an argument parser
    parser = ArgumentParser(description=description)

    # Add required arguments
    parser.add_argument('input_file', help=in_help, type=str)

    # Add optional arguments, with given default values if user gives no args
    parser.add_argument('-o', '--output', default=dlft_out, type=str, help=out_help)

    # Get the arguments
    args = parser.parse_args()

    # -----------------------
    # Parse the data
    # -----------------------
    spectrum_parse(args.input_file, args.output)
