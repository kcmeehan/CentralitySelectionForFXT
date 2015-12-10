//This macro runs the compilded skimmerAndBinner binary.
//The source code for skimmerAndBinner.cxx can be found at
//    src/analysis/skimmerAndBinner.cxx

void RunSkimmerAndBinner(TString inputFile, TString starLibrary, Long64_t nEvents=-1, TString outputFile=""){

  //Load the Necessary Libraries
  gSystem->Load("../bin/TrackInfo_cxx.so");
  gSystem->Load("../bin/PrimaryVertexInfo_cxx.so");
  gSystem->Load("../bin/EventInfo_cxx.so");
  gSystem->Load("../bin/ParticleInfo_cxx.so");

  gSystem->Load("../bin/UserCuts_cxx.so");
  gSystem->Load("../bin/skimmerAndBinner_cxx.so");

  skimmerAndBinner(inputFile,starLibrary,nEvents,outputFile);

}
