# ------------------------------------------------------------------------
# Procedure to get install directory
#
# Note: all names in the pkginfo file are available as env vars here.
#
echo ""
echo "Enter the directory where you want to install $NAME:"

MESSAGE="directory [default = $BASEDIR]"
BASEDIR=`ckpath -d $BASEDIR -aty -p "$MESSAGE"` || exit $?

(
	echo "BASEDIR=$BASEDIR"
) >$1

exit 0
