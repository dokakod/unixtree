# ------------------------------------------------------------------------
#	makefile for man pages
#
#	Note that normally the man pages are not built, since not
#	all UNIXes have the man macros and nroff installed.
#
#	Instead, we build the man pages (on Sun) and check the built
#	pages into the source.
#
#	Note that the "all" target does nothing: to actually make the manpages
#	run "make manpages".
#
#	We deliver two different versions of the man pages:
#
#		man/man1/$(PROGRAM).1		man pages in "man" format
#		catman/man1/$(PROGRAM).1	man pages in "cat" format
#
#	We don't currently deliver an HTML version of the man pages.  Sorry.
#
#	Since this is a program, the man pages are delivered to the "man1"
#	sub-directory, since section 1 of man is for programs.
#

# ------------------------------------------------------------------------
# target names
#
TARGET		= pgm.man pgm.cat

# ------------------------------------------------------------------------
# source list
#
SRC_LIST	= \
	makefile man.txt \
	$(TARGET)

# ------------------------------------------------------------------------
# build targets
#
all :

manpages : $(TARGET)

pgm.man :	man.txt
	rm -f $@
	$(TEXTSUB) -l $(RESLANG) man.txt > $@

pgm.cat :	pgm.man
	rm -f $@
	nroff -man pgm.man | \
	sed -e "s/.//g" > $@

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@if [ "$(OS_TYPE)" = "UNIX" ]; \
	then \
		$(DELIVER) -n $(PROGRAM).1 man/man1    pgm.man; \
		$(DELIVER) -n $(PROGRAM).1 catman/man1 pgm.cat; \
	else \
		$(DELIVER) -n $(PROGRAM)-doc.txt . pgm.cat; \
	fi

# ------------------------------------------------------------------------
# targets to be installed
#
install : all
	@[ "$(OS_TYPE)" != "UNIX" ] || \
	{ \
		$(INSTALL) -n $(PROGRAM).1 man/man1    pgm.man; \
		$(INSTALL) -n $(PROGRAM).1 catman/man1 pgm.cat; \
	}

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
