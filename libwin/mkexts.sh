# ------------------------------------------------------------------------
# Extract out all externs from the ecurses header file and
# match them with their category.
#
# output is sorted by category.
#
file=$1
title=

cat $file |
while read line
do
	echo "$line" | grep '^extern' >$DEV_NULL
	if [ $? -eq 0 ]
	then
		[ "$title" != "" ] &&
		{
			name=`echo "$line" |
				sed -e 's/[ 	]*(.*//' -e 's/^.*[ 	]//'`
			echo "$title	$name"
		}
		continue
	fi

	echo "$line" | grep '(ecurs_' >$DEV_NULL
	if [ $? -eq 0 ]
	then
		title=`echo $line | sed -e 's/.*(//' -e 's/).*//'`
	fi
done |
sort
