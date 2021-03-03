
NUM=10
WSURL='ws://95.216.64.56:8080/'
NODE_ID_PREFIX='client'

if [ -z "$1" ]
  then
    echo "No first argument given, using ${NODE_ID_PREFIX} as default node id prefix"
  else
    NODE_ID_PREFIX=$1
fi

if [ -z "$2" ]
  then
    echo "No second argument given, running default of ${NUM} clients"
  else
    NUM=$2
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
   screen -S tsclient${c} -dm bash -c "WS_URL=${WSURL} npx ts-node client.ts ${NODE_ID_PREFIX}${c} &> logs/${NODE_ID_PREFIX}${c}.txt ; exec sh"
done

screen -ls
