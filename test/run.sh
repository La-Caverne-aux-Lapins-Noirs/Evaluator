#!/bin/sh

success_test=0
total_test=0

flags="-O0 -Og -g -g3 -ggdb -W -Wall -std=c11 -fprofile-arcs -ftest-coverage --coverage -rdynamic"
libs="-L${HOME}/.froot/lib/ -I${HOME}/.froot/include/ -L../ -I../include/ --whole-file -levaluator -llapin -lcrawler -lsfml-graphics -lsfml-audio -lsfml-window -lsfml-system -lavcall -lopencv_imgproc -lopencv_highgui -lopencv_objdetect -lopencv_video -lopencv_core -ldl -lm -lusb -lgcov -lstdc++"

run_test()
{
    output=`echo "$1" | rev | cut -d '.' -f 2 | rev`
    gcc $1 $libs $flags -o ./$output && \
	LD_LIBRARY_PATH=../ ./$output && \
	success_test=$((success_test + 1)) && \
	(echo "\033[0;32mTest $output OK\033[00m" ; echo "$output ok" >> trace) || \
	    (echo "\033[0;31mTest $output KO\033[00m" ; echo "$output failure" >> trace)
    total_test=$((total_test + 1))
}

rm -f trace

if [ "$#" -eq "0" ]; then
    for param in `find . -name '*.c'`; do
	run_test $param
    done
    lcov --directory . --directory ../src/ -c -o coverage_raw.info #> /dev/null 2> /dev/null && \
    lcov --remove coverage_raw.info '/usr/include/*' '7/*' -o coverage.info #> /dev/null 2> /dev/null && \
    genhtml -o ./report/ -t "TechnoCore: Coverage" coverage.info | grep "\%"
    rm -f coverage_raw.info
else
    for param in "$@"; do
	run_test $param
    done
fi

echo "Tests passed:" $success_test "/" $total_test
if [ "$success_test" -ne "$total_test" ]; then
    exit 1
fi
exit 0
