# ------------------------------------------------------------------------
# makefile to create a pkgadd file
#

# ------------------------------------------------------------------------
# name of target
#
NAME	= $(PACKAGE)

# ------------------------------------------------------------------------
# actual target to be built
#
PKGINST	= $(PROGRAM)-$(SKU).pkg

TARGET	= $(PKGINST).Z

# ------------------------------------------------------------------------
# list of source files
#
INSTALL_FILES	= \
	pkginfo.txt \
	request.sh

SCRIPTS	= \
	make-pkgadd.sh \
	make-proto.sh

SRC_LIST	= makefile $(INSTALL_FILES) $(SCRIPTS)

# ------------------------------------------------------------------------
# build targets
#
all deliver install :

# ------------------------------------------------------------------------
# package target
#
package : info cpypkg clean

info :
	@echo "--> Creating $(PROGRAM)-$(SKU).pkg.Z in $(SKU_DIR)"

$(TARGET) : $(PKGINST)
	compress -fc $(PKGINST) > $@

$(PKGINST) :
	make-pkgadd.sh $(NAME) $(PKGINST)

cpypkg : $(TARGET)
	@[ -d $(SKU_DIR) ] || mkdir -p $(SKU_DIR)
	cp $(TARGET) $(SKU_DIR)

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	rm -f $(TARGET) $(PKGINST) prototype copyright pkginfo
	rm -rf $(NAME)
