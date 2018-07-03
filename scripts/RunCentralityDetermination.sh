#!/bin/bash

# This script is used to find the centrality binning of data
# It needs a Glauber Simulation to have been completed first.

# USER DEFINED VARIABLES
# File with the centrality variable histogram
dataFileName=../userfiles/FinalTest/CentralityVariableDistributions.root
# Name of the histogram in the file from above
dataHistoName=nGoodTracks                                                
# Name of file with Glauber model
glauberFileName=../userfiles/FinalTest/Glauber_197_197_30mb_WoodsSaxon.root
#outputfile (will get re-written if it already exists!)
outputFileName=../userfiles/FinalTest/CentralityBins_50k.root
#The bin center value to begin the chi^2 matching/optimization routine
normStartBinCenter=110 
#The bin center value to end the chi^2 matching/optimization routine Use -1 to use the last bin of the data histogram. NOTE: This value must be larger than normStartBinCenter to make any sense.
normStopBinCenter=230
#110 to 230


root -l -b -q ../macros/RunCentralityDetermination.C\(\"$dataFileName\",\"$dataHistoName\",\"$glauberFileName\",\"$outputFileName\",$normStartBinCenter,$normStopBinCenter\)
