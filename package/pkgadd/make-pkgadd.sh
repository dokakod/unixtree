# ------------------------------------------------------------------------
# make a pkgadd file
#

# ------------------------------------------------------------------------
# collect arguments
#
if [ $# -ne 2 ]
then
	echo "usage: $0 <name> <package>" >&2
	exit 1
fi

NAME=$1
PKGINST=$2

# ------------------------------------------------------------------------
# create copyright file
#
echo "$COPYRTE" >copyright

# ------------------------------------------------------------------------
# create pkginfo file
#
$TEXTSUB -l $RESLANG pkginfo.txt >pkginfo

# ------------------------------------------------------------------------
# create prototype file
#
./make-proto.sh $DST_DIR >prototype

# ------------------------------------------------------------------------
# now create pkgadd directory
#
pkgmk -a $ARCH -d `pwd` -f prototype

# ------------------------------------------------------------------------
# now create pkgadd file
#
pkgtrans -s `pwd` $PKGINST $NAME
