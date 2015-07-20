To run both `meta_parse` and `spectrum_parse` scripts via the command line, only an input file must be specified. An output file name may also be specified as an optional argument.

```
$ python meta_parse.py test_data.rad
$ python spectrum_parse.py test_data.rad
```

if no output file name is specified, a default file name will be given to the output file. For the `meta_parse` script, the output filename format is given by:
```
meta_YYYY_MM_DD.hh-mm-ss.txt
```
similarly, for the `spectrum_parse`, the default is
```
spec_YYYY_MM_DD.hh-mm-ss.txt
```

to specify an output filename, simply add the `-o` or `--output` argument followed by the desired filename

```
$ python meta_parse.py test_data.rad -o meta_output.txt
```

to view the help for the scripts, use the `-h` or `--help` flags

```
$ python spectrum_parse.py -h
```
