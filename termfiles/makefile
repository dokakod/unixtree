# ------------------------------------------------------------------------
#	makefile for term-files
#
#	This makefile just delivers the term-files.
#
#	Note that, by default, no term-files are delivered for the
#	Windows platform.
#

# ------------------------------------------------------------------------
# source list
#
TERM_FILES	= *.trm

SRC_LIST	= \
	makefile \
	$(TERM_FILES)

# ------------------------------------------------------------------------
# build targets
#
all :

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@[ "$(OS_TYPE)" != "UNIX" ] || \
	{ \
		$(DELIVER)    trm $(TERM_FILES); \
		$(DELIVER) -w trm $(TERM_FILES); \
	}

# ------------------------------------------------------------------------
# targets to be installed
#
install : all
	@[ "$(OS_TYPE)" != "UNIX" ] || $(INSTALL) trm $(TERM_FILES)

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
