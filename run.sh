# Temporary solution until I figure out how to use makefiles :(

clear

echo "Setting up..."
mkdir -p bin

echo "Compiling..."
gcc -o bin/server src/main.c src/server.c

if gcc -o bin/server src/main.c src/server.c -Wall -Wextra -g; then
    echo -e "Running...\n"
    ./bin/server 64198
fi
