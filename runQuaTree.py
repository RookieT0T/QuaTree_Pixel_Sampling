import os
import subprocess
import time
import glob

# Function to remove *.txt files
def remove_txt_files():
    txt_files = glob.glob("*.txt")
    for txt_file in txt_files:
        os.remove(txt_file)
        print(f"Removed {txt_file}")

# Function to compile the C++ program
def compile_cpp():
    compile_command = "g++ quaTree.cpp -o quaTree.exe"
    result = subprocess.run(compile_command, shell=True)
    if result.returncode != 0:
        print("Compilation failed!")
        exit(1)
    print("Compilation successful!")

# Path to the executable
exe_path = "./quaTree.exe"

# Remove *.txt files
remove_txt_files()

# Compile the C++ program
compile_cpp()

# Record the start time
start_time = time.time()

# Run the executable
process = subprocess.run(exe_path, shell=True)

# Record the end time
end_time = time.time()

# Calculate the elapsed time
elapsed_time = end_time - start_time

# Convert elapsed time to hours, minutes, seconds, milliseconds
hours, rem = divmod(elapsed_time, 3600)
minutes, rem = divmod(rem, 60)
seconds, milliseconds = divmod(rem, 1)
milliseconds *= 1000

# Print the elapsed time
print(f"Elapsed Time: {int(hours)} hours {int(minutes)} minutes {int(seconds)} seconds {int(milliseconds)} milliseconds")
