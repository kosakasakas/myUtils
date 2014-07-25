#!/bin/sh

if [ $# -ne 2 ]; then
  echo "can't work." 1>&2
  exit 1
fi
rm -rf ~/tmp
mkdir ~/tmp
touch ~/tmp/list{1...2}
cd $1
find . > ~/tmp/list1
cd $2
find . > ~/tmp/list2
diff -b -U 0 ~/tmp/list1 ~/tmp/list2
rm -rf ~/tmp
echo "Done"
