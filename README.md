# ILDA Analyzer

A simple CLI tool for analyzing [ILDA files](https://www.ilda.com/resources/StandardsDocs/ILDA_IDTF14_rev011.pdf).

It displays information about the ILDA frames stored in an ILDA file and prints warnings and errors if the file does not conform to the standard.

## Building

Just run `make`. An executable called `ilda-analyzer` will be created in the `build` directory.

## Usage

Run `./ilda-analyzer <FILENAME>` where `<FILENAME>` is the name or the ILDA file you want to analyze.
