# ------------------------------------------------------------------------
# create an "install root" and a list of files
#

# ------------------------------------------------------------------------
# install script
#
ROOT=$1
PREFIX=$2

# ------------------------------------------------------------------------
# now create list of files
#
find $ROOT$PREFIX -print |
sed -e "s,$ROOT,," |
while read file
do
	if [ -d $ROOT$file ]
	then
		echo "%attr(775 bin bin) %dir $file"

	elif [ -x $ROOT$file ]
	then
		echo "%attr(775 bin bin)      $file"

	else
		echo "%attr(664 bin bin)      $file"
	fi
done
