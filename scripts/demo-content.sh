#!/bin/sh
./build/bin/tests --dist

cp -v dist/output/tests.fmt examples/web-server/pages/public/tests.fmt
cp -v dist/output/tests.fmt examples/web-server/pages/static/tests-fmt.txt
cp -v docs/schedule.fmt examples/web-server/pages/public/schedule.fmt
cp -v docs/about.fmt examples/web-server/pages/public/index.fmt
cp -v docs/about.fmt examples/web-server/pages/static/about-fmt.txt
cp -v examples/web-server/pages/public/stats.templ examples/web-server/pages/static/stats-templ.txt
cp -v examples/web-server/pages/nav.config examples/web-server/pages/static/nav-config.txt
