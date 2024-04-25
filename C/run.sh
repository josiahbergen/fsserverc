
# Little script to compile and run the server

clear

echo "Compiling...";
gcc main.c -o server

echo "Running...";
./server 64198
