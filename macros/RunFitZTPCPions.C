void RunFitZTPCPions(TString yieldHistoFile, TString outputSpectraFile, TString starLibrary,
		     Int_t centralityIndex, Double_t rapidity=-999, TString imageDir=""){

  gSystem->Load("../bin/utilityFunctions_cxx.so");
  gSystem->Load("../bin/ParticleInfo_cxx.so");
  gSystem->Load("../bin/fitZTPCUtilities_cxx.so");
  gSystem->Load("../bin/fitZTPCPions_cxx.so");

  //Find the rapidity index of the user specified rapidity value
  Int_t rapidityIndex = -1;
  if (rapidity != -999)
    rapidityIndex = GetRapidityIndex(rapidity);

  //Check to make sure the rapidityIndex is good before continuing
  if (rapidity != -999 && rapidityIndex < 0){
    cout <<"ERROR in RunFitZTPCPions.C: value of rapidity is not in rapidity range! EXITING" <<endl;
    return;
  }

  fitZTPCPions(yieldHistoFile,outputSpectraFile,starLibrary,centralityIndex,rapidityIndex,imageDir);

}
