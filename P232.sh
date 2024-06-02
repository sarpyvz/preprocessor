if [ $# -ne 1 ]; then
    echo "Usage: $0 <input_filename>"
    exit 1
fi

gcc -o preprocessor preprocessor.c
./preprocessor "$1"
