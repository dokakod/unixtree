# ------------------------------------------------------------------------
#	makefile for curses man pages
#
#	This is the man pages for the extended curses library.
#	These pages are included as a reference to users who are
#	using this package.
#
#	The "all" target will create symbolic links to all section
#	entries for each ecurses function.
#

# ------------------------------------------------------------------------
# source list
#
MAN_FILES	= ecurses ecurs_*
SCRIPTS		= ecurs-funcs.sh

SRC_LIST	= makefile $(MAN_FILES) $(SCRIPTS)

# ------------------------------------------------------------------------
# build targets
#
all : addch
	@ecurs-funcs.sh -av

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all

# ------------------------------------------------------------------------
# targets to be installed
#
install : all

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	ecurs-funcs.sh -d
	rm -f $(OS_CLEAN)
