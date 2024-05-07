
# Yeah... I have no idea how to use makefiles :(

clear

echo "Setting up..."
echo;
rm -rf bin
mkdir -p bin

echo "Files to compile:"

search_dir="$(pwd)/src"
files=''

for entry in $(ls $search_dir/*.c); do
    echo $entry
    files+=" $entry"
done

echo;
echo "Compiling..."

gcc -o bin/server $files -Wall -Wextra -g -fsanitize=address;
status=$?

echo "Compilation complete with status $status."

if [ $status -eq 0 ]; then
    echo "Running..."
    echo;
    ./bin/server 64198
else
    echo;
    echo "Compilation failed. See above for details."
fi
