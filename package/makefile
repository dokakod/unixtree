# ------------------------------------------------------------------------
#	package makefile
#
#	This makefile packages up all SKUs for a particular platform
#	and delivers them to the $(SKU_DIR) directory.
#
#	The files to be "packaged" are taken from the deliver directories
#	$(DST_DIR) & $(UTL_DIR), so a "make deliver" must have already have
#	been done prior to running this make.
#
#	The SKU(s) for a platform are as follows:
#
#	Unix:
#
#		$(PROGRAM)-$(SKU).tar	A tar file containing the entire package.
#
#		termutils-$(SKU).tar	A tar file containing all utility programs.
#
#		libecurses-$(SKU).tar	A tar file containing the libecurses package.
#
#		$(PROGRAM)-$(SKU).<ext>	An optional platform-specific install package.
#								Currently, the only platform-specific SKUs are:
#
#								pkgadd	$(PROGRAM)-$(SKU).pkg	SVR4 platforms
#								rpm		$(PROGRAM)-$(SKU).rpm	linux
#
#	Note that all files are compressed before delivering.  The compress
#	program used is "compress" for all platforms except linux, which
#	uses "gzip".  The "rpm" package is not compressed, since it is already
#	in compressed format.
#
#	Windows:
#
#		$(PROGRAM)-$(SKU).zip	A "pkzip" bundle of the package.
#		libecurses-$(SKU).zip	A "pkzip" bundle of the libecurses package.
#

# ------------------------------------------------------------------------
# platform-specific install directories
#
INST_DIRS	= \
	pkgadd \
	rpm

# ------------------------------------------------------------------------
# list of source files
#
SRC_LIST	= \
	makefile

# ------------------------------------------------------------------------
# build targets
#
all deliver install notarget :

sku : archive package

# ------------------------------------------------------------------------
# produce generic SKU
#
archive :
	@[ ! -d $(DST_DIR) ] || $(MAKESKU) $(DST_DIR) $(PROGRAM)-$(SKU)
	@[ ! -d $(UTL_DIR) ] || $(MAKESKU) $(UTL_DIR) termutils-$(SKU)
	@[ ! -d $(WIN_DIR) ] || $(MAKESKU) $(WIN_DIR) libecurses-$(SKU)

# ------------------------------------------------------------------------
# produce platform-specific SKU
#
package :
	@[ "$(INSTPKG)" = "" ] || ( cd $(INSTPKG); $(MAKE) $@ )

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)
	@for dir in $(INST_DIRS); \
	do \
		( cd $$dir; $(MAKE) $@ ) \
	done

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
