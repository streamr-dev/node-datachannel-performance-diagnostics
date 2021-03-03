
NUM=10
WSURL='ws://localhost:8080/'

if [ -z "$1" ]
  then
    echo "No first argument given, running default of ${NUM} clients"
  else
    NUM=$1
fi

if [ -z "$WS_URL" ]
  then
    echo "Environment variable WS_URL not set, starting with default WS_URL of ${WSURL}"
  else
    WSURL=$WS_URL
fi


mkdir -p logs
for (( c=1; c<=$NUM; c++ ))
do  
   screen -S tsclient${c} -dm bash -c 'WS_URL=${WSURL} npx ts-node client.ts > logs/tsclient${c}.txt; exec sh'
done

screen -ls
