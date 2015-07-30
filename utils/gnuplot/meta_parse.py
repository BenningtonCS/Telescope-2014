#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
Command line tool to format SRT output data files to a gnuplot-readable format.
"""
from argparse import ArgumentParser
import time


def info_parse(input_file, output_file):
    """
    Parses information from the input file then formats it so it is gnuplot readable and
    writes the formatted data to a file.

    :param input_file: file name of the file to parse
    :type input_file: str

    :param output_file: file name the formatted data will be saved to
    :type output_file: str

    :return: None
    """
    # open the input file and read the data, line by line, into the input data list. Each line
    # that is read in will be split (by spaces) into a list, and any empty indicies in that list
    # will be filtered out. After this process, the input_data file should be a list of lists.
    with open(input_file, 'rb') as f:
        input_data = [filter(None, line.replace('\n', '').split(' ')) for line in f.readlines()]

    header_info = extract_header_info(input_data)  # get the column labels
    data = extract_values(input_data)              # get the data rows

    # determine the formatting separation. this works by checking for the max size of the string
    # at the i-th index of every list in the data list, then adding an arbitrary spacer distance
    # (which in this case is 5). This max-size data is used to generate a formatter string which
    # denotes left-alignment and ensures column width of max_size+5
    max_vals = []
    for i in range(len(data[0])-1):
        max_val = 0
        for j in range(len(data)-1):
            max_val = max(max_val, len(data[j][i]))
        max_vals.append(str(max_val + 5))

    formatter_string = '{:<' + '}{:<'.join(max_vals) + '}'

    # write the data to the file
    with open(output_file, 'w') as f:
        # write the header to the file
        f.write('#' + formatter_string.format(*header_info) + '\n')

        # write the data to the file
        for data_row in data:
            f.write(' ' + formatter_string.format(*data_row) + '\n')


def extract_header_info(data):
    """
    Extract column header info from file

    :param data:
    :return:
    """
    header_info = []
    for line in data[:2]:
        if 'MHz' in line:
            # We don't use 'MHz', and it messes with our ordering. The solution? Get rid of it!
            line.remove('MHz')
        header_info.extend(line[::2])
    return header_info


def extract_values(data):
    """
    Extract data values from file

    :param data:
    :return:
    """
    values = []
    tmp = []
    for i, line in enumerate(data):
        if i % 4 == 0 or i % 4 == 1:
            if 'MHz' in line:
                # We don't use 'MHz', and it messes with our ordering. The solution? Get rid of it!
                line.remove('MHz')
            tmp.extend(line[1::2])

        if i % 4 == 1:
            values.append(tmp)
            tmp = []

    return values


if __name__ == "__main__":
    # -----------------------
    # Argument Parser Setup
    # -----------------------
    description = 'parser to format data into a gnuplot-readable form'
    in_help = 'name of the file to parse'
    out_help = 'name of the output file. if unspecified, the file will be named in the format: YYYY_MM_DD.hh-mm-ss.txt'

    dlft_out = 'meta_{}.txt'.format(time.strftime("%Y_%m_%d.%H-%M-%S"))

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
    info_parse(args.input_file, args.output)
