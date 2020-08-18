# Tdq

This is a clone of `tdq` from the [LITCAVE](https://litcave.rudi.ir/).
I just adapted it for my own setup, feel free to steal the converted dictionaries (the repo is a little under 100 MB, in case you want to clone it)<br>

## Original README:
# TDQ

Tdq is a fast dictionary lookup program for tinydict dictionary files, which can be created from dictionaries with stardict or xdxf format or Arpabet pronunciation listings.  The following programs are included:

- tdq: look up words in tinydict files
- tdi: create tinydict index files from tinydict definition files
- tds: sort tinydict definition files
- xd2td: convert xdxf files to tinydict
- sd2td: convert stardict files to tinydict
- ab2td: convert Arpabet pronunciations to tinydict as IPA

For importing xdxf files, these steps are necessary (dict.xdxf is the
input xdxf dictionary):

    cat dict.xdxf | xd2td | tds | sed 's/&amp;/\&/g' >dict.td
    tdi <dict.td >dict.ti

Importing stardict files is quite similar (sdict.dz is a stardict
definition file and sdict.idx is its index file):

    zcat sdict.dz | sd2td sdict.idx | tds >dict.td
    tdi <dict.td >dict.ti

Arpabet pronunciations, such as CMUDict or BEEP, can be converted to
IPA and imported with these commands (dict.ab lists words with their
Arpabet pronunciations):

    cat dict.ab | ab2td | tds >dict.td
    tdi <dict.td >dict.ti

Now the dicts[] array in tdq.c should be edited to add the new
dictionary files:

	 #define DICTROOT	"/my/dicts/dir/"

	 static struct dictpath {
	       char *td;	/* tinydict file */
	       char *ti;	/* tinydict index; tdi output */
	       char *sign;	/* dict sign characters */
	 } dicts[] = {
	       {DICTROOT "dict.td", DICTROOT "dict.ti", "d"},
	 };

The third field can be used to select the dictionary to search.
After compiling you can use tdq:

    tdq word		# look up word in default dictionaries (m sign)
    tdq word*		# list of all words starting with word
    tdq *word*		# list of all words containing word
    tdq / word		# search word in all dictionary files
    tdq s word		# search word in dictionaries with sign s
