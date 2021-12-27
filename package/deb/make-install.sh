# ------------------------------------------------------------------------
# create an "install root" and a list of files
#

# ------------------------------------------------------------------------
# install script
#
ROOT=$1
PREFIX=$2
PROGDIR=$PROGRAM

# ------------------------------------------------------------------------
# create initial build-root
#
mkdir -p $ROOT$PREFIX
mkdir -p $ROOT/DEBIAN

# ------------------------------------------------------------------------
# copy over all files from deliver directory into $PREFIX/$PROGDIR
#
mkdir -p $ROOT$PREFIX/$PROGDIR
cp -r $DST_DIR/* $ROOT$PREFIX/$PROGDIR

# ------------------------------------------------------------------------
# copy man pages to $PREFIX/man
#
mkdir -p $ROOT$PREFIX/man/man1
cp $ROOT$PREFIX/$PROGDIR/man/man1/$PROGRAM.1 $ROOT$PREFIX/man/man1

# ------------------------------------------------------------------------
# add scripts to $PREFIX/bin
#
mkdir -p $ROOT$PREFIX/bin

(
	echo "exec $PREFIX/$PROGDIR/${PROGRAM} \$*"
) >$ROOT$PREFIX/bin/${PROGRAM}
chmod 775 $ROOT$PREFIX/bin/${PROGRAM}
