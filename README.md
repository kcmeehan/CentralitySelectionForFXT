# CentralitySelectionForFXT
Code used for centrality selection for Au + Au 4.5GeV fxt test run for my thesis. (Doesn't include pileup code).

--------------------------------------------------------------------------------------------------------------
GETTING STARTED:
-------------------------------------------------------------------------------------------------------------

After cloning the repository enter:
	git submodule init
	git submodule update
to initialize the submodules (where the heart of the glauber model+ NB code is).
If you are prompted for a password at any time during this process simply hit enter.

To compile the code navigate to the davisdstanalysis directory and type:
	make
When this procedure is done you will have a new directory, bin/, continaing all of
the compiled binaries.


--------------------------------------------------------------------------------------------------------------
PERFORMING CENTRALITY SELECTION:
-------------------------------------------------------------------------------------------------------------

1. Set the analysis cuts in userfiles/UserCuts.cxx. These should currently be set to the correct analysis cuts.

2. Create the glauber model and the centrality variable distribution. The order does not matter and these can be run simultaneously.

	a. To run the glauber model go to the scripts directory and open RunGlauberSimulation.sh. Set the number of events to run over (500,000 seems to generate good statistics), set the number of nucleons in the beam and the target, set the cross-section in mb (we use 30 mb for a center-of-mass energy of 4.5 GeV) and choose between a uniform-hard sphere or a Woods=Saxon distribution for the nucleons within the nuclei. For my dissertation the Woods-Saxon distribution is used. Set the output directory and then run the script with the command: ./RunGlauberSimulation.sh

	b. To generate the histogram of the centrality variable distribution navigate to the scripts directory and open RunCentralityVariableDistributions.sh. Set the path to where the fxt davisdsts are, and the path to the output directory. By default the script should run over all events. To run the script execute: ./RunCentralityVariableDistributions.sh

3. Fit the glauber model to the data distribution to determine the centrality bins. This step requires both the glauber model and the centrality variable root files produced in the previous step. Navigate to the scripts directory and open RunCentralityDetermination.sh. Set the path to both the variable distribution and the glauber model. Also set "dataHistoName" to the name of the histogram in the centrality variable root file that corresponds to the multiplicity variable that is being used for the centrality selection. In the case of this dissertation this variable should be set to nGoodTracks. Set the output file name and the bin center value for the bins marking the range the glauber model should be fit to. In the case of this dissertation we fit in the multiplicity range of 110 to 230. To run the script execute: ./RunCentralityDetermination.sh 

Note: At the end of this script, information on the "best trial" (including the trial number which will be needed for the drawing macro) will be printed. This information includes the multiplicity cutoffs corresponding to each centrality bin. These values should be added to the UserCuts.cxx file in order for the centrality cuts to be drawn when running the drawing macro.

4. Drawing the centrality bins. The figure in the paper draft at the time of this dissertation was drawn with a macro written by Todd Kinghorn and Daniel Brandenburg and is a more polished version of the macro included in this repository and includes a drawing of the pileup simulation. The macro used to draw a simple figure illustrating the centrality variable distribution and the centrality bins is in the drawingmacros directory and is called DrawCentralityBinResults.C. 




