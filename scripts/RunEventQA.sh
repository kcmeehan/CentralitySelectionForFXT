#!/bin/bash

#This runs the QA macros which loads the necessary libraries
#and then runs the QA binaries. The code is run for each root file in the data
# directory.

###########################################################
#SET THE DATA DIRECTORY HERE
#dataDirectory=/scratch_rigel/FixedTargetData/AuAl_DavisDSTs/AuAl_3_0_2010/
dataDirectory=/scratch_rigel/AuAu_4_5_2015/

#SET THE OUTPUT DIRECTORY HERE
outputDirectory=../userfiles/qa/

#SET THE NUMBER OF EVENTS HERE (USE -1 FOR ALL)
nEvents=10 #1000

########################################################### 

#Array containing all of the dataFiles
#dataFiles=( $dataDirectory/*.root )
dataFiles=AuAu_4_5GeV_2015_0.root
processID=()
#numberOfFiles=${#dataFiles[@]}
numberOfFiles=1
#outFiles=()
i=$dataDirectory
i+=$dataFiles
bool=false

#for i in ${dataFiles[@]}
#do
    echo "Running on dataFile: " $i
    outFile=$(basename $i .root)

    if $bool; then
      outFile=$outputDirectory"$outFile"_eventQA_eventCuts.root
		else
		  outFile=$outputDirectory"$outFile"_eventQA_noCuts.root
		fi
 
    #outFiles+=($outFile)

    root -l -q -b ../macros/RunEventQA.C\(\"$i\",\"$outFile\",$bool,$nEvents\) 

    processID+=($!)
    echo ${processID[@]}
wait ${processID[@]}

wait

exit
