# ------------------------------------------------------------------------
#	Top-level make file
# ------------------------------------------------------------------------
#
#	Main targets:
#
#		all				makes everything (default target)
#		install			makes & installs everything to $INS_DIR
#		deliver			makes & delivers everything to the package dirs
#		bundle			packages up the package dirs as SKU bundles in $SKU_DIR
#		sku				makes & delivers everything in the package dirs and
#						packages them up as SKU bundles in $SKU_DIR
#						(same as "deliver bundle")
#
#	Various clean targets:
#
#		notarget		deletes all linked executables
#		clean			cleans everything
#		clean-dist		cleans all distribution directories
#
#	The following targets *should* all produce the same list of source files:
#
#		srclist			produces sorted source list via makefiles
#		fndlist			produces sorted source list via find
#		cmslist			produces sorted source list via CM system
#
#		src.list		same as "make srclist >src.list"
#		fnd.list		same as "make fndlist >fnd.list"
#		cms.list		same as "make cmslist >cms.list"
#
#	Other targets:
#
#		lint			runs lint against all source files
#		srcsku			packages up the source and delivers it to $SKU_DIR
#		xrflist			create a cross-reference list of global names
#		xrf.list		same as "make xrflist >xrf.list"
#
#	Internal targets:
#
#		build-check		check if "build" script has been run
#		deliver-check	check if source has been built & delivered
#
# ------------------------------------------------------------------------
#
#	To build & deliver a release build from scratch, run the cmds:
#
#		$ . build -r <platform>
#		$ make clean sku
#
#	This will result in final SKU packages being delivered to $SKU_DIR
#	(by default $HOME/sku).
#
# ------------------------------------------------------------------------
#
#	Directory list
#
#	The following libraries are sub-libraries for "libecurses":
#
#		libsys			library for basic system functions
#		libterm			library for device I/O functions
#		libtcap			library for termcap/terminfo interface functions
#		libwin			library for "curses" functions
#
#	The following is the replacement library for "libcurses"
#	(built from the sub-libraries above):
#
#		libecurses		extended curses library & headers
#
#	The following libraries are not used, but are included to provide a
#	complete curses package:
#
#		libform			The standard curses forms library.
#		libmenu			The standard curses menu  library.
#		libpanel		The standard curses panel library.
#
#	Stand-alone libraries:
#
#		libxpm			library for pixmap manipulation
#		libxvt			library for xterm emulation
#		libos			library for O/S-dependent functions
#		libdiff			library for file diff functions
#		libgzip			library for gzip functions
#		libmagic		library for magic functions
#
#	Other libraries:
#
#		libutils		library for general-purpose utility functions
#		libftp			library for FTP functions
#		libres			library for resource-file processing
#		libhelp			help file creation - not really a library
#
#	Program directories:
#
#		libprd			library with actual program code
#		product			main program which links with all libraries
#
#	Other directories:
#
#		termutils		terminal-oriented utility testing programs
#		manpages		program man pages for delivery
#		termfiles		contains term-files for delivery
#		support			contains misc "support" files for delivery
#
#		package			produces SKUs (tar/zip files) and any platform-specific
#						install packages.
#
#		bin				misc scripts & programs used during the build
#		env				platform-specific env files sourced by build
#
# ------------------------------------------------------------------------
#
#	File list
#
#	The files in this directory are:
#
#		CHANGES			Changes log
#		COPYING			The standard GPL text
#		README			A text file documenting the build process
#		bldinfo			A script sourced by build to set all build info
#		build			A script to be sourced prior to doing any builds
#		pgminfo			A script sourced by build to set all program info
#		makefile		This file
#
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# bin directory (programs & scripts used in this build)
#
BIN_DIRS	= \
	bin

# ------------------------------------------------------------------------
# extended curses libraries
#
CURSES_LIB_DIRS	= \
	libsys \
	libterm \
	libtcap \
	libwin \
	libecurses \
	libform \
	libmenu \
	libpanel

# ------------------------------------------------------------------------
# stand-alone libraries
#
SA_LIB_DIRS	= \
	$(XPMDIR) \
	libxvt \
	libos \
	libdiff \
	libgzip \
	libmagic

SR_LIB_DIRS	= \
	libxpm \
	libxvt \
	libos \
	libdiff \
	libgzip \
	libmagic

# ------------------------------------------------------------------------
# other libraries
#
OTHER_LIB_DIRS	= \
	libutils \
	libftp \
	libres \
	libhelp

# ------------------------------------------------------------------------
# program directories
#
PGM_DIRS	= \
	libprd \
	product

# ------------------------------------------------------------------------
# terminal utility test programs
#
UTIL_DIRS	= \
	termutils

# ------------------------------------------------------------------------
# support directories
#
SUPPORT_DIRS	= \
	termfiles \
	manpages \
	support

# ------------------------------------------------------------------------
# SKU packaging directories
#
PACKAGE_DIRS	= \
	package

# ------------------------------------------------------------------------
# misc directories which are not built
#
NOBUILD_DIRS	= \
	env

# ------------------------------------------------------------------------
# list of sub-directories which are built
#
BUILD_DIRS	= \
	$(BIN_DIRS) \
	$(CURSES_LIB_DIRS) \
	$(SA_LIB_DIRS) \
	$(OTHER_LIB_DIRS) \
	$(PGM_DIRS) \
	$(UTIL_DIRS) \
	$(SUPPORT_DIRS)

