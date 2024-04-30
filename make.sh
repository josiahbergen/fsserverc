
# Temporary solution until I figure out how to use makefiles :(

clear

echo "\033[0;90mSetting up...\033[0;37m"
rm -rf bin
mkdir -p bin

echo "\033[0;90mCompiling...\033[0;37m"

gcc -o bin/server src/main.c src/server.c -Wall -Wextra -g;
status=$?

echo "\033[0;90mCompilation complete with status $status.\033[0;37m"

if [ $status -eq 0 ]; then
    echo "\033[0;90mRunning...\033[0;37m"
    echo;
    ./bin/server 64198
else
    echo "\033[1;31mCompilation failed.\033[0;37m See above for details."
fi
