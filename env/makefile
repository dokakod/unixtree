# ------------------------------------------------------------------------
#	makefile for env directory
#
#	This directory contains "env" files for configuring the
#	environment for different platforms.  These files are
#	scripts which are "sourced" by the top-level "build" script.
#
#	All variables defined in these scripts are automatically
#	exported.
#
#	A file is referenced by the environment variable $PLATFORM.
#	The "build" script will source the file "env/$PLATFORM.env".
#
#	See the file "build" for a description of the contents of an "env" file.
#
#	If you add a new env file, please include a "# platform: ..." line
#	in it for the "build -help" mechanism.
#
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# source files
#
ENV_FILES	= *.env

SRC_LIST	= makefile $(ENV_FILES)

# ------------------------------------------------------------------------
# build targets
#
all :

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver :

# ------------------------------------------------------------------------
# targets to be installed
#
install :

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
	rm -f $(OS_CLEAN)
