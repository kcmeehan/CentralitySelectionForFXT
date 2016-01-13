//Implementations for the commonly used functions that are useful which extracting the yield
//of particles species for spectra

#include <iostream>
#include <vector>

#include <TString.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TGraphErrors.h>
#include <TLine.h>

#include "globalDefinitions.h"
#include "utilityFunctions.h"
#include "fitZTPCUtilities.h"

//______________________________________________________________________________________________________
TH1D *GetYieldHistogram(TH3D *yieldHisto3D, Double_t rapidity, Double_t mTm0, TString detector){

  //Return a 1D histogram from the 3D histogram cut on the values of 
  //rapidity and mTm0 for the given detector

  //Find the bins in the 3D histogram corresponding ot the values of rapidity and mTm0
  Int_t rapidityBin = yieldHisto3D->GetXaxis()->FindBin(rapidity);
  Int_t mTm0Bin     = yieldHisto3D->GetYaxis()->FindBin(mTm0);

  //Create the 1D Histogram 
  TH1D *zProj = yieldHisto3D->ProjectionZ(Form("%s_%d_%d",yieldHisto3D->GetName(),rapidityBin,mTm0Bin),
					  rapidityBin,rapidityBin,mTm0Bin,mTm0Bin);

  //Set Axis Names and Histogram Title
  zProj->GetYaxis()->SetTitle(Form("dN_{Ch}/dZ_{%s}",detector.Data()));
  zProj->GetXaxis()->SetTitle(Form("Z_{%s}",detector.Data()));
  zProj->SetTitle(Form("%s, y= [%g,%g], m_{T}-m_{0}= [%g,%g]",yieldHisto3D->GetTitle(),
		       yieldHisto3D->GetXaxis()->GetBinLowEdge(rapidityBin),
		       yieldHisto3D->GetXaxis()->GetBinLowEdge(rapidityBin)+
		       yieldHisto3D->GetXaxis()->GetBinWidth(rapidityBin),
		       yieldHisto3D->GetYaxis()->GetBinLowEdge(mTm0Bin),
		       yieldHisto3D->GetYaxis()->GetBinLowEdge(mTm0Bin)+
		       yieldHisto3D->GetYaxis()->GetBinWidth(mTm0Bin)));
  
  return zProj;

}

//______________________________________________________________________________________________________
void LoadYieldHistograms(TH3D *yieldHisto3D, std::vector<std::vector<TH1D *> > *zHistos, 
			 TString detector, Int_t rapidityIndex){
   
  //Loop Over the rapidity and mTm0 Bins and load all the zHistograms
  for (Int_t yIndex=0; yIndex<nRapidityBins; yIndex++){
    
    //If the user has specified a certain rapidity index then only
    //load histograms in that index
    if (rapidityIndex >=0 && rapidityIndex != yIndex)
      continue;

    for (Int_t mTm0Index=0; mTm0Index<nmTm0Bins; mTm0Index++){
      
      zHistos->at(yIndex).at(mTm0Index) = 
	GetYieldHistogram(yieldHisto3D,GetRapidityRangeCenter(yIndex),
			  GetmTm0RangeCenter(mTm0Index),detector);

    }//End Loop Over mTm0Index
  }//End Loop Over yIndex


}

//______________________________________________________________________________________________________ 
void DefineParticlePars(ParticlePars *p, TString name){

  //Create the objects held in the ParticleParas structure.
  
  for (Int_t yIndex=0; yIndex<nRapidityBins; yIndex++){

    p->amp[yIndex] = new TGraphErrors();
    p->amp[yIndex]->SetName(Form("ampGraph_%s_%02d",name.Data(),yIndex));
    p->amp[yIndex]->SetTitle(Form("%s Amplitude %02d",name.Data(),yIndex));
    p->amp[yIndex]->SetMarkerStyle(20);
    p->ampPoint[yIndex] = 0;

    p->mean[yIndex] = new TGraphErrors();
    p->mean[yIndex]->SetName(Form("meanGraph_%s_%02d",name.Data(),yIndex));
    p->mean[yIndex]->SetTitle(Form("%s Mean %02d",name.Data(),yIndex));
    p->mean[yIndex]->SetMarkerStyle(20);
    p->meanPoint[yIndex] = 0;

    p->amp[yIndex] = new TGraphErrors();
    p->amp[yIndex]->SetName(Form("ampGraph_%s_%02d",name.Data(),yIndex));
    p->amp[yIndex]->SetTitle(Form("%s Amplitude %02d",name.Data(),yIndex));
    p->amp[yIndex]->SetMarkerStyle(20);
    p->ampPoint[yIndex] = 0;

    p->mean[yIndex] = new TGraphErrors();
    p->mean[yIndex]->SetName(Form("meanGraph_%s_%02d",name.Data(),yIndex));
    p->mean[yIndex]->SetTitle(Form("%s Mean %02d",name.Data(),yIndex));
    p->mean[yIndex]->SetMarkerStyle(20);
    p->meanPoint[yIndex] = 0;

    p->width[yIndex] = new TGraphErrors();
    p->width[yIndex]->SetName(Form("widthGraph_%s_%02d",name.Data(),yIndex));
    p->width[yIndex]->SetTitle(Form("%s Width %02d",name.Data(),yIndex));
    p->width[yIndex]->SetMarkerStyle(20);
    p->widthPoint[yIndex] = 0;

  }

}

//_______________________________________________________________________________
TLine *PredictionLine(TLine *line, Double_t prediction, Double_t height){

  line->SetX1(prediction);
  line->SetX2(prediction);
  line->SetY1(0);
  line->SetY2(height);

  return line;
}
