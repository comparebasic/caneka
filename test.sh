#!/bin/sh
./make.sh
now=$(date +%s)
human=$(date)
mkdir -p dist
out=dist/test_run.html

echo "<div class=\"test-run\"><h2>Caneka Tests run $now ($human)</h2>" > $out 
echo "<p>" >> $out
git log -n 1 | head -n 1 >> $out
echo "</p>" >> $out
./build/caneka test=html 1>&1 >> $out
if [ $? -eq 0 ]; then
    echo "PASS"
else
    echo "FAIL"
    cat $out
fi
echo "</div>" >> $out 
echo "Test output sent to $out"
