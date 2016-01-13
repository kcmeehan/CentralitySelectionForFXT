//Extracts the yield of pion minus and pion plus mesons.

#include <iostream>
#include <vector>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH3D.h>
#include <TClonesArray.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TLine.h>

#include "globalDefinitions.h"
#include "ParticleInfo.h"
#include "TrackInfo.h"
#include "PrimaryVertexInfo.h"
#include "EventInfo.h"

#include "utilityFunctions.h"
#include "fitZTPCUtilities.h"

//-------FUNCTIONS DEFINED BELOW MAIN ------
void FitPionMinus();
void FitPionPlus();

//-------GLOBALS--------

//Save?
Bool_t SAVEOBJ = true;
Bool_t SAVEIMG = true;

//Canvases
TCanvas *fittingCanvas, *fitParCanvas, *spectraCanvas;

//Particle Parameterizations and predictions
ParticleInfo *particleInfo;
ParticlePars *pion, *kaon, *proton, *electron;
TLine *pionLine, *kaonLine,*protonLine, *electronLine;

//ZTPC Variable Histograms
std::vector<std::vector<TH1D *> > tpcPionMinusHisto(nRapidityBins,std::vector<TH1D *>(nmTm0Bins,(TH1D *)NULL));
std::vector<std::vector<TH1D *> > tpcPionPlusHisto(nRapidityBins,std::vector<TH1D *>(nmTm0Bins,(TH1D *)NULL));

//Number of Events Histogram
TH1F *nEventsHisto;

//Spectra

//User Specified Indices
Int_t rapidityIndex;
Int_t centralityIndex;

//-------MAIN-------
void fitZTPCPions(TString ZTPCFILE, TString SPECTRAFILE, TString STARLIB, Int_t CENTRALITYINDEX,
		  Int_t RAPIDITYINDEX=-1, TString IMAGEDIR=""){

  //If the ImageDir argument is empty then turn off saving images                                        
  if (!IMAGEDIR.CompareTo("") && SAVEIMG == true)
    SAVEIMG = false;

  //If we are going to save images then create subdirectories                           
  if (SAVEIMG && !gSystem->OpenDirectory(IMAGEDIR+"/PionPlus/")){
    gSystem->mkdir(IMAGEDIR+"/PionPlus/");
  }
  if (SAVEIMG && !gSystem->OpenDirectory(IMAGEDIR+"PionMinus/")){
    gSystem->mkdir(IMAGEDIR+"/PionMinus/");
  }

  //Pass the Value of the user's input indices to the globals
  rapidityIndex = RAPIDITYINDEX;
  centralityIndex = CENTRALITYINDEX;

  //Create the Particle Info Object which has all the Bichesl Curves
  particleInfo = new ParticleInfo(STARLIB,true);

  //Create the Particle Parameterization Objects
  pion = new ParticlePars();
  kaon = new ParticlePars();
  proton = new ParticlePars();
  electron = new ParticlePars();
  
  DefineParticlePars(pion,"pion");
  DefineParticlePars(kaon,"kaon");
  DefineParticlePars(proton,"proton");
  DefineParticlePars(electron,"electron");

  //Particle ZTPC Prediction Lines
  pionLine = new TLine();
  kaonLine = new TLine();
  protonLine = new TLine();
  electronLine = new TLine();

  pionLine->SetLineColor(particleInfo->GetParticleColor(PION));
  kaonLine->SetLineColor(particleInfo->GetParticleColor(KAON));
  protonLine->SetLineColor(particleInfo->GetParticleColor(PROTON));
  electronLine->SetLineColor(particleInfo->GetParticleColor(ELECTRON));

  pionLine->SetLineWidth(2);
  kaonLine->SetLineWidth(2);
  protonLine->SetLineWidth(2);
  electronLine->SetLineWidth(2);

  //Get the Yield Histogram file and Get the 3D Z Histograms 
  TFile *inFile = new TFile(ZTPCFILE,"READ");
  TH3D *pionPlusZTPC3D = (TH3D *)inFile->Get(Form("PionPlus/zTPC_PionPlus_Cent%d",centralityIndex));
  TH3D *pionMinusZTPC3D = (TH3D *)inFile->Get(Form("PionMinus/zTPC_PionMinus_Cent%d",centralityIndex));
  LoadYieldHistograms(pionPlusZTPC3D,&tpcPionPlusHisto,rapidityIndex);
  LoadYieldHistograms(pionMinusZTPC3D,&tpcPionMinusHisto,rapidityIndex);
     
  //Set Style Options
  gStyle->SetOptStat(0);
  gStyle->SetOptFit(112);

  //Create Global Canvases
  fittingCanvas = new TCanvas("fittingCanvas","Fitting Canvas",20,20,800,600);
  fittingCanvas->SetLogy();
  fitParCanvas = new TCanvas("fitParCanvas","Fit Parameter Canvas",20,20,1100,1100);
  fitParCanvas->Divide(3,3);
  spectraCanvas = new TCanvas("spectraCanvas","Spectra Canvas",20,850,800,600);
  spectraCanvas->SetLogy();

  
  //Fit the Pion Minuses
  FitPionMinus();

}


//------------------------------------------------------------------------
void FitPionMinus(){

  //Local Variables
  TH1D *yieldHisto = NULL;

  for (Int_t yIndex=0; yIndex<nRapidityBins; yIndex++){


    //If the user has specified a rapidityIndex then only do that index
    if (rapidityIndex >= 0 && yIndex != rapidityIndex)
      continue;

    cout <<"Working on Rapidity Index: " <<yIndex <<endl;

    for (Int_t mTm0Index=2; mTm0Index<nmTm0Bins; mTm0Index++){

      //Make sure the histogram exists
      if (!tpcPionMinusHisto.at(yIndex).at(mTm0Index))
	continue;

      //Pass the histogram to a local pointer
      yieldHisto = tpcPionMinusHisto.at(yIndex).at(mTm0Index);

      //make sure it has enough entires
      if (yieldHisto->GetEntries() < 100)
	continue;

      //Compute the kinematics for this bin
      Double_t rapidity = GetRapidityRangeCenter(yIndex);
      Double_t mTm0 = GetmTm0RangeCenter(mTm0Index);
      Double_t mT   = mTm0 + particleInfo->GetParticleMass(PION);
      Double_t pZ   = ComputepZ(mT,rapidity);
      Double_t pT   = ConvertmTm0ToPt(mTm0,particleInfo->GetParticleMass(PION));
      Double_t pTotal = ComputepTotal(pT,pZ);

      yieldHisto->SetAxisRange(-1,2.5);

      fittingCanvas->cd();
      tpcPionMinusHisto.at(yIndex).at(mTm0Index)->Draw("E");

      Double_t lineHeight = 
	yieldHisto->GetBinContent(yieldHisto->FindBin(particleInfo->
						       PredictZTPC(pTotal,PION,PION)));
      PredictionLine(pionLine,particleInfo->PredictZTPC(pTotal,PION,PION),lineHeight)->Draw("SAME");
      PredictionLine(kaonLine,particleInfo->PredictZTPC(pTotal,PION,KAON),lineHeight)->Draw("SAME");
      PredictionLine(protonLine,particleInfo->PredictZTPC(pTotal,PION,PROTON),lineHeight)->Draw("SAME");
      PredictionLine(electronLine,particleInfo->PredictZTPC(pTotal,PION,ELECTRON),lineHeight)->Draw("SAME");

      fittingCanvas->Update();

      gSystem->Sleep(1000);

    }

  }//End Loop Over yIndex


}

//--------------------------------------------------------------------
void FitPionPlus(){


}
