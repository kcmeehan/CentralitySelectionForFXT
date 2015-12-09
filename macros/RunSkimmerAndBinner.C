//This macro runs the compilded skimmerAndBinner binary.
//The source code for skimmerAndBinner.cxx can be found at
//    src/analysis/skimmerAndBinner.cxx

void RunSkimmerAndBinner(TString inputFile, TString centralityFile, TString outputFile=""){

  //Load the Necessary Libraries
  gSystem->Load("../bin/TrackInfo_cxx.so");
  gSystem->Load("../bin/PrimaryVertexInfo_cxx.so");
  gSystem->Load("../bin/EventInfo_cxx.so");
  gSystem->Load("../bin/skimmerAndBinner_cxx.so");



  skimmerAndBinner(inputFile,centralityFile,outputFile);

}
