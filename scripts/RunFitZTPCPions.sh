#!/bin/bash

#This calss the RunFitZTPCPions.C file in the macros directory which loads the necessary
#libraries and then runs fitZTPCPions.cxx in the src/analysis directory.

######################################################
#SET THE YIELD HISTOGRAM FILE NAME AND PATH (FULL PATH)
yieldHistoFile=/scratch_ssdMenkar/cflores/davisdstanalysis/userfiles/AuAl_3_0/YieldHistograms.root

#SET THE SPECTRA FILE NAME AND PATH (FULL PATH)
spectraFile=/scratch_ssdMenkar/cflores/davisdstanalysis/userfiles/AuAl_3_0/Spectra.root

#SET THE OUTPUT IMAGE DIRECTORY (LEAVE BLANK TO SAVE NO IMAGES)
imageDir=

#SET THE RAPIDITY VALUE (USE -999 FOR ALL)
rapidityValue=1.25

#SET THE CENTRALITY BIN INDEX (USE -1 FOR ALL)
centralityIndex=0

#SET THE STARLIBRARY VERSION
starlib=SL10h
######################################################

root -l ../macros/RunFitZTPCPions.C\(\"$yieldHistoFile\",\"$spectraFile\",\"$starlib\",$centralityIndex,$rapidityValue,\"$imageDir\"\) #> /dev/null 2>&1 &

exit
