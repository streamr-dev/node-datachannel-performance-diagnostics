
NUM=10

if [ -z "$1" ]
  then
    echo "No argument given, running default of ${NUM} clients"
  else
    NUM=$1
fi

for (( c=1; c<=$NUM; c++ ))
do  
   screen -S tsclient${c} -dm bash -c 'npx ts-node client.ts; exec sh'
done

screen -ls
