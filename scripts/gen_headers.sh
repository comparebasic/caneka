#bin/bash

section=base

mfile="include/$section.h";
for mod in $(ls $section); do
    echo $mod;
    mkdir -p "include/$section/$mod";
    sfile="$section/${mod}.h"
    echo "#include \"$sfile\"" >> $mfile;
    touch "include/$sfile";
    for file in $(ls "$section/$mod/"); do
        ffile="$mod/$file";
        echo "== $ffile";
        hfile=$(echo "$ffile" | sed -e 's/\.c/.h/');
        echo "#include \"$hfile\"" >> "include/$sfile";
        cat "$section/$ffile" | grep '^[A-Za-z0-9_]\+\** \+\**[A-Za-z0-9_]\+([-A-Za-z0-9_, .*]*) *{$' | sed 's/{$/;/' >> include/$section/$hfile;
    done;
done
