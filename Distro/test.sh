#!/bin/bash

if [ $# -eq 0 ]
    then
    numCats=3
    numCheeses=5
    mode=1
    depth=10
    smartness=0.9
    startSeed=1000
    endSeed=1020
elif [ $# -ne 7 ];
    then echo "Usage: test.sh num_cats num_cheeses mode depth smartness start_seed end_seed"
    exit 1
else
    numCats=$1
    numCheeses=$2
    mode=$3
    depth=$4
    smartness=$5
    startSeed=$6
    endSeed=$7
fi

./compile.sh

wins=0
losses=0

for i in `seq $startSeed $endSeed`
do
    ./MiniMax_search $i $numCats $numCheeses $mode $depth $smartness 2> run.log
    isLose=`grep "Oh no! poor poor mouse!" run.log | wc -l`
    isWin=`grep "Happy, well-fed mouse!" run.log | wc -l`
    if [ $isLose -eq 1 ]; then
        echo "Lost seed $i"
    elif [ $isWin -eq 1 ]; then
        echo "Won seed $i"
    fi
    wins=`expr $wins + $isWin`
    losses=`expr $losses + $isLose`
done

echo "Losses: $losses"
echo "Wins: $wins"
