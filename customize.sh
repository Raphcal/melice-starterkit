#!/bin/sh

if [[ $# -ne 1 ]]; then
    echo "Usage: customize.sh <Game Name>" >&2
    exit 2
fi

DIR=`dirname $0`
echo "Will rename the starter kit as $1"

echo "# $1\n\nEnter description here.\n" > README.md

NAME=`echo $1 | tr -d ' '`

for file in Makefile CMakeLists.txt; do
    sed -i '' -e "s/RenameMe/$NAME/g" "$DIR/$file"
done

sed -i '' -e "s/melice-starterkit.xcodeproj/$1.xcodeproj/g" "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/melice-starterkit Simulator.xcscheme" "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/melice-starterkit Playdate.xcscheme"
sed -i '' -e "s/melice-starterkit/$NAME/g" "$DIR/melice-starterkit.xcodeproj/project.pbxproj" "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/melice-starterkit Simulator.xcscheme" "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/melice-starterkit Playdate.xcscheme"
sed -i '' -e "s/RenameMe/$NAME/g" "$DIR/melice-starterkit.xcodeproj/project.pbxproj" "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/melice-starterkit Simulator.xcscheme" "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/melice-starterkit Playdate.xcscheme"
mv "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/melice-starterkit Simulator.xcscheme" "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/$NAME Simulator.xcscheme"
mv "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/melice-starterkit Playdate.xcscheme" "$DIR/melice-starterkit.xcodeproj/xcshareddata/xcschemes/$NAME Playdate.xcscheme"
rm -rf "$DIR/melice-starterkit.xcodeproj/xcuserdata"
rm -rf "$DIR/project.xcworkspace/xcuserdata"
mv "$DIR/melice-starterkit.xcodeproj" "$DIR/$1.xcodeproj"

if [[ ! `grep "melice" "$DIR/.git/config"` == '' ]]; then
    echo "Melice-Starterkit git repository detected. Creating new git repository..."
    rm -rf "$DIR/.git"
    git init
fi
echo "Done"
rm "$DIR/customize.sh"
