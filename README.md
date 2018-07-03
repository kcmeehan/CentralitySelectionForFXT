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
	a. To run the glauber model go to the scripts directory and set the number of events to run over (500,000 seems to generate good statistics), set the number of nucleons in the beam and the target, set the cross-section in mb (we use 30 mb for a center-of-mass energy of 4.5 GeV) and choose between a uniform-hard sphere or a Woods=Saxon distribution for the nucleons within the nuclei. For my dissertation the Woods-Saxon distribution is used. Set the output directory and then run the script with the command: ./RunGlauberSimulation.sh
	b. To generate the histogram of the centrality variable distribution navigate to the scripts directory and set the path to where the fxt davisdsts are, and the path to the output directory. By default the script should run over all events. To run the script execute: ./RunCentralityVariableDistributions.sh

3. Fit the glauber model to the data distribution to determine the centrality bins. 





