
# Yeah... I have no idea how to use makefiles :(

output_file="bin/server"
args="-Wall -Wextra -g"

# Add -fsanitize=address if memory errors happen.
# It breaks things when run on Linux.

clear

echo "Setting up..."
echo;
rm -rf bin
rm -rf temp
mkdir -p bin
mkdir -p temp

echo "Files to compile:"

search_dir="$(pwd)/src"
files=''

for entry in $(ls $search_dir/*.c); do
    echo $entry
    files+=" $entry"
done

echo;
echo "Compiling..."

# Run the compilation command inside a pseudo-terminal
gcc $args -o $output_file $files
status=$?

if [ $status -eq 1 ]; then

    # Compilation failed with error(s)
    echo;
    echo "Compilation failed. See above for details."

else
    # Compilation completed successfully
    ./bin/server 64198
fi
