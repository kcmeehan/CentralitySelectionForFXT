#include <iostream>
#include <vector>

using namespace std;

void RunCentralityDetermination(TString DATAFILE, TString DATAHISTO,
				TString GLAUBERFILE,TString OUTFILE,
				Double_t STARTBIN=20, Double_t STOPBIN=-1){

  //Load the Necessary Libraries
  gSystem->Load("../bin/GlauberClass_cxx");
  gSystem->Load("../bin/GlauberModel_cxx");
  gSystem->Load("../bin/FindBestFitNegativeBinomialParameters_cxx");
  gSystem->Load("../bin/FindCentralityBinCuts_cxx");
  gSystem->Load("../bin/DetermineCentralityBin_cxx");
  gSystem->Load("../bin/ReturnStructs_cxx");
  gSystem->Load("../bin/FindNpartNcollDistributions_cxx");
  gSystem->Load("../bin/PrintResults_cxx");

  //Load the Data Histogram that we are trying to match
  TFile *dataFile = new TFile(DATAFILE);
  TH1D *dataHisto = (TH1D *)dataFile->Get(DATAHISTO);

  //Load the File from the Glauber Simulation
  TFile *glauberFile = new TFile(GLAUBERFILE);
  TTree *glauberTree = (TTree *)glauberFile->Get("GlauberTree");

  //Create an output file to save things to
  TFile *outFile = new TFile(OUTFILE,"RECREATE");
  
  //Variables Used in the Loops Below
  const Int_t nTrials(5);          //Number of times to repeat this process
  Int_t bestTrial(0);              //This is the Index of the trial that had the highest Inverse Chi2
  Double_t startBinCenter(STARTBIN);  //The value of the bin where we want to start matching
  Double_t stopBinCenter(STOPBIN);    //The value of the bin where we want to stop matching
  Int_t nItersPerTrial(100);       //The number of choices of (n,p) in each trial
  NegBinomialSearchResults bestBinomialParameters[nTrials];
  
  // First, we need to find the parameters of the negative binomial distribution
  // which results in a multiplicity distribution that best matches the data. We
  // repeat this process several times to get a feel for the associated error.
  for (Int_t iTrial=0; iTrial< nTrials; iTrial++){

    //Find the Best Fit NB Parameters
    bestBinomialParameters[iTrial] =
      FindBestFitNegativeBinomialParameters(dataHisto,glauberTree,outFile,
					    startBinCenter,stopBinCenter,
					    nItersPerTrial,iTrial);
    
    //Keep track of the trial with the best inverse chi2
    if (bestBinomialParameters[iTrial].InverseChi2 >=
	bestBinomialParameters[bestTrial].InverseChi2)
      bestTrial = iTrial;

  }//End Loop Over Trials

  cout <<"-----Step 1 of 3 is Complete-----" <<endl;
  cout <<"The Trial with the largest 1/Chi2 is: " <<bestTrial
       <<" With 1/Chi2: " <<bestBinomialParameters[bestTrial].InverseChi2 
       <<" With n: " <<bestBinomialParameters[bestTrial].n
       <<" With p: " <<bestBinomialParameters[bestTrial].p <<endl;
                                          
  //Now we loop over the trials again to accomplish two things:
  //1. Find the centrality bin cuts for each trial since the exact values of
  //    the cuts might change slightly with different NB parameter choices
  //2. Find the Npart, Ncoll, and impact parameter distributions for each
  //    trial. These will be used in the error determination
  //they will differ depending on their negative binomial
  for (Int_t iTrial=0; iTrial< nTrials; iTrial++){

    FindCentralityBinCuts(&bestBinomialParameters[iTrial]);
 
    FindNpartNcollDistributions(&bestBinomialParameters[iTrial],glauberTree);

  }//End Loop Over Trials
  
  //The Trial with the Highest value of one over chi2 will be the trial
  //which determines the central values of all the distributions. The other trials
  //will be used to compute the systematic Error
  Double_t nPartSysError(0), nCollSysError(0), impactParamSysError(0);
  for (Int_t iCentBin=0; iCentBin<bestBinomialParameters[bestTrial].nCentralityBins; iCentBin++){

    std::vector<double> nPartMeans;
    std::vector<double> nCollMeans;
    std::vector<double> impactParamMeans;
    
    for (Int_t iTrial=0; iTrial<nTrials; iTrial++){
      
      nPartMeans.push_back(bestBinomialParameters[iTrial].nPartMeans[iCentBin]);
      nCollMeans.push_back(bestBinomialParameters[iTrial].nCollMeans[iCentBin]);
      impactParamMeans.push_back(bestBinomialParameters[iTrial].impactParamMeans[iCentBin]);
	
    }//End Loop Over Trials

    nPartSysError = TMath::RMS((Int_t)nPartMeans.size(),&nPartMeans[0]);
    nCollSysError = TMath::RMS((Int_t)nCollMeans.size(),&nCollMeans[0]);
    impactParamSysError = TMath::RMS((Int_t)impactParamMeans.size(),&impactParamMeans[0]);

    //Print the Results
    PrintResults(&bestBinomialParameters[bestTrial],iCentBin,
		 nPartSysError,nCollSysError,impactParamSysError);
    
  }//End Loop Over Centrality Bins

  //Save the Data Histo to the output file                                                                    
  outFile->cd();
  dataHisto->Write();
  
}
