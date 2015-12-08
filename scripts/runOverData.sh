#!/bin/bash

#This runs the RunReader.C macro which runs the reader.cxx function over 
#the data specified in the data directory. Each file is run in parallel.
#The script then waits until all the files have been processed before
#hadding the files together

###########################################################
#SET THE DATA DIRECTORY HERE
dataDirectory=/scratch_rigel/FixedTargetData/AuAl_DavisDSTs/AuAl_3_0_2010/

#SET THE OUTPUT DIRECTORY HERE
outputDirectory=../

#SET THE NUMBER OF EVENTS HERE (USE -1 FOR ALL)
nEvents=1000

###########################################################

#Array containing all the datafiles
dataFiles=( $dataDirectory/*.root )
processID=()
numberOfFiles=${#dataFiles[@]}
outFiles=()

for i in ${dataFiles[@]}
do
    echo "Running on dataFile: " $i
    
    outFile=$(basename $i .root)
    outFile=$outputDirectory/"$outFile"_Processed.root

    outFiles+=($outFile)

    root -l -q -b '../macros/RunReaderExample.C ('\"$i\"','$nEvents','\"$outFile\"')' > /dev/null 2>&1 &
    
    processID+=($!)
    echo ${processID[@]}
done 
wait ${processID[@]}

hadd $outputDirectory/Combined.root ${outFiles[@]}

wait

rm ${outFiles[@]}


exit
