//Functions that are potentially useful in all scripts

#include <iostream>

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
