#!/bin/sh
./make.sh
now=$(date +%s)
human=$(date)
mkdir -p dist
out=dist/test_run.txt

echo "Caneka Tests run $now ($human)" > $out 
git log -n 1 | head -n 1 >> $out
./build/caneka test=no-color 1>&1 >> $out
if [ $? -eq 0 ]; then
    echo "PASS"
else
    echo "FAIL"
    cat $out
fi
echo "Test output sent to $out"
