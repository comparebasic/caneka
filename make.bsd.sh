CC=cc
CFLAGS="-g -Werror -Wno-incompatible-pointer-types-discards-qualifiers"
INC="-I./include"

msg() {
    echo -e "    ${@}"
}

coreobj="core/caneka core/mem core/string core/error \
	core/log core/slab core/span core/lookup core/chain core/testsuite \
    core/hash core/table core/compare core/iter core/array core/int \
    core/maker core/mess core/single core/debug"
parserobj="parser/parsers parser/match parser/roebling parser/scursor parser/range"
httpobj="serve/proto/http/protodef serve/proto/http/proto serve/proto/http/parserdef \
    serve/proto/http/pat_matches serve/proto/http/debug"

xmlobj="formats/xml/xml formats/xml/parserdef"

testobj="tests/core_tests tests/string_tests tests/table_tests tests/xml_tests \
    tests/match_tests tests/span_tests tests/hash_tests tests/roebling_tests"

allobj="$testobj $parserobj $httpobj $xmlobj $coreobj"

#serveobj="serve/req serve/serve serve/proto serve/proto/headers/parserdef \
#    serve/proto/headers/utils"
#testobj="tests/serve_tests  tests/proto_http_tests $testobj"
#allobj="$serveobj $allobj"

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
	msg "building ${obj} - $CC -c $CFLAGS $INC ${obj}.c -o build/${obj}.o"
	$CC -c $CFLAGS $INC "${obj}.c" -o "build/${obj}.o";
	if [ $? != 0 ]; then
		exit
	fi
    msg "adding to lib 'build/${obj}.o'"
    ar r build/libcaneka.a "build/${obj}.o"
done

msg "Compiling binary"
cmd="$CC $CFLAGS $INC $LIBS -o build/caneka main.c build/libcaneka.a"
msg $cmd
$cmd
