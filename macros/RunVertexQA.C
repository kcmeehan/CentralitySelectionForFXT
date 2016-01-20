//This macro runs the compiled vertexQAmaker binary.
//The source code for vertexQAmaker.cxx can be found at
//  src/qa/vertexQAmaker.cxx

void RunVertexQA(TString inputFile, TString outputFile, Bool_t vertexCuts, Int_t nEvents){

		//Load the Necessary Libraries
		gSystem->Load("../bin/TrackInfo_cxx.so");
    gSystem->Load("../bin/PrimaryVertexInfo_cxx.so");
    gSystem->Load("../bin/EventInfo_cxx.so");
    gSystem->Load("../bin/ParticleInfo_cxx.so");
  
    gSystem->Load("../bin/UserCuts_cxx.so");
    gSystem->Load("../bin/vertexQAmaker_cxx.so");

    vertexQAmaker(inputFile,outputFile,vertexCuts,nEvents);

}