# ------------------------------------------------------------------------
# list of sub-directories which are linted
#
LINT_DIRS	= \
	$(CURSES_LIB_DIRS) \
	$(OTHER_LIB_DIRS) \
	$(PGM_DIRS)

# ------------------------------------------------------------------------
# list of all sub-directories
#
ALL_DIRS	= \
	$(BIN_DIRS) \
	$(CURSES_LIB_DIRS) \
	$(SR_LIB_DIRS) \
	$(OTHER_LIB_DIRS) \
	$(PGM_DIRS) \
	$(UTIL_DIRS) \
	$(SUPPORT_DIRS) \
	$(PACKAGE_DIRS) \
	$(NOBUILD_DIRS)

# ------------------------------------------------------------------------
# list of source files in this directory
#
FILES	= \
	CHANGES \
	COPYING \
	README

SCRIPTS	= \
	bldinfo \
	build \
	pgminfo

SRC_LIST	= makefile $(SCRIPTS) $(FILES)

# ------------------------------------------------------------------------
# all:			build all directories
# install:		build & install all directories
# deliver:		build & deliver all directories
#
all install deliver : build-check
	@for dir in $(BUILD_DIRS); \
	do \
		echo "=== building $$dir"; \
		( cd $$dir; $(MAKE) $@ ) || exit 1; \
	done
	@echo "=== done"

# ------------------------------------------------------------------------
# notarget:		delete all linked executables
#
notarget : build-check
	@for dir in $(BUILD_DIRS); \
	do \
		echo "=== untargeting $$dir"; \
		( cd $$dir; $(MAKE) $@ ) || exit 1; \
	done
	@echo "=== done"

# ------------------------------------------------------------------------
# lint:			lint all source files (produces lots of output)
#
lint : build-check
	@for dir in $(LINT_DIRS); \
	do \
		echo "=== linting $$dir"; \
		( cd $$dir; $(MAKE) $@ ) \
	done
	@echo "=== done"

# ------------------------------------------------------------------------
# bundle:		produce SKU bundle(s)
#
bundle : build-check deliver-check
	@for dir in $(PACKAGE_DIRS); \
	do \
		( cd $$dir; $(MAKE) sku ) \
	done

# ------------------------------------------------------------------------
# sku:			build & deliver everything and produce SKU bundle(s)
#
sku : deliver bundle

# ------------------------------------------------------------------------
# srcsku:		deliver the source as an SKU
#
srcsku : build-check
	@DIR=$(PROGRAM)-$(VERSION).$(RELEASE); \
	NAM=$(PROGRAM)-source-$(VERSION).$(RELEASE).tar; \
	TGT=$(SKU_DIR)/$$NAM; \
	echo "Getting source list"; \
	LST=`$(MAKE) srclist | sed -e "s,^,$$DIR/,"`; \
	ln -s . $$DIR; \
	echo "Creating $$TGT.Z"; \
	tar chf - $$LST | compress -c >$$TGT.Z; \
	echo "Creating $$TGT.gz"; \
	tar chf - $$LST | gzip     -c >$$TGT.gz; \
	rm $$DIR

# ------------------------------------------------------------------------
# clean:		clean the entire tree (Note that $SKU_DIR is never cleaned)
#
clean : clean-dist
	@for dir in $(BUILD_DIRS); \
	do \
		echo "=== cleaning $$dir"; \
		( cd $$dir; $(MAKE) $@ ) \
	done
	@echo "=== done"

# ------------------------------------------------------------------------
# clean-dist:	clean all distribution directories (except $SKU_DIR)
#
clean-dist : build-check
	rm -rf $(DST_DIR) $(UTL_DIR) $(WIN_DIR)

# ------------------------------------------------------------------------
# srclist:		get list of all files via makefiles (to stdout)
# src.list:		get list of all files via makefiles (to src.list)
#
srclist : build-check
	@( \
		$(LISTSRC) $(SRC_LIST); \
		for dir in $(ALL_DIRS); \
		do \
			( cd $$dir; $(MAKE) srclist ) \
		done \
	) | fsort.sh

src.list :
	@$(MAKE) srclist >$@

# ------------------------------------------------------------------------
# fndlist:		get list of all read-only files via find (to stdout)
# fnd.list:		get list of all read-only files via find (to fnd.list)
#
fndlist :
	@find . \( -type f -a ! -perm -200 \) -print | \
		sed -e '/...*\//s/..//' | \
		fsort.sh

fnd.list :
	@$(MAKE) fndlist >$@

# ------------------------------------------------------------------------
# cmslist:		get list of all files via CM system (to stdout)
# cms.list:		get list of all files via CM system (to cms.list)
#
cmslist : build-check
	@cms.sh -i list | fsort.sh

cms.list :
	@$(MAKE) cmslist >$@

# ------------------------------------------------------------------------
# xrflist:		get xref list of global names in all object files (to stdout)
# xrf.list:		get xref list of global names in all object files (to xrf.list)
#
xrflist : build-check
	@$(XRFLIST) -v

xrf.list :
	@$(MAKE) xrflist >$@

# ------------------------------------------------------------------------
# build-check:	check if "build" script has been run
#
build-check :
	@[ "$(PLATFORM)" != "" ] || \
	{ \
		echo "No build environment setup." \
			"Run \". build <platform>\" first."; \
		exit 1; \
	}

# ------------------------------------------------------------------------
# deliver-check:	check if source was built & delivered
#
deliver-check :
	@[ -d $(DST_DIR) ] || \
	{ \
		echo "No deliver directory found." \
			"Run \"make deliver\" first."; \
		exit 1; \
	}
