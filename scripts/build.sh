g++ --version

#! https://stackoverflow.com/a/246128
SCRIPT_DIR=$(cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd)
ROOT_DIR=$(realpath $SCRIPT_DIR/..)

echo "Got root of directory: $ROOT_DIR"
mkdir -p $ROOT_DIR/build

echo "Building main.exe"
params="-Wall -I src/include -D LOGGING -std=c++20"
if [ "$1" == "debug" ]
then
    params="$params -g -D DEBUG"
    echo "Building in debug mode"

else
    params="$params -O3"
    if [ "$1" == "profile-generate" ]
    then
        params="$params -fprofile-generate"
        echo "Building in profile mode"

    elif [ "$1" == "profile-use" ]
    then
        params="$params -fprofile-use"
        echo "Building using generated profile data"

    else
        echo "Building normally"

    fi

fi

command="g++ $params $ROOT_DIR/src/main.cpp -o $ROOT_DIR/build/main.exe"
echo "Running \"$command\""
$command
