# ------------------------------------------------------------------------
# interface script to create libraries
#

# ------------------------------------------------------------------------
# check for help option
#
if [ "$1" = "-?" -o "$1" = "-help" ]
then
	echo "usage: $0 <library> object ..."
	exit 0
fi

# ------------------------------------------------------------------------
# get target library
#
TARGET=$1
shift

# ------------------------------------------------------------------------
# get ARFLAGS
#
[ "$ARFLAGS" = "" ] && ARFLAGS=rc

# ------------------------------------------------------------------------
# check if target directory exists
#
DIR=`dirname $TARGET`
if [ ! -d "$DIR" ]
then
	mkdir -p "$DIR" || exit 1
fi

# ------------------------------------------------------------------------
# add objects to the library
#
ar $ARFLAGS $TARGET $@

# ------------------------------------------------------------------------
# check if ranlib needed
#
[ "$RANLIB" = "" ] || $RANLIB $TARGET
