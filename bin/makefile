# ------------------------------------------------------------------------
# makefile for bin directory
#
# The following programs are built:
#
#	chknewer		Check if a file is "newer" than another file.
#					This is used by "deliver.sh" to determine if a copy
#					is needed.
#
#	ic				A program to insert comment lines in source text.
#
# The following scripts are used in the build process:
#
#	guess.sh		a script to try to guess the platform
#
#	deliver.sh		deliver a file to a "delivery" directory
#	hdrcomp.sh		compare the contents of two header files
#	makelib.sh		add object files to a library
#	makesku.sh		package up files in a directory into an SKU (in $SKU_DIR)
#	textsub.sh		substitute env-var values for tokens in a text file
#
# The following "helper" scripts are referenced by various makefiles,
# but are not needed to just do a build:
#
#	cms.sh			front-end script for the CM system in use (I use perforce)
#	fsort.sh		sort a list of files
#	listsrc.sh		product a source-file list
#	xrflist.sh		create a xref-list of all objects in a source tree
#	xrfobjs.sh		produce a cross-ref list of object files (used by xrflist)
#
# The following are "extra" scripts:
#
#	linksrc.sh		produce symlink copies of all files in a directory
#	makesrc.sh		produce symlink copies of all files in an entire tree
#	mkcurses.sh		produce a copy of the ecurses sources
#
# Note that all programs & scripts here will produce a usage message
# if invoked with a "-?" or "-help" option.
#

# ------------------------------------------------------------------------
# build targets
#
TARGET	= \
	chknewer$(EXE_EXT) \
	ic$(EXE_EXT)

# ------------------------------------------------------------------------
# cmd options
#
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LDFLAGS	= $(LDOPTS) $(OPTS) $(LDDEFS)

# ------------------------------------------------------------------------
# build scripts
#
BLD_SCRIPTS	= \
	guess.sh \
	deliver.sh \
	hdrcomp.sh \
	makelib.sh \
	makesku.sh \
	textsub.sh

# ------------------------------------------------------------------------
# helper scripts
#
HLP_SCRIPTS	= \
	cms.sh \
	fsort.sh \
	listsrc.sh \
	xrflist.sh \
	xrfobjs.sh

# ------------------------------------------------------------------------
# extra scripts
#
EXT_SCRIPTS	= \
	linksrc.sh \
	makesrc.sh \
	mkcurses.sh

# ------------------------------------------------------------------------
# C source files
#
CSRC	= \
	chknewer.c \
	ic.c \
	getopt.i

# ------------------------------------------------------------------------
# source list
#
SRC_LIST	= makefile \
	$(BLD_SCRIPTS) \
	$(HLP_SCRIPTS) \
	$(EXT_SCRIPTS) \
	$(CSRC)

# ------------------------------------------------------------------------
# build targets
#
all : $(TARGET)

chknewer$(EXE_EXT) : chknewer.$(OBJ_EXT)
	$(CC) $(LDFLAGS) $? -o $@

ic$(EXE_EXT) : ic.$(OBJ_EXT)
	$(CC) $(LDFLAGS) $? -o $@

# ------------------------------------------------------------------------
# targets to be delivered
#
.PHONY : deliver install

deliver : all

# ------------------------------------------------------------------------
# targets to be installed
#
install : all

# ------------------------------------------------------------------------
# delete any linked executables
#
notarget :

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	rm -f *.$(OBJ_EXT) $(TARGET) $(OS_CLEAN)

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
