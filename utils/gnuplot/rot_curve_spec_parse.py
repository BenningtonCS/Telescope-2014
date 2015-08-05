# This is a spectrum parsing file specific for rotation curve surveys. It averages all the spectra taken at a single
# point in the galactic plane to produce an average spectrum for that point. Additionally, it prints the velocities
# associated with frequencies red/blueshifted from the center freq of 1420.406 MHz in the second column. 
# Undying gratitude to E.Daniszewski, guiding light of the development of this mini program.


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

    #for HI spectra, generate a velocity corresponding to each frequency in freqs, for doppler shift
    q = 1/1420.406
    c = 299970
    vels = []
    for r in freqs:
        w = 1/r
        vsrc = c*((w-q)/q)
        vels.append(vsrc)

    # Averaging all the spectra taken at a certain point to produce one avg spectrum for that point
    specsG0 = []
    specsG10 = []
    specsG20 = []
    specsG30 = []
    specsG40 = []
    specsG50 = []
    specsG60 = []
    specsG70 = []
    specsG80 = []
    specsG90 = []

    for i, line in enumerate(data):
        if i % 4 == 0:
            current_value = line[21]
            spec_line = data[i+3]
            if str(current_value) == 'G0':
                specsG0.append(spec_line)
            elif str(current_value) == 'G10':
                specsG10.append(spec_line)
            elif str(current_value) == 'G20':
                specsG20.append(spec_line)
            elif str(current_value) == 'G30':
                specsG30.apped(spec_line)
            elif str(current_value) == 'G40':
                specsG40.append(spec_line)
            elif str(current_value) == 'G50':
                specsG50.append(spec_line)
            elif str(current_value) == 'G60':
                specsG60.append(spec_line)
            elif str(current_value) == 'G70':
                specsG70.append(spec_line)
            elif str(current_value) == 'G80':
                specsG80.append(spec_line)
            elif str(current_value) == 'G90':
                specsG90.append(spec_line)

    nspecG0 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG0)]
    nspecG10 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG10)]
    nspecG20 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG20)]
    nspecG30 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG30)]
    nspecG40 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG40)]
    nspecG50 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG50)]
    nspecG60 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG60)]
    nspecG70 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG70)]
    nspecG80 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG80)]
    nspecG90 = [float(sum([float(val) for val in col]))/len(col) for col in zip(*specsG90)]

    nspecs = [nspecG0, nspecG10, nspecG20, nspecG30, nspecG40, nspecG50, nspecG60, nspecG70, nspecG80, nspecG90]

    # write to output file
    with open(output_file, 'w') as f:

        for i, freq in enumerate(freqs):
            # get all power values associated with the current frequency
            freq_pwrs = [str(spec[i]) for spec in nspecs if spec]
            # prepend the frequency to the found powers
            freq_pwrs.insert(0, str(freq))
            # ADD VELOCITIES TO COLUMN AFTER FREQ
            freq_pwrs.insert(1, str(vels[i]))
            # set the format spacing to 15 (arbitrarily) to make even columns
            spacing_size = ['15'] * len(freq_pwrs)
            formatter_string = '{:<' + '}{:<'.join(spacing_size) + '}\n'

            # write out line of data to file
            f.write(formatter_string.format(*freq_pwrs))



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
