ps -ef | grep c\ 127\\.0\\.0\\.1 | awk '{print $2}' | while read -r line; do kill "$line"; done
