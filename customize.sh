#!/bin/sh

DIR=`base $0`
echo "Will rename the starter kit as $1"

for file in Makefile; do
    sed -i "s/RenameMe/$1/g" $DIR/$file
done

rm $DIR/customize.sh
