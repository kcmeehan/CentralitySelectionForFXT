#!/bin/bash

# This script runs the code that creates a histogram of the measured multiplicity distribution
# used for centrality selection.

###########################################################
# SET THE DATA DIRECTORY HERE
dataDirectory=/scratch_menkar/FixedTargetData/AuAu_4_5GeV_2015/

# SET THE OUTPUT DIRECTORY HERE 
outputDirectory=../userfiles/FinalTest/

# SET THE NUMBER OF EVENTS HERE (USE -1 FOR ALL) 
nEvents=-1 

########################################################### 


# Array containing all of the dataFiles         
dataFiles=( $dataDirectory/*.root )
processID=()
numberOfFiles=${#dataFiles[@]}
outFiles=()

# Run the code for each data file
for i in ${dataFiles[@]}
do
    echo "Running on dataFile: " $i

    outFile=$(basename $i .root)
    outFile=$outputDirectory/"$outFile"_CentProcessed.root

    outFiles+=($outFile)

    root -l -q -b ../macros/RunCentralityVariableDistributions.C\(\"$i\",$nEvents,\"$outFile\"\) > /dev/null 2>&1 &

    processID+=($!)
    echo ${processID[@]}
done
wait ${processID[@]}

# Combine the output data files into a single file
hadd $outputDirectory/CentralityVariableDistributions.root ${outFiles[@]}

wait

# Cleanup
rm ${outFiles[@]}


exit
