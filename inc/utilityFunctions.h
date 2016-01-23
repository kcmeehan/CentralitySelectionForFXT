//Functions are that potentially useful in all sripts

#ifndef UTILITYFUNCTIONS_H
#define UTILITYFUNCTIONS_H

//Kinematics
Double_t GetRapidityRangeLow(Int_t rapidityIndex);
Double_t GetRapidityRangeHigh(Int_t rapidityIndex); 
Double_t GetRapidityRangeCenter(Int_t rapidityIndex);
Int_t GetRapidityIndex(Double_t rapidity);
Double_t GetmTm0RangeLow(Int_t mTm0Index);
Double_t GetmTm0RangeHigh(Int_t mTm0Index);
Double_t GetmTm0RangeCenter(Int_t mTm0Index);
Int_t GetmTm0Index(Double_t mTm0);
Double_t ComputemTm0(Double_t pT, Double_t mass);
Double_t ConvertmTm0ToPt(Double_t mTm0, Double_t mass);
Double_t ComputepTotal(Double_t pT, Double_t pZ);
Double_t ComputepZ(Double_t mTm0, Double_t rapidity);

//TGraph Operations
void TGraphChop(TGraphErrors *graph, Double_t threshold, Bool_t below=true);
TGraphErrors *TGraphScale(TGraphErrors *graph, Double_t scaleFactor);

//Spectra Fits
Double_t InvertBoseEinstein(Double_t Slope, Double_t pMass);
Double_t BoseEinsteinFitFunc(Double_t *x, Double_t *par);

#endif
