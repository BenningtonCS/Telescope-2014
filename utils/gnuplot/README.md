# utilities for srtn+gnuplot
_note: these scripts are Python 2.7 compatible, though not yet Python 3.x compatible._

This folder contains a collection of scripts which are used to manipulate srtn output
data to format it in a particular way that is compatible with the gnuplot tool. Below
is a summary of the usage information for each of the scripts. The same information
can be found by running each script with the `-h` or `--help` flag

In the `examples` subdirectory exist example output files generated from these scripts.

## Overview
The scripts in this directory are all run basically the same way. They take a file name
as input, and have an optional input argument for the output file name. If no output file
name is specified, a default one will be chosen, which will take the form:
```
  <PARSER_IDENTIFIER>_YYYY_MM_DD.hh-mm-ss.txt
```
To specify the output file name, use the `-o` or `--output` flag when running the script.
The help information for each script can be viewed by using the `-h` or `--help` flag.

The files generated from running the parse scripts are formatted such that they
work with gnuplot. Once all desired files are converted from srtn-format to the
gnuplot-compatible format, they should be ready for use with gnuplot (see wiki entry
on how to use gnuplot)

### Examples
Running a script with no optional arguments
```
$ python meta_parse.py test_data.rad
```

Running a script, specifying the output filename
```
$ python meta_parse.py test_data.rad -o meta_output.txt
```

View the help information for a parse script
```
$ python spectrum_parse.py -h
```

## Scripts
### meta_parse.py
```
usage: meta_parse.py [-h] [-o OUTPUT] input_file

parser to format data into a gnuplot-readable form. meta_parse extracts the
meta-information recorded for each observation in the data file. the output
fileit generates contains rows for every observation with associated meta-
information and columns corresponding to each bit of meta-information, e.g.:

date, obsn, az, el, freq_MHz, Tsys, Tant, vlsr, glat, glon, source, Fstart,
fstop, spacing, bw, fbw, nfreq, nsam, npoint, integ, sigma

positional arguments:
  input_file            name of the file to parse

optional arguments:
  -h, --help            show this help message and exit
  -o OUTPUT, --output OUTPUT
                        name of the output file. if unspecified, the file will
                        be named in the format: YYYY_MM_DD.hh-mm-ss.txt
```


### rot_curve_spec_parse.py
```
usage: rot_curve_spec_parse.py [-h] [-o OUTPUT] input_file

parser to format srtn spectrum data into a gnuplot-readable form. this parses
a file for rotation curve surveys. it averages all spectra taken at a single
point in the galactic plane to produce an average spectrum for that point. the
velocities associated with frequencies red/blueshifted from the center freq.
of 1420.406 MHz are also calculated.


positional arguments:
  input_file            name of the file to parse

optional arguments:
  -h, --help            show this help message and exit
  -o OUTPUT, --output OUTPUT
                        name of the output file. if unspecified, the file will
                        be named in the format: YYYY_MM_DD.hh-mm-ss.txt
```


### hi_spec_parse.py
```
usage: HI_spec_parse.py [-h] [-o OUTPUT] input_file

parser to format srtn spectrum data into a gnuplot-readable form. this parses
a file for rotation curve surveys. it prints the column numbers of the last
spectrum taken before the telescope moved to the next point. the velocities
associated with frequencies red/blueshifted from the center freq. of 1420.406
MHz are also calculated.

positional arguments:
  input_file            name of the file to parse

optional arguments:
  -h, --help            show this help message and exit
  -o OUTPUT, --output OUTPUT
                        name of the output file. if unspecified, the file will
                        be named in the format: YYYY_MM_DD.hh-mm-ss.txt
```


### spectrum_parse.py
```
usage: spectrum_parse.py [-h] [-o OUTPUT] input_file

parser to format data into a gnuplot-readable form. spectrum_parse extracts
the recorded spectrum power for each observed frequency. the output file it
generates contains rows for every frequency observed with columns containing
the observed power for each observation.

positional arguments:
  input_file            name of the file to parse

optional arguments:
  -h, --help            show this help message and exit
  -o OUTPUT, --output OUTPUT
                        name of the output file. if unspecified, the file will
                        be named in the format: YYYY_MM_DD.hh-mm-ss.txt
```
