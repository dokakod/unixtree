# ------------------------------------------------------------------------
# makefile to create linux deb files
#

# ------------------------------------------------------------------------
# deb file & actual target to be built
#
DEBFILE	= $(PACKAGE)-$(VERSION).$(RELEASE)_$(ARCH).deb

TARGET	= $(SKU_DIR)/$(DEBFILE)

# ------------------------------------------------------------------------
# list of source files
#
DEB_FILES	= \
	make-install.sh \
	make-control.sh \
	deb-skel.txt

SRC_LIST	= makefile $(DEB_FILES)

# ------------------------------------------------------------------------
# other macros
#
ROOT	= /tmp/deb.root
CONTROL	= $(ROOT)/DEBIAN/control

PREFIX	= /usr/local

# ------------------------------------------------------------------------
# target rules
#
all deliver install :

# ------------------------------------------------------------------------
# package target
#
package : info $(TARGET) clean

info :
	@echo "--> Creating $(PROGRAM)-$(SKU).deb in $(SKU_DIR)"

$(TARGET) : $(DEBFILE)
	@[ -d $(SKU_DIR) ] || mkdir -p $(SKU_DIR)
	cp $? $@

$(DEBFILE) : $(CONTROL)
	dpkg-deb --build $(ROOT) $(DEBFILE)

$(CONTROL) :
	make-install.sh $(ROOT) $(PREFIX)
	make-control.sh deb-skel.txt > $@

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	-rm -f $(DEBFILE)
	-rm -rf $(ROOT)
