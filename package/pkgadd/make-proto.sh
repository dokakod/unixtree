# ------------------------------------------------------------------------
# Procedure to create the prototype file
#

# ------------------------------------------------------------------------
# collect arguments
#
if [ $# -ne 1 ]
then
	echo "usage: $0 <del-dir>" >&2
	exit 1
fi

DEL_DIR=$1

# ------------------------------------------------------------------------
# output install files
#
echo "i pkginfo=pkginfo"
echo "i copyright=copyright"
echo "i request=request.sh"

# ------------------------------------------------------------------------
# now output list of all files & directories
#
(cd $DEL_DIR; find . -print) |
sort |
sed -e 's,./*,,' |
while read file
do
	if [ "$file" = "" ]
	then
		: #echo "d none \$BASEDIR 755 bin bin"

	elif [ -d $DEL_DIR/$file ]
	then
		echo "d none \$BASEDIR/$file 775 bin bin"

	elif [ -x $DEL_DIR/$file ]
	then
		echo "f none \$BASEDIR/$file=$DEL_DIR/$file 775 bin bin"

	elif [ -f $DEL_DIR/$file ]
	then
		echo "f none \$BASEDIR/$file=$DEL_DIR/$file 664 bin bin"
	fi
done
