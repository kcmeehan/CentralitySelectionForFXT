//This macro runs the compiled centralityVariableDistributions binary.
//The source can be found at:
//    src/analysis/centralityVariableDistributions.cxx

void RunCentralityVariableDistributions(TString inputFile, Long64_t nEvents=-1, TString outputFile=""){

  //Load the Necessary Libraries
  gSystem->Load("../bin/TrackInfo_cxx.so");
  gSystem->Load("../bin/PrimaryVertexInfo_cxx.so");
  gSystem->Load("../bin/EventInfo_cxx.so");

  gSystem->Load("../bin/UserCuts_cxx.so");
  gSystem->Load("../bin/centralityVariableDistributions_cxx.so");

  centralityVariableDistributions(inputFile,nEvents,outputFile);

}
