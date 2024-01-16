#!/bin/bash

# Define the URL to curl
URL1="http://127.0.0.1:1234/home.html"
URL2="http://127.0.0.1:1234/homeTEST.html"

# Number of parallel requests
NUM_REQUESTS=2

# Output file for all responses
OUTPUT_FILE="output.txt"

# Remove the output file if it already exists
rm -f "$OUTPUT_FILE"

# Loop to run parallel curl requests 20 times
for ((i = 1; i <= NUM_REQUESTS; i++)); do
  {
    #curl -sS "$URL1" &
    curl -sS "$URL2" &
  } #>> "$OUTPUT_FILE" &
done

# Wait for all background processes to finish
wait
