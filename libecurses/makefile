# ------------------------------------------------------------------------
#	libecurses make file
#
#	Targets:
#		libecurses		extended curses library
#
#	This makefile creates a directory with all necessary headers &
#	libraries to use the extended curses package.
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libcurses

TARGET_HDRS	= \
	curses.h \
	term.h \
	termcap.h \
	sysmem.h \
	sysopt.h \
	syssig.h \
	systhread.h

TARGET_LIBS	= \
	$(LIB_TGT)/$(NAME).$(LIB_EXT)

TARGETS	= \
	$(TARGET_HDRS) \
	$(TARGET_LIBS)

# ------------------------------------------------------------------------
# list of all source files
#
SRC_LIST	= makefile

# ------------------------------------------------------------------------
# build targets
#
all : $(TARGETS)

curses.h : ../libwin/curses.h
	cat ../libwin/curses.h    > $@

term.h : ../libwin/term.h
	cat ../libwin/term.h      > $@

termcap.h : ../libwin/termcap.h
	cat ../libwin/termcap.h   > $@

sysmem.h : ../libsys/sysmem.h
	cat ../libsys/sysmem.h    > $@

sysopt.h : ../libsys/sysopt.h
	cat ../libsys/sysopt.h    > $@

syssig.h : ../libsys/syssig.h
	cat ../libsys/syssig.h    > $@

systhread.h : ../libsys/systhread.h
	cat ../libsys/systhread.h > $@

$(LIB_TGT)/$(NAME).$(LIB_EXT) : \
	../lib/libsys.$(LIB_EXT) \
	../lib/libterm.$(LIB_EXT) \
	../lib/libtcap.$(LIB_EXT) \
	../lib/libwin.$(LIB_EXT)
	@rm -f $@
	$(MAKELIB) $@ \
		../libsys/*.$(OBJ_EXT) \
		../libterm/*.$(OBJ_EXT) \
		../libtcap/*.$(OBJ_EXT) \
		../libwin/*.$(OBJ_EXT)

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@$(DELIVER) -w include $(TARGET_HDRS)
	@$(DELIVER) -w lib $(TARGET_LIBS)
	@$(DELIVER) -w man/man3 man3/ecurs*

# ------------------------------------------------------------------------
# targets to be installed
#
install : all

# ------------------------------------------------------------------------
# delete any linked executables
#
notarget :

# ------------------------------------------------------------------------
# lint all source files
#
lint:

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)
	@(cd man3; $(MAKE) $@)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	rm -f $(TARGETS) $(OS_CLEAN)
