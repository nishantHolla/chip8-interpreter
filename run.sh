#!/bin/bash

# Check if executable exists
if [ ! -e ./out/c8i ]; then
  echo -e "Could not find executable ./out/c8i.\nPlease run \"make release\" before running this script."
  exit 1
fi

# Declare arrays to store file paths and labels
declare -a files
declare -A file_map

# Counter for numbering
count=1

# Find all files and group them by their directory
while IFS= read -r filepath; do
    dir=$(dirname "$filepath")
    file=$(basename "$filepath")

    # If directory heading hasn't been printed yet, print it
    if [[ -z "${file_map[$dir]}" ]]; then
        echo
        echo "In directory: $dir"
        file_map[$dir]=1
    fi

    echo "$count) $file"
    files+=("$filepath")
    ((count++))
done < <(find roms -type f -name "*.ch8" | sort)

# Prompt user to choose
echo
read -p "Enter the number of the ROM file you want to run: " choice

# Validate and use the selection
if [[ "$choice" =~ ^[0-9]+$ ]] && (( choice >= 1 && choice <= ${#files[@]} )); then
    selected_file="${files[choice-1]}"
    # Run the selected ROM
    ./out/c8i $selected_file
else
    echo "Invalid selection."
fi

