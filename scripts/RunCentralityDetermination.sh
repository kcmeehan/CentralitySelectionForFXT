#!/bin/bash

#This script is used to find the centrality binning of data
#It needs a Glauber Simulation to have been completed first.

#USER DEFINED VARIABLES
dataFileName=../userfiles/AuAl_3_0/CentralityVariableDistributions.root #File containing histogram to be matched
dataHistoName=nPionHisto                                                #Name of the histogram in the file from above
glauberFileName=../userfiles/Glauber_197_27_30mb_WoodsSaxon.root        #Glauber file generated from runGlauberSimulation.sh
outputFileName=../userfiles/AuAl_3_0/CentralityBins.root                #outputfile (will get re-written if it already exists!)
normStartBinCenter=12                                                   #The bin center value to begin the chi^2 matching/optimization routine
normStopBinCenter=-1                                                    #The bin center value to end the chi^2 matching/optimization routine Use -1 to use the last bin of the data histogram. NOTE: This value must be larger than normStartBinCenter to make any sense.

root -l -b -q ../macros/RunCentralityDetermination.C\(\"$dataFileName\",\"$dataHistoName\",\"$glauberFileName\",\"$outputFileName\",$normStartBinCenter,$normStopBinCenter\)
