#!/bin/bash

# File to modify
config_file="optimize.ini"

N_min=71
sed -i '$s/N_min:.*/N_min: '"$N_min"'/' "$config_file"
# Loop over values for N_max from 100 to 220
for N_max in $(seq 80 220); do
    # Modify the last line of the file
    sed -i '$s/N_max:.*/N_max: '"$N_max"'/' "$config_file"
    
    # Run the optimization program
    ./runOptimization
done
