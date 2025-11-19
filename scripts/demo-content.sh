#!/bin/sh
./build/bin/tests --dist

cp dist/output/tests.fmt example/web-server/pages/public/tests.fmt
cp doc/about.fmt example/web-server/pages/public/index.fmt
cp doc/about.fmt example/web-server/pages/static/about-fmt.txt
cp doc/schedule.fmt example/web-server/pages/static/schedule.fmt
