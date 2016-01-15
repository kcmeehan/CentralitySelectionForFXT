//This macro runs the compiled eventQAmaker binary.
//The source code for eventQAmaker.cxx can be found at
//  src/qa/eventQAmaker.cxx

void RunEventQA(TString inputFile, Bool_t eventCuts, Int_t nEvents){

		//Load the Necessary Libraries
		gSystem->Load("../bin/TrackInfo_cxx.so");
    gSystem->Load("../bin/PrimaryVertexInfo_cxx.so");
    gSystem->Load("../bin/EventInfo_cxx.so");
    gSystem->Load("../bin/ParticleInfo_cxx.so");
  
    gSystem->Load("../bin/UserCuts_cxx.so");
    gSystem->Load("../bin/eventQAmaker_cxx.so");

    eventQAmaker(inputFile,eventCuts,nEvents);

}
