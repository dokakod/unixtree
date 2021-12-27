# ------------------------------------------------------------------------
# makefile to create linux rpm files
#

# ------------------------------------------------------------------------
# rpm file & actual target to be built
#
RPMFILE	= $(PACKAGE)-$(VERSION).$(RELEASE).$(ARCH).rpm

TARGET	= $(SKU_DIR)/$(PROGRAM)-$(SKU).rpm

# ------------------------------------------------------------------------
# list of source files
#
RPM_FILES	= \
	make-spec.sh \
	make-list.sh \
	make-install.sh \
	rpm-skel.txt

SRC_LIST	= makefile $(RPM_FILES)

# ------------------------------------------------------------------------
# other macros
#
ROOT	= /tmp/rpm.root
LIST	= rpm.files
SPEC	= rpm.spec

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
	@echo "--> Creating $(PROGRAM)-$(SKU).rpm in $(SKU_DIR)"

$(TARGET) : $(RPMFILE)
	@[ -d $(SKU_DIR) ] || mkdir -p $(SKU_DIR)
	cp $? $@

$(RPMFILE) : $(SPEC)
	rpmbuild -bb --buildroot $(ROOT) $(SPEC)

$(SPEC) : $(LIST)
	make-spec.sh rpm-skel.txt $(LIST) > $@

$(LIST) :
	make-install.sh $(ROOT) $(PREFIX)
	make-list.sh    $(ROOT) $(PREFIX) > $@

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	-rm -f $(RPMFILE) $(LIST) $(SPEC)
	-rm -rf $(ROOT)
