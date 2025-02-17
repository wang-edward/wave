#!/bin/bash
# Loop through all arguments (can be files or directories)
for path in "$@"; do
    # Check if the argument is a directory
    if [[ -d "$path" ]]; then
        # Find all .cc and .hh files recursively in the directory
        find "$path" -type f \( -name "*.c" -o -name "*.h" \) |
        while IFS= read -r file; do
            # Print the filename
            echo "$(basename "$file")"
            echo '```c'
            # Print the contents of the file
            cat "$file"
            echo '```'
        done
    elif [[ -f "$path" ]]; then
        # If it's a file, process it directly
        echo "$(basename "$path")"
        echo '```c'
        cat "$path"
        echo '```'
    else
        echo "Warning: $path is not a valid file or directory." >&2
    fi
done
