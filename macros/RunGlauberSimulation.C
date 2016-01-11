//This Macro simply calls the Glauber Model Simulation Function
//With the Given parameters. This macro can be called from the
//bash script in the parent directory called runGlauberSimulation.sh

void RunGlauberSimulation(Int_t nEvents, Int_t nNucleonsA, Int_t nNucleonsB,
			  Double_t nnCrossSection, Int_t model=0, TString outDir=""){

  //Load the Necessary Libraries
  gSystem->Load("../bin/GlauberClass_cxx");
  gSystem->Load("../bin/GlauberModel_cxx");

  //Run the Glauber Simulation
  GlauberModel(nEvents,nNucleonsA,nNucleonsB,nnCrossSection,model,outDir);
  
}
