
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

tmp_file=$(mktemp)

# Run the compilation command inside a pseudo-terminal
script -q -c "gcc -o $output_file $files $args" "$tmp_file"
status=$?

# Print the content of the temporary file to the terminal (this retains colors)
cat "$tmp_file"

if [ $status -eq 1 ]; then

    # Compilation failed with error(s)
    echo;
    echo "Compilation failed. See above for details."

elif grep -q "warning:" "$tmp_file"; then

    # Compilation completed with warnings
    echo;
    echo "Compilation complete with warnings. Continue? (y/n)"

    read action
    if [ "$action" = "y" ]; then
        ./bin/server 64198
    else
        echo "Exiting..."
    fi

else

    # Compilation completed successfully
    ./bin/server 64198

fi

rm "$tmp_file"

