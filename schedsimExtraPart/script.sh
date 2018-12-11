#!/bin/bash

echo "Enter the file name:"
read archivo

while [ ! -f $archivo ];
do 
	echo "File does not exists! Enter file name again:"
	read archivo
done


echo "Enter number of CPUs:"
read maxCPUs

while (( $maxCPUs > 8 ))
do
	echo "Too many CPUs! Please enter a number lower than 8 and higher than 0:"
	read maxCPUs
done

# Gets all the algorithm names and erases the first line

./schedsim -L > schedAlgorithms.txt
sed -i -e "1d" schedAlgorithms.txt

if [ -d "results" ]; then
	rm -rf results
fi

mkdir results

# Read the file until no more input

while read nameSched;
do
	echo "Current Algorithm: " $nameSched
	#If SJF remember to add -p
	for ((cpus=1;cpus<=$maxCPUs;cpus++))
	do
		# Add -p for preemptive
		if [ "$nameSched" == "SJF" ]; then
			./schedsim -n $cpus -i $archivo -s $nameSched -p
		elif [ "$nameSched" == "PRIO" ]; then
			./schedsim -n $cpus -i $archivo -s $nameSched -p
		else	
			./schedsim -n $cpus -i $archivo -s $nameSched
		fi

		cd results

		if [ ! -d "$cpus" ]; then
			mkdir $cpus
		fi

		cd ..

		for ((i=0;i<$cpus;i++))
		do	
			mv CPU_$i.log results
			cd ../gantt-gplot 
			./generate_gantt_chart ../schedsim/results/CPU_$i.log ${nameSched}
			cd ../schedsim
			mv results/CPU_${i}_${nameSched}.eps results/$cpus
		done
	done
done < schedAlgorithms.txt

echo "Tasks completed!"
