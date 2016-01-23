//Functions that are potentially useful in all scripts

#include <iostream>

#include <TGraphErrors.h>

#include "globalDefinitions.h"
#include "utilityFunctions.h"

//___________________________________________________________
Double_t GetRapidityRangeLow(Int_t rapidityIndex){

  return rapidityMin + (rapidityIndex * rapidityBinWidth);
}

//___________________________________________________________
Double_t GetRapidityRangeHigh(Int_t rapidityIndex){

  return GetRapidityRangeLow(rapidityIndex)+rapidityBinWidth;
}

//___________________________________________________________
Double_t GetRapidityRangeCenter(Int_t rapidityIndex){

  return (GetRapidityRangeLow(rapidityIndex) + GetRapidityRangeHigh(rapidityIndex)) / 2.0;
}

//___________________________________________________________
Int_t GetRapidityIndex(Double_t rapidity){

  //Make sure the value of rapidity is in the [min,max] range
  if (rapidity < rapidityMin || rapidity > rapidityMax)
    return -1;

  //Loop through the rapidity bins and return the index
  //of the bin that this value of rapidity falls into
  for (Int_t index=0; index<nRapidityBins; index++){
    
    if (rapidity >= GetRapidityRangeLow(index) &&
	rapidity < GetRapidityRangeHigh(index))
      return index;
  }

  return -1;
}

//___________________________________________________________
Double_t GetmTm0RangeLow(Int_t mTm0Index){

  return mTm0Min + (mTm0Index * mTm0BinWidth);
}

//___________________________________________________________
Double_t GetmTm0RangeHigh(Int_t mTm0Index){

  return GetmTm0RangeLow(mTm0Index) + mTm0BinWidth;

}

//___________________________________________________________
Double_t GetmTm0RangeCenter(Int_t mTm0Index){

  return (GetmTm0RangeLow(mTm0Index) + GetmTm0RangeHigh(mTm0Index)) / 2.0;
}

//___________________________________________________________
Int_t GetmTm0Index(Double_t mTm0){

  //Make sure the value of mTm0 is in the [min,max] range
  if (mTm0 < mTm0Min || mTm0 > mTm0Max)
    return -1;

  //Loop through the mTm0 bins and return the index
  //of the bin that this value of mTm0 falls into
  for (Int_t index=0; index<nmTm0Bins; index++){

    if (mTm0 >= GetmTm0RangeLow(index) &&
	mTm0 < GetmTm0RangeHigh(index))
      return index;
  }

  return -1;
}

//___________________________________________________________
Double_t ComputemTm0(Double_t pT, Double_t mass){

  return sqrt(pow(pT,2) + pow(mass,2)) - mass;
}

//___________________________________________________________
Double_t ConvertmTm0ToPt(Double_t mTm0, Double_t mass){

  return sqrt( pow(mTm0,2) + 2.0*mass*mTm0 );
}

//___________________________________________________________
Double_t ComputepTotal(Double_t pT, Double_t pZ){

  return sqrt(pow(pT,2) + pow(pZ,2));
}

//___________________________________________________________
Double_t ComputepZ(Double_t mT, Double_t rapidity){

  return mT*sinh(rapidity);
}

//___________________________________________________________
void TGraphChop(TGraphErrors *graph, Double_t threshold, Bool_t below){

  //Remove points from graph that are below (or above) the
  //the x value of threshold

  for (Int_t iPoint=graph->GetN()-1; iPoint>=0; iPoint--){

    //If the user wants to remove points above the threshold
    if (!below && graph->GetX()[iPoint] > threshold){
      graph->RemovePoint(iPoint);
    }

    else if (below && graph->GetX()[iPoint] < threshold) {
      graph->RemovePoint(iPoint);
    }

  }

}

//___________________________________________________________
TGraphErrors *TGraphScale(TGraphErrors *graph, Double_t scaleFactor){

  //Returns a new TGraph scaled by scaleFactor
  TGraphErrors *scaledGraph = new TGraphErrors();
  scaledGraph->SetName(graph->GetName());
  scaledGraph->SetTitle(graph->GetTitle());
  scaledGraph->SetMarkerStyle(graph->GetMarkerStyle());
  scaledGraph->SetMarkerColor(graph->GetMarkerColor());
  
  Int_t nPoints = graph->GetN();
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++){

    scaledGraph->SetPoint(iPoint,
			  graph->GetX()[iPoint],
			  graph->GetY()[iPoint]*scaleFactor);
    scaledGraph->SetPointError(iPoint,
			       graph->GetEX()[iPoint],
			       graph->GetEY()[iPoint]*scaleFactor);
    
  }
  
  return scaledGraph;
}

//_____________________________________________________________________
Double_t InvertBoseEinstein(Double_t Slope, Double_t pMass){

  //Returns the dN/dy of the Spectra Fits by numerically inverting the
  //Bose-Einstein Function.  Here we integrate the fitting function over dmT
  //from 0 to 5 GeV in .01 GeV Steps

  Double_t stepWidth = .01;
  Double_t maxRange = 5.0;
  Int_t nSteps = (Int_t)(maxRange/stepWidth);
  Double_t sum = 0.0;
  Double_t x = stepWidth/2.0; // x = mT-m0

  for (Int_t stepIndex=0; stepIndex < nSteps; stepIndex++){

    sum += ( (x + pMass)/(exp((x+pMass)/Slope)-1.0) ) * stepWidth;

    //Increment x
    x += stepWidth;

  }

  return sum;
}

//___________________________________________________________________________
Double_t BoseEinsteinFitFunc(Double_t *x, Double_t *par){

  //This is the Bose Einstein Function whihc is used to fit the spectra
  //It consists of the normalized part which contains the dNdy and the
  //BE Functional Form

  //Get the Independent Variable
  Double_t xx = x[0];

  //Get the Parameters
  Double_t dNdy = par[0];
  Double_t tSlope = par[1];
  Double_t pMass = par[2];   //This Should be Fixed!

  return (1.0/(2.0*3.14159)) * (dNdy/InvertBoseEinstein(tSlope,pMass)) * (1.0/(exp((xx+pMass)/tSlope) - 1.0));

}
