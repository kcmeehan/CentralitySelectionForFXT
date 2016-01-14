//Extracts the yield of pion minus and pion plus mesons.

#include <iostream>
#include <vector>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH3D.h>
#include <TF1.h>
#include <TClonesArray.h>
#include <TCanvas.h>
#include <TSystem.h>
#include <TStyle.h>
#include <TLine.h>
#include <TGraphErrors.h>

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

//Fit Functions
std::vector<std::vector<TF1 *> > tpcPionMinusFit(nRapidityBins,std::vector<TF1 *>(nmTm0Bins,(TF1 *)NULL));
std::vector<std::vector<TF1 *> > tpcPionPlusFit(nRapidityBins,std::vector<TF1 *>(nmTm0Bins,(TF1 *)NULL));

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
  
  DefineParticlePars(pion,"Pion");
  DefineParticlePars(kaon,"Kaon");
  DefineParticlePars(proton,"Proton");
  DefineParticlePars(electron,"Electron");

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
  TF1 *yieldFit   = NULL;
  Double_t rapidity(0), mTm0(0), mT(0), pZ(0), pT(0), pTotal(0);
  Double_t pionPredict(0), kaonPredict(0), protonPredict(0), electronPredict(0);
  Double_t lineHeight(0);

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
      yieldHisto->SetAxisRange(-1,2.5);

      //Make sure it has enough entires
      if (yieldHisto->GetEntries() < 100)
	continue;

      //Compute the kinematics for this bin
      rapidity = GetRapidityRangeCenter(yIndex);
      mTm0 = GetmTm0RangeCenter(mTm0Index);
      mT   = mTm0 + particleInfo->GetParticleMass(PION);
      pZ   = ComputepZ(mT,rapidity);
      pT   = ConvertmTm0ToPt(mTm0,particleInfo->GetParticleMass(PION));
      pTotal = ComputepTotal(pT,pZ);

      //Make the Predictions for the particle species
      pionPredict = particleInfo->PredictZTPC(pTotal,PION,PION);
      kaonPredict = particleInfo->PredictZTPC(pTotal,PION,KAON);
      protonPredict = particleInfo->PredictZTPC(pTotal,PION,PROTON);
      electronPredict = particleInfo->PredictZTPC(pTotal,PION,ELECTRON);

      //Stop if any of the confounding particles are overlapping
      if (fabs(pionPredict-kaonPredict) < 2*.08 ||
	  fabs(pionPredict-protonPredict) < 2*.08){
	continue;
      }

      //Create the Fit and pass it to a local pointer
      if (!tpcPionMinusFit.at(yIndex).at(mTm0Index)){
	tpcPionMinusFit.at(yIndex).at(mTm0Index) = new TF1("","gaus(0)",-.2,.2);
	yieldFit = tpcPionMinusFit.at(yIndex).at(mTm0Index);
      }

      //Draw the Histogram
      fittingCanvas->cd();
      tpcPionMinusHisto.at(yIndex).at(mTm0Index)->Draw("E");
      
      //Set the Parameters of the Fit Function
      yieldFit->SetParameter(0,yieldHisto->GetBinContent(yieldHisto->FindBin(0)));
      yieldFit->SetParameter(1,particleInfo->PredictZTPC(pTotal,PION,PION));
      yieldFit->SetParameter(2,.08);
      
      //Fit the Histogram
      yieldHisto->Fit(yieldFit,"RQ");

      //Draw the Prediction Lines
      lineHeight = yieldHisto->GetBinContent(yieldHisto->FindBin(pionPredict));
      PredictionLine(pionLine,pionPredict,lineHeight)->Draw("SAME");
      PredictionLine(kaonLine,kaonPredict,lineHeight)->Draw("SAME");
      PredictionLine(protonLine,protonPredict,lineHeight)->Draw("SAME");
      PredictionLine(electronLine,electronPredict,lineHeight)->Draw("SAME");

      fittingCanvas->Update();

      //Add the Mean and Width to their respective graphs
      AddPointToParGraph(pion->mean[yIndex],yieldFit,1,mTm0Index);
      AddPointToParGraph(pion->width[yIndex],yieldFit,2,mTm0Index);
      fitParCanvas->cd(2);
      pion->mean[yIndex]->Draw("APZ");
      fitParCanvas->cd(3);
      pion->width[yIndex]->Draw("APZ");
      fitParCanvas->Update();
      
      gSystem->Sleep(100);

    }//End Loop Over mTm0Index

    //Fit the Pion Width as a function of mTm0
    pion->widthFit[yIndex] = new TF1(Form("pionMinus_WidthFit_%02d",yIndex),
                                     SwooshFunc,.05,1.0,4);
    pion->widthFit[yIndex]->SetParameters(.2,.5,.3,.1);
    pion->widthFit[yIndex]->SetParLimits(3,.0,3);
    
    //Get the Maximum mTm0 from the graph and use it as a bound on the parameter
    Double_t max = pion->width[yIndex]->GetX()[pion->width[yIndex]->GetN()-1];
    pion->widthFit[yIndex]->SetParLimits(2,.1,max);
    
    fitParCanvas->cd(3);
    pion->width[yIndex]->Fit(pion->widthFit[yIndex],"RQ");

    fitParCanvas->Update();
    
    cout <<"Finished Parameterizing the Pion Width!" <<endl;
    

  }//End Loop Over yIndex


}

//--------------------------------------------------------------------
void FitPionPlus(){


}
