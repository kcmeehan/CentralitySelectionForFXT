//This macro runs the compiled reader code

void RunReader(TString inputFile, Int_t nEvents=-1, TString outputFile=""){

  //Load the necessary Librarires
  gSystem->Load("../bin/TrackInfo_cxx.so");
  gSystem->Load("../bin/PrimaryVertexInfo_cxx.so");
  gSystem->Load("../bin/EventInfo_cxx.so");
  gSystem->Load("../bin/reader_cxx.so");

  reader(inputFile,nEvents,outputFile);

  
}
