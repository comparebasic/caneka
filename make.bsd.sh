CC=cc
CFLAGS="-g -Werror -Wno-incompatible-pointer-types-discards-qualifiers"
INC="-I./include"

coreobj="core/caneka core/mem core/string core/error core/debug \
	core/log core/slab core/span core/lookup core/chain core/testsuite \
    core/hash core/table core/compare core/iter core/array core/int \
    core/maker core/mess core/single"
parserobj="parser/parsers parser/match parser/roebling parser/scursor parser/range"
serveobj="serve/req serve/serve serve/proto serve/proto/headers/parserdef \
    serve/proto/headers/utils"
httpobj="serve/proto/http/protodef serve/proto/http/proto serve/proto/http/parserdef \
    serve/proto/http/pat_matches serve/proto/http/debug"
xmlobj="formats/xml/xml formats/xml/parserdef"
testobj="tests/core_tests tests/string_tests tests/serve_tests \
	tests/span_tests tests/hash_tests tests/roebling_tests tests/proto_http_tests \
    tests/table_tests tests/xml_tests tests/match_tests"

allobj="$coreobj $testobj $parserobj $httpobj $serveobj $xmlobj"

echo "cleaning"
clean="rm -Rf build/*"

echo "make dirs"
mkdir -p build
mkdir -p build/core
mkdir -p build/parser
mkdir -p build/serve/proto/http
mkdir -p build/serve/proto/headers
mkdir -p build/tests
mkdir -p build/proto/http
mkdir -p build/formats/xml

for obj in $allobj; do \
	echo "building ${obj} - $CC -c $CFLAGS $INC ${obj}.c -o build/${obj}.o"
	$CC -c $CFLAGS $INC "${obj}.c" -o "build/${obj}.o";
	if [ $? != 0 ]; then
		exit
	fi
done

echo "compiling binary"
deps=""
for obj in $allobj; do \
	deps="$deps build/$obj"
	echo "${deps}"
	$CC $CFLAGS $INC $LIBS -o build/caneka main.c $deps 
done

