# ------------------------------------------------------------------------
# script to sort a list of files
#
# The purpose of this script is to sort a list of files
# such that any *files* precede any *sub-directories*.
#
# Thus, a "normal" sort would product the list:
#
#	makefile
#	sub-dir/foo.c
#	sub-dir/makefile
#	vfoo.c
#
# This sort would product the list:
#
#	makefile
#	vfoo.c
#	sub-dir/foo.c
#	sub-dir/makefile
#
# Also, any lines with no directory will have a "./" prepended to it
# and any lines starting with a "./" that have a sub-directory will
# have the "./" removed.
#

# ------------------------------------------------------------------------
# check for help
#
if [ "$1" != "" ]
then
	case "$1" in

	'-?' | -help | --help)
		echo "usage: $0 [file ...]"
		exit 0
		;;

	esac
fi

# ------------------------------------------------------------------------
# do the sort
#
cat $* |
sed \
	-e '/^\.\/.*\//s/\.\///' \
	-e '/^[^/]*$/s/^/.\//' \
	-e '/\/[^/]*$/s//\/.&/' |
sort |
sed -e 's/\/\.//'
