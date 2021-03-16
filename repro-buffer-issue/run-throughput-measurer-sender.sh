echo "----------------- Running with packet size 32"
timeout 30 npx ts-node throughputmeasurer.ts 32 true
sleep 10
echo "----------------- Running with packet size 64"
timeout 30 npx ts-node throughputmeasurer.ts 64 true
sleep 10
echo "----------------- Running with packet size 128"
timeout 30 npx ts-node throughputmeasurer.ts 128 true
sleep 10
echo "----------------- Running with packet size 256"
timeout 30 npx ts-node throughputmeasurer.ts 256 true
sleep 10
echo "----------------- Running with packet size 512"
timeout 30 npx ts-node throughputmeasurer.ts 512 true
sleep 10
echo "----------------- Running with packet size 1024"
timeout 30 npx ts-node throughputmeasurer.ts 1024 true
sleep 10
echo "----------------- Running with packet size 2048"
timeout 30 npx ts-node throughputmeasurer.ts 2048 true
sleep 10
echo "----------------- Running with packet size 4096"
timeout 30 npx ts-node throughputmeasurer.ts 4096 true
sleep 10
echo "----------------- Running with packet size 8192"
timeout 30 npx ts-node throughputmeasurer.ts 8192 true
sleep 10
echo "----------------- Running with packet size 16384"
timeout 30 npx ts-node throughputmeasurer.ts 16384 true
sleep 10
echo "----------------- Running with packet size 32768"
timeout 30 npx ts-node throughputmeasurer.ts 32768 true
sleep 10
echo "----------------- Running with packet size 65536"
timeout 30 npx ts-node throughputmeasurer.ts 65536 true
sleep 10
echo "----------------- Running with packet size 131072"
timeout 30 npx ts-node throughputmeasurer.ts 131072 true
sleep 10
echo "----------------- Running with packet size 262144"
timeout 30 npx ts-node throughputmeasurer.ts 262144 true
sleep 10
echo "----------------- Running with packet size 524288"
timeout 30 npx ts-node throughputmeasurer.ts 524288 true
sleep 10
echo "----------------- Running with packet size 1048576"
timeout 30 npx ts-node throughputmeasurer.ts 1048576 true

echo "----------------- Sending completed"
