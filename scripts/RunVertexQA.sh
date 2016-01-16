#!/bin/bash

#This runs the RunVertexQA.C macros which loads the necessary libraries
#and then runs the vertexQAmaker.cxx binary. The code is run for each root file in the data
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
dataFiles=( $dataDirectory/*.root )
#dataFiles=AuAu_4_5GeV_2015_0.root
processID=()
numberOfFiles=${#dataFiles[@]}
#numberOfFiles=1
outFiles=()
#i=$dataDirectory
#i+=$dataFiles
boolVert=false

for i in ${dataFiles[@]}
do
    echo "Running on dataFile: " $i
    outFile=$(basename $i .root)

    outFile=$outputDirectory"$outFile"_Processed.root
 
    outFiles+=($outFile)

    root -l -q -b ../macros/RunEventQA.C\(\"$i\",\"$outFile\",$boolVert,$nEvents\) 

    processID+=($!)
    echo ${processID[@]}
done

wait ${processID[@]}

suffix=vertexQA
if $boolVert; then
  suffix=$suffix"_vertexCuts" 
else
  suffix=$suffix"_noCuts"
fi

hadd $outputDirectory/$suffix.root ${outFiles[@]}

wait

rm ${outFiles[@]}

exit
