DIR=./
mkdir -p $DIR
gcc server.c -o "$DIR/server.exe"
gcc generator.c -o "$DIR/generator.exe"