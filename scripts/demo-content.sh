#!/bin/sh
./build/bin/tests --dist

cp -v dist/output/tests.fmt examples/web-server/pages/public/tests.fmt
cp -v docs/schedule.fmt examples/web-server/pages/public/schedule.fmt
cp -v docs/about.fmt examples/web-server/pages/public/index.fmt
cp -v docs/about.fmt examples/web-server/pages/static/about-fmt.txt
