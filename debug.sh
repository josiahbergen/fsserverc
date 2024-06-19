
# Yeah... I have no idea how to use makefiles :(

output_file="bin/server"
args="-Wall -Wextra -g"

# Add -fsanitize=address if memory errors happen.
# It breaks things when run on Linux.

echo "Setting up..."
rm -rf bin
mkdir -p bin

search_dir="$(pwd)/src"
files=''
num_files=0

for entry in $(ls $search_dir/*.c); do
    num_files=$((num_files+1))
    files+=" $entry"
done

echo "$num_files files to compile."
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
    valgrind --leak-check=yes ./bin/server 64198 2
fi
