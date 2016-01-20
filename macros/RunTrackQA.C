//This macro runs the compiled trackQAmaker binary.
//The source code for trackQAmaker.cxx can be found at
//  src/qa/trackQAmaker.cxx

void RunTrackQA(TString inputFile, TString outputFile, Bool_t trackCuts, Int_t nEvents){

		//Load the Necessary Libraries
		gSystem->Load("../bin/TrackInfo_cxx.so");
    gSystem->Load("../bin/PrimaryVertexInfo_cxx.so");
    gSystem->Load("../bin/EventInfo_cxx.so");
    gSystem->Load("../bin/ParticleInfo_cxx.so");
  
    gSystem->Load("../bin/UserCuts_cxx.so");
    gSystem->Load("../bin/trackQAmaker_cxx.so");

    trackQAmaker(inputFile,outputFile,trackCuts,nEvents);

}
