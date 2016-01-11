#!/bin/bash

#This runs the calls the RunSkimmerAndBinner.C macro which loads the necessary libraries
#and then runs skimmerAndBinner.cxx. The code is run for each root file in the data
# directory.

###########################################################
#SET THE DATA DIRECTORY HERE
#dataDirectory=/scratch_rigel/FixedTargetData/AuAl_DavisDSTs/AuAl_3_0_2010/
dataDirectory=/scratch_ssdMenkar/cflores/testData/

#SET THE OUTPUT DIRECTORY HERE
outputDirectory=../userfiles/AuAl_3_0/

#SET THE NUMBER OF EVENTS HERE (USE -1 FOR ALL)
nEvents=-1 #1000

#SET THE STAR LIBRARY VERSION
starlib=SL10h

########################################################### 

#Array containing all of the dataFiles
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

    root -l -q -b ../macros/RunSkimmerAndBinner.C\(\"$i\",\"$starlib\",$nEvents,\"$outFile\"\) > /dev/null 2>&1 &

    processID+=($!)
    echo ${processID[@]}
done
wait ${processID[@]}

hadd $outputDirectory/YieldHistograms.root ${outFiles[@]}

wait

rm ${outFiles[@]}


exit
