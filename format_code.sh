#!/bin/sh
 

#  This script searches all source code files and formats them according to the .clang-format style. 

maj_min=3
maj_max=5

base=clang-format
format=""

# Redirect output to stderr.
exec 1>&2

 # check if clang-format is installed
type "$base" >/dev/null 2>&1 && format="$base"

# if not, check all possible versions
# (i.e. clang-format-<$maj_min-$maj_max>-<0-9>)
if [ -z "$format" ]
then
    for j in `seq $maj_max -1 $maj_min`
    do
        for i in `seq 0 9`
        do
            type "$base-$j.$i" >/dev/null 2>&1 && format="$base-$j.$i" && break
        done
        [ -z "$format" ] || break
    done
fi

asdf="$(which $format)"
echo "$asdf"

# no versions of clang-format are installed
if [ -z "$format" ]
then
    echo "$base is not installed. Cannot format code..."
    exit 1
fi

echo "$format was found, going to format your code..." >&2

find . \
    -not \( -path "*/build/*" -prune \) \
    -not \( -path "*/_build/*" -prune \) \
    -not \( -path "*/cmake/*" -prune \) \
    -not \( -path "*/config/*" -prune \) \
    -not \( -path "*/scripts/*" -prune \) \
    -not \( -path "*/.vscode/*" -prune \) \
    -not \( -path "*/third_party/*" -prune \) \
    -not \( -path "*/doc/*" -prune \) \
    \( -name "*.h" -o -name "*.hpp" -o -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.proto" \) \
| xargs $format -i


echo "done..."
