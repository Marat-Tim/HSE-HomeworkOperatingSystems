DIR=./out
mkdir -p $DIR
gcc server.c -o "$DIR/server.exe"
gcc generator.c -o "$DIR/generator.exe"
gcc duty.c -o "$DIR/duty.exe"
gcc doctor.c -o "$DIR/doctor.exe"