//These are commonly used functions that are useful which extracting the yield
//of particles for spectra

#ifndef FITZTPCUTILITIES_H
#define FITZTPCUTILITIES_H

#include "globalDefinitions.h"

class TH1D;
class TH3D;
class TString;
class TGraphErrors;
class TLine;

//Structure to hold the information for the Particle Parameterizations
struct ParticlePars{

  //Parameters as a function of mTm0
  TGraphErrors *amp[nRapidityBins], *mean[nRapidityBins],
    *width[nRapidityBins];
  Int_t ampPoint[nRapidityBins], meanPoint[nRapidityBins],
    widthPoint[nRapidityBins];
  
  TF1 *ampFit[nRapidityBins], *meanFit[nRapidityBins],
    *widthFit[nRapidityBins];
  
};

//Functions Implemented in src/analysis/fitZTPCUtilities.cxx
TH1D *GetYieldHistogram(TH3D *yieldHisto3D, Double_t rapidity, Double_t mTm0, TString detector);
void LoadYieldHistograms(TH3D *yieldHisto3D, std::vector<std::vector<TH1D *> > *zHistos, 
			 TString detector="TPC", Int_t rapidityIndex=-1);
void DefineParticlePars(ParticlePars *p, TString name);
TLine *PredictionLine(TLine *line, Double_t prediction, Double_t height);

#endif
