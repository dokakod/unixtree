# ------------------------------------------------------------------------
#	support makefile
#
#	This makefile just delivers various support files.
#
#	The following files are used by the program:
#
#		$(PROGRAM).dev		platform-specific device table
#		$(PROGRAM).fc		default file-color file
#		$(PROGRAM).mnu		default application-menu table
#
#	The $(PROGRAM).dev file delivered is obtained from the
#	$(DEVTBL).dev file.  This value is set by the build mechanism
#	and defaults to $(PLATFORM).  It is not delivered if no matching
#	file is found.
#
#	The following files are delivered as informational files only:
#
#		CHANGES				the changes log
#		FILES				list of all files delivered
#		LICENSE				a file containing license info
#		COPYING				a copy of the GPL (GNU General Public License)
#		INSTALL				install instructions
#		TERMFILES			a file documenting term-files
#
#		sample.rc			a sample $(PROGRAM).rc file which
#							contains all default values
#
#	The files CHANGES, FILES, LICENSE, INSTALL, & TERMFILES are also
#	delivered directly to the SKU directory via "make sku".
#

# ------------------------------------------------------------------------
# support files which are built
#
SUPPORT_FILES	= \
	LICENSE \
	INSTALL-UNIX \
	INSTALL-WINDOWS \
	FILES-UNIX \
	FILES-WINDOWS \
	FILES-UTILS \
	FILES-ECURS \
	TERMFILES-PGM \
	TERMFILES-LIB \
	sample.rc

# ------------------------------------------------------------------------
# source files
#
DEV_FILES	= *.dev

SUPPORT_SRC	= \
	license.txt \
	install-unix.txt \
	install-windows.txt \
	files-unix.txt \
	files-windows.txt \
	files-utils.txt \
	files-ecurs.txt \
	termfiles.txt \
	sample.txt

SRC_LIST	= \
	makefile \
	pgm.mnu \
	pgm.fc \
	$(DEV_FILES) \
	$(SUPPORT_SRC)

# ------------------------------------------------------------------------
# build targets
#
all : $(SUPPORT_FILES)

LICENSE : license.txt
	$(TEXTSUB) -l $(RESLANG) license.txt          > $@

FILES-UNIX : files-unix.txt
	$(TEXTSUB) -l $(RESLANG) files-unix.txt       > $@

FILES-WINDOWS : files-windows.txt
	$(TEXTSUB) -l $(RESLANG) files-windows.txt    > $@

FILES-UTILS : files-utils.txt
	$(TEXTSUB) -l $(RESLANG) files-utils.txt      > $@

FILES-ECURS : files-ecurs.txt
	$(TEXTSUB) -l $(RESLANG) files-ecurs.txt      > $@

INSTALL-UNIX : install-unix.txt
	$(TEXTSUB) -l $(RESLANG) install-unix.txt     > $@

INSTALL-WINDOWS : install-windows.txt
	$(TEXTSUB) -l $(RESLANG) install-windows.txt  > $@

TERMFILES-PGM : termfiles.txt
	$(TEXTSUB) -l $(RESLANG) -d LIB termfiles.txt > $@

TERMFILES-LIB : termfiles.txt
	$(TEXTSUB) -l $(RESLANG) -d PGM termfiles.txt > $@

# ------------------------------------------------------------------------
# sample rc file with all defaults
#
sample.rc : sample.tmp ../product/$(PROGRAM)$(EXE_EXT)
	../product/$(PROGRAM) -ZD | \
	cat sample.tmp - >$@

sample.tmp : sample.txt
		$(TEXTSUB) -l $(RESLANG) sample.txt >$@

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@$(DELIVER)                   . ../CHANGES ../COPYING LICENSE
	@$(DELIVER)                   . sample.rc
	@$(DELIVER) -n $(PROGRAM).fc  . pgm.fc
	@$(DELIVER) -w                . LICENSE ../COPYING
	@$(DELIVER) -w -n FILES       . FILES-ECURS
	@$(DELIVER) -w -n TERMFILES   . TERMFILES-LIB
	@if [ "$(OS_TYPE)" = "UNIX" ]; \
	then \
		$(DELIVER) -n INSTALL        . INSTALL-UNIX; \
		$(DELIVER) -n FILES          . FILES-UNIX; \
		$(DELIVER) -n TERMFILES      . TERMFILES-PGM; \
		$(DELIVER) -n $(PROGRAM).mnu . pgm.mnu; \
		[ ! -f $(DEVTBL).dev ] || \
		$(DELIVER) -n $(PROGRAM).dev . $(DEVTBL).dev; \
		$(DELIVER) -u                . LICENSE ../COPYING; \
		$(DELIVER) -u -n TERMFILES   . TERMFILES-LIB; \
		$(DELIVER) -u -n FILES       . FILES-UTILS; \
	else \
		$(DELIVER) -n INSTALL        . INSTALL-WINDOWS; \
		$(DELIVER) -n FILES          . FILES-WINDOWS; \
	fi

# ------------------------------------------------------------------------
# targets to be installed
#
install : all
	@$(INSTALL)                   . ../CHANGES ../COPYING LICENSE
	@$(INSTALL)                   . sample.rc
	@$(INSTALL) -n $(PROGRAM).fc  . pgm.fc
	@if [ "$(OS_TYPE)" = "UNIX" ]; \
	then \
		$(INSTALL) -n INSTALL        . INSTALL-UNIX; \
		$(INSTALL) -n FILES          . FILES-UNIX; \
		$(INSTALL) -n TERMFILES      . TERMFILES-PGM; \
		$(INSTALL) -n $(PROGRAM).mnu . pgm.mnu; \
		[ ! -f $(DEVTBL).dev ] || \
		$(INSTALL) -n $(PROGRAM).dev . $(DEVTBL).dev; \
		$(INSTALL)                   . FILES-UTILS; \
	else \
		$(INSTALL) -n INSTALL        . INSTALL-WINDOWS; \
		$(INSTALL) -n FILES          . FILES-WINDOWS; \
	fi

# ------------------------------------------------------------------------
# delete any linked executables
#
notarget :

# ------------------------------------------------------------------------
# copy all support files to SKU directory for posting purposes
# (This target is manually run.)
#
sku : all
	@$(DELIVER) -d $(SKU_DIR) . ../CHANGES
	@$(DELIVER) -d $(SKU_DIR) . ../COPYING
	@$(DELIVER) -d $(SKU_DIR) . LICENSE
	@$(DELIVER) -d $(SKU_DIR) . INSTALL-UNIX
	@$(DELIVER) -d $(SKU_DIR) . INSTALL-WINDOWS
	@$(DELIVER) -d $(SKU_DIR) . FILES-UNIX
	@$(DELIVER) -d $(SKU_DIR) . FILES-WINDOWS
	@$(DELIVER) -d $(SKU_DIR) . FILES-UTILS

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	rm -f $(SUPPORT_FILES) sample.tmp $(OS_CLEAN)
