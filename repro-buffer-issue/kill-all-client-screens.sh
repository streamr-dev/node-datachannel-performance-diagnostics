
echo "Before killing"
screen -ls
screen -ls | grep tsclient | cut -d. -f1 | awk '{print $1}' | xargs kill
echo "After killing"
screen -ls
