# ------------------------------------------------------------------------
# script to create an SKU package
#
# It is assumed that all files in <src-dir> are to be
# packaged up into $SKU_DIR/<name>.ext
#

# ------------------------------------------------------------------------
# check for help option
#
if [ "$1" = "-?" -o "$1" = "-help" ]
then
	echo "usage: $0 <src-dir> <name>"
	exit 0
fi

# ------------------------------------------------------------------------
# collect args
#
if [ $# != 2 ]
then
	echo "$0: invalid arg count" >$2
	echo "usage: $0 <src-dir> <name>" >&2
	exit 1
fi

source_dir="$1"
target_name="$2"

# ------------------------------------------------------------------------
# now do it
#
[ ! -d $SKU_DIR ] && mkdir -p $SKU_DIR

case $PLATFORM in

	# --------------------------------------------------------------------
	# Windows
	#
	winx86)
		if [ "$PKZIP_PROG" != "" ]
		then
			target_file=$target_name.zip
			target_path=$SKU_DIR/$target_file
			rm -f $target_path

			echo "--> creating $target_file in $SKU_DIR"

			(
				cd $source_dir
				$PKZIP_PROG $PKZIP_OPTS $target_path
			)
		fi
		;;

	# --------------------------------------------------------------------
	# Windows (cygwin)
	#
	cygwin | cygwin-gcc)
		target_file=$target_name.zip
		target_path=$SKU_DIR/$target_file
		rm -f $target_path

		echo "--> creating $target_file in $SKU_DIR"

		(
			cd $source_dir
			zip $target_path *
		)
		;;

	# --------------------------------------------------------------------
	# Linux
	#
	linux*)
		target_file=$target_name.tar.gz
		target_path=$SKU_DIR/$target_file
		rm -f $target_path

		echo "--> creating $target_file in $SKU_DIR"

		(
			cd $source_dir
			tar cf - * | gzip -c - >$target_path
		)
		;;

	# --------------------------------------------------------------------
	# All other Unixes
	#
	*)
		target_file=$target_name.tar.Z
		target_path=$SKU_DIR/$target_file
		rm -f $target_path

		echo "--> creating $target_file in $SKU_DIR"

		(
			cd $source_dir
			tar cf - * | compress -c >$target_path
		)
		;;

esac
