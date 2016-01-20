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
#include <TPaveText.h>
#include <TPaveStats.h>
#include <TLegend.h>

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
void DrawPredictionLines(TCanvas *canvas);
void ComputeBinKinematics(Int_t mTm0Index, Double_t y, Int_t PID);
Bool_t AddPointToSpectra(TH1D *yieldHisto, TF1 *yieldFit, TGraphErrors *rawSpectra, Int_t yIndex,
			 Int_t mTm0Index, Int_t PID, Int_t CHARGE, TString IMAGEDIR="");

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
Double_t rapidity, mTm0, mT, pZ, pT, pTotal;
Double_t pionPredict, kaonPredict, protonPredict, electronPredict;
Double_t lineHeight;

//ZTPC Variable Histograms
std::vector<std::vector<TH1D *> > tpcPionMinusHisto(nRapidityBins,std::vector<TH1D *>(nmTm0Bins,(TH1D *)NULL));
std::vector<std::vector<TH1D *> > tpcPionPlusHisto(nRapidityBins,std::vector<TH1D *>(nmTm0Bins,(TH1D *)NULL));

//Fit Functions
std::vector<std::vector<TF1 *> > tpcPionMinusFit(nRapidityBins,std::vector<TF1 *>(nmTm0Bins,(TF1 *)NULL));
std::vector<std::vector<TF1 *> > tpcPionPlusFit(nRapidityBins,std::vector<TF1 *>(nmTm0Bins,(TF1 *)NULL));

//Raw Spectra
std::vector<TGraphErrors *> rawSpectraMinus(nRapidityBins,(TGraphErrors *)NULL);
std::vector<TGraphErrors *> rawSpectraPlus(nRapidityBins,(TGraphErrors *)NULL);

//Number of Events Histogram
TH1D *nEventsHisto;

//Species Controls
Bool_t killElectron;

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

  killElectron = false;
  
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
  nEventsHisto = (TH1D *)inFile->Get("nEvents");
     
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

  //Make the Pion Minus Spectra
  for (Int_t yIndex=0; yIndex<nRapidityBins; yIndex++){
    
    //If the user has specified a rapidityIndex then only do that index
    if (rapidityIndex >= 0 && yIndex != rapidityIndex)
      continue;

    for (Int_t mTm0Index=2; mTm0Index<nmTm0Bins; mTm0Index++){

      //If the spectra doesn't exist already then make it
      if (!rawSpectraMinus.at(yIndex)){
	
	rawSpectraMinus.at(yIndex) = new TGraphErrors();
	rawSpectraMinus.at(yIndex)->SetMarkerStyle(20);
	rawSpectraMinus.at(yIndex)->SetName(Form("rawSpectra_%s_Cent%02d_yIndex_%02d",
				 particleInfo->GetParticleName(PION,-1),
				 centralityIndex,yIndex));
	//rawSpectra->SetTitle(Form(""));
      }

      
      Bool_t success = AddPointToSpectra(tpcPionMinusHisto.at(yIndex).at(mTm0Index),
					 tpcPionMinusFit.at(yIndex).at(mTm0Index),
					 rawSpectraMinus.at(yIndex),
					 yIndex,mTm0Index,PION,-1,IMAGEDIR);

      
    }//End Loop Over mTm0Index
    
  }//End Loop Over yIndex

  //Fit the Pion Pluses
  FitPionPlus();

  //Make the Pion Plus Spectra
  for (Int_t yIndex=0; yIndex<nRapidityBins; yIndex++){

    //If the user has specified a rapidityIndex then only do that index
    if (rapidityIndex >= 0 && yIndex != rapidityIndex)
      continue;
    
    for (Int_t mTm0Index=2; mTm0Index<nmTm0Bins; mTm0Index++){
      
      //If the spectra doesn't exist already then make it
      if (!rawSpectraPlus.at(yIndex)){
	
	rawSpectraPlus.at(yIndex) = new TGraphErrors();
	rawSpectraPlus.at(yIndex)->SetMarkerStyle(20);
	rawSpectraPlus.at(yIndex)->SetName(Form("rawSpectra_%s_Cent%02d_yIndex_%02d",
						 particleInfo->GetParticleName(PION,1),
						 centralityIndex,yIndex));
	//rawSpectra->SetTitle(Form(""));
      }
      
      
      Bool_t success = AddPointToSpectra(tpcPionPlusHisto.at(yIndex).at(mTm0Index),
					 tpcPionPlusFit.at(yIndex).at(mTm0Index),
					 rawSpectraPlus.at(yIndex),
					 yIndex,mTm0Index,PION,-1,IMAGEDIR);
      
      
    }//End Loop Over mTm0Index

  }//End Loop Over yIndex
  
}


//------------------------------------------------------------------------
void FitPionMinus(){

  //Local Variables
  TH1D *yieldHisto = NULL;
  TF1 *yieldFit   = NULL;

  for (Int_t yIndex=0; yIndex<nRapidityBins; yIndex++){

    //If the user has specified a rapidityIndex then only do that index
    if (rapidityIndex >= 0 && yIndex != rapidityIndex)
      continue;

    //Compute the Rapidity
    rapidity = GetRapidityRangeCenter(yIndex);
    
    cout <<"Working on Rapidity Index: " <<yIndex <<" Rapidity: " <<rapidity <<endl;

    //----- ROUND 1 - PARAMETERIZE THE PION WIDTH -----
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
      ComputeBinKinematics(mTm0Index,rapidity,PION);
       
      //Make the Predictions for the particle species
      pionPredict = particleInfo->PredictZTPC(pTotal,PION,PION);
      kaonPredict = particleInfo->PredictZTPC(pTotal,PION,KAON);
      protonPredict = particleInfo->PredictZTPC(pTotal,PION,PROTON);
      electronPredict = particleInfo->PredictZTPC(pTotal,PION,ELECTRON);

      //Stop if any of the confounding particles are overlapping
      /*
      if (fabs(pionPredict-kaonPredict) < 2*.08 ||
	  fabs(pionPredict-protonPredict) < 2*.08){
	continue;
      }
      */

      //Stop at this mTm0
      if (mTm0 > .5)
	break;

      //Create the Fit and pass it to a local pointer
      if (!tpcPionMinusFit.at(yIndex).at(mTm0Index)){
	tpcPionMinusFit.at(yIndex).at(mTm0Index) = new TF1("","gaus(0)",-.2,.125);
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
      DrawPredictionLines(fittingCanvas);
      fittingCanvas->Update();

      //Add the Mean and Width to their respective graphs
      AddPointToParGraph(pion->mean[yIndex],yieldFit,1,mTm0Index);
      AddPointToParGraph(pion->width[yIndex],yieldFit,2,mTm0Index);
      fitParCanvas->cd(2);
      pion->mean[yIndex]->Draw("APZ");
      fitParCanvas->cd(3);
      pion->width[yIndex]->Draw("APZ");
      fitParCanvas->Update();
      
      //gSystem->Sleep(100);

    }//End Loop Over mTm0Index

    //Check to make sure there are entries in the parameter graphs
    if (pion->width[yIndex]->GetN() < 1){
      cout <<"fitZTPCPions.cxx:FitPionMinus() WARNING: No successful parameterizations for rapidity index: "
	   <<yIndex <<", rapidity = " <<rapidity <<endl;
      continue;
    }
    

    //Fit the Pion Width as a function of mTm0
    pion->widthFit[yIndex] = new TF1(Form("pionMinus_WidthFit_%02d",yIndex),
                                     SwooshFunc,.05,1.0,4);
    pion->widthFit[yIndex]->SetParameters(.2,.5,.3,.1);
    pion->widthFit[yIndex]->SetParLimits(3,.0,3);
    
    //Get the Maximum mTm0 from the graph and use it as a bound on the parameter
    Double_t max = pion->width[yIndex]->GetX()[pion->width[yIndex]->GetN()-1];
    pion->widthFit[yIndex]->SetParLimits(2,.1,max);
    
    /*
    pion->widthFit[yIndex] = new TF1(Form("pionMinus_WidthFit_%02d",yIndex),
				     "[0]+[1]*x+[2]/sqrt(x)",.05,1.0);
    pion->widthFit[yIndex]->SetParameters(0.02,0.03,0.02);
    */
    fitParCanvas->cd(3);
    pion->width[yIndex]->Fit(pion->widthFit[yIndex],"RQ");

    fitParCanvas->Update();
    
    cout <<"Finished Parameterizing the Pion Width!" <<endl;


    //----- ROUND2 - PARAMETERIZE THE ELECTRON MEAN AND WIDTH -----
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
      ComputeBinKinematics(mTm0Index,rapidity,PION);
      
      //Make the Predictions for the particle species
      pionPredict = particleInfo->PredictZTPC(pTotal,PION,PION);
      kaonPredict = particleInfo->PredictZTPC(pTotal,PION,KAON);
      protonPredict = particleInfo->PredictZTPC(pTotal,PION,PROTON);
      electronPredict = particleInfo->PredictZTPC(pTotal,PION,ELECTRON);

      //Stop at this mTm0
      if (mTm0 > .5)
	break;

      //Get the Previous Pion Mean
      Double_t pionMeanPrev = pion->mean[yIndex]->Eval(mTm0);
      
      //Create the Fit and pass it to a local pointer
      if (tpcPionMinusFit.at(yIndex).at(mTm0Index))
	tpcPionMinusFit.at(yIndex).at(mTm0Index)->Delete();
      
      tpcPionMinusFit.at(yIndex).at(mTm0Index) = new TF1("","gaus(0)+gaus(3)",-.2,electronPredict+.125);
      yieldFit = tpcPionMinusFit.at(yIndex).at(mTm0Index);

      //Pion Parameters
      yieldFit->SetParameter(0,yieldHisto->GetBinContent(yieldHisto->FindBin(pionMeanPrev)));
      yieldFit->FixParameter(1,pionMeanPrev);
      yieldFit->FixParameter(2,pion->widthFit[yIndex]->Eval(mTm0));

      //Electron Parameters
      yieldFit->SetParameter(3,yieldHisto->GetBinContent(yieldHisto->FindBin(electronPredict)));
      yieldFit->SetParameter(4,electronPredict);
      yieldFit->SetParameter(5,.08);
      yieldFit->SetParLimits(3,yieldFit->GetParameter(3)*.8, yieldFit->GetParameter(3)*1.02);
      yieldFit->SetParLimits(4,electronPredict*.9,electronPredict*1.1);
      yieldFit->SetParLimits(5,.07,.12);			     
      
      //Draw the Histogram
      fittingCanvas->cd();
      yieldHisto->Draw("E");

      //Fit the Yield Histogram
      yieldHisto->Fit(yieldFit,"RQ");
      fittingCanvas->Update();

      //Draw the Prediction Lines
      lineHeight = yieldHisto->GetBinContent(yieldHisto->FindBin(pionPredict));
      DrawPredictionLines(fittingCanvas);
      fittingCanvas->Update();

      //Add the points to the fit par Graphs
      AddPointToParGraph(electron->mean[yIndex],yieldFit,4,mTm0Index);
      AddPointToParGraph(electron->width[yIndex],yieldFit,5,mTm0Index);

      //Draw the Parameter Graphs
      fitParCanvas->cd(5);
      electron->mean[yIndex]->Draw("APZ");
      fitParCanvas->cd(6);
      electron->width[yIndex]->Draw("APZ");
      fitParCanvas->Update();

      //gSystem->Sleep(100);

    }//End Loop Over mTm0Index

    //Get the number of points in the electron mean graph and decide if it
    //should be killed or not
    if (electron->mean[yIndex]->GetN() < 5){
      killElectron = true;
      cout <<"INFO: fitZTPCPions.cxx:FitPionMinus(): The electron has ben killed due to too few points in the mean parameterization in rapidity index: " <<yIndex <<" rapidity = " <<rapidity <<endl;
    }

    //If the electron is not killed then fit its mean and width
    if (!killElectron){
      
      //Electron Mean
      electron->meanFit[yIndex] = new TF1(Form("electronMinus_MeanFit_%02d",yIndex),
					  particleInfo,&ParticleInfo::ConfoundMeanFitTPC,.03,1.0,6);
      electron->meanFit[yIndex]->FixParameter(0,PION);
      electron->meanFit[yIndex]->FixParameter(1,ELECTRON);
      electron->meanFit[yIndex]->FixParameter(2,rapidity);
      electron->meanFit[yIndex]->SetParameter(3,0.0);
      electron->meanFit[yIndex]->SetParameter(4,1.0);
      electron->meanFit[yIndex]->SetParLimits(4,.5,1.5);
      electron->meanFit[yIndex]->SetParameter(5,1);
      electron->meanFit[yIndex]->SetParLimits(5,.6,1.2);

      //Electron Width
      electron->widthFit[yIndex] = new TF1(Form("electronMinus_WidthFit_%02d",yIndex),
					   SwooshFunc,.05,1.0,4);
      electron->widthFit[yIndex]->SetParameters(.2,.5,.3,.1);
      electron->widthFit[yIndex]->SetParLimits(3,.0,3);

      //Get the Maximum mTm0 from the graph and use it as a bound on the parameter
      Double_t max = electron->width[yIndex]->GetX()[electron->width[yIndex]->GetN()-1];
      electron->widthFit[yIndex]->SetParLimits(2,.1,max);
    }
    else{
      electron->meanFit[yIndex] = NULL;
      electron->widthFit[yIndex] = NULL;
    }

    fitParCanvas->cd(5);
    electron->mean[yIndex]->Fit(electron->meanFit[yIndex],"R");
    fitParCanvas->cd(6);
    electron->width[yIndex]->Fit(electron->widthFit[yIndex],"R");
    fitParCanvas->Update();
    
    cout <<"Finished Parameterizing the Electron mean and width!" <<endl;

    //----- ROUND 3: PARAMETERIZE THE ELECTRON AMPLITUDE
    for (Int_t mTm0Index=2; mTm0Index<nmTm0Bins; mTm0Index++){

      //Make sure that the electron hasn't already been killed
      if (killElectron)
	continue;
   
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
      ComputeBinKinematics(mTm0Index,rapidity,PION);
      
      //Make the Predictions for the particle species
      pionPredict = particleInfo->PredictZTPC(pTotal,PION,PION);
      kaonPredict = particleInfo->PredictZTPC(pTotal,PION,KAON);
      protonPredict = particleInfo->PredictZTPC(pTotal,PION,PROTON);
      electronPredict = particleInfo->PredictZTPC(pTotal,PION,ELECTRON);

      //Stop at this mTm0
      if (mTm0 > 1.0)
	break;

      //Get the Previous Pion Mean
      Double_t pionMeanPrev = pion->mean[yIndex]->Eval(mTm0);
      
      //Create the Fit and pass it to a local pointer
      if (tpcPionMinusFit.at(yIndex).at(mTm0Index))
	tpcPionMinusFit.at(yIndex).at(mTm0Index)->Delete();
      
      tpcPionMinusFit.at(yIndex).at(mTm0Index) = new TF1("","gaus(0)+gaus(3)",-.2,electronPredict+.125);
      yieldFit = tpcPionMinusFit.at(yIndex).at(mTm0Index);
      
      //Pion Parameters
      yieldFit->SetParameter(0,yieldHisto->GetBinContent(yieldHisto->FindBin(pionMeanPrev)));
      yieldFit->FixParameter(1,pionMeanPrev);
      yieldFit->FixParameter(2,pion->widthFit[yIndex]->Eval(mTm0));

      //Electron Parameters
      yieldFit->SetParameter(3,yieldHisto->GetBinContent(yieldHisto->FindBin(electronPredict)));
      yieldFit->FixParameter(4,electron->meanFit[yIndex]->Eval(mTm0));
      yieldFit->FixParameter(5,electron->widthFit[yIndex]->Eval(mTm0));

      fittingCanvas->cd();
      yieldHisto->Draw("E");
      yieldHisto->Fit(yieldFit,"RQ");
      lineHeight = yieldHisto->GetBinContent(yieldHisto->FindBin(pionPredict));
      DrawPredictionLines(fittingCanvas);      
      fittingCanvas->Update();

      //Add Point to the Electron Parameter Graph
      AddPointToParGraph(electron->amp[yIndex],yieldFit,3,mTm0Index);

      fitParCanvas->cd(4);
      electron->amp[yIndex]->Draw("APZ");
      fitParCanvas->Update();
      
    }//End Loop Over mTm0Index


    //----- ROUND 4: Final Fitting Loop
    for (Int_t mTm0Index=2; mTm0Index<nmTm0Bins; mTm0Index++){

      //Make sure that the electron hasn't already been killed
      if (killElectron)
	continue;
   
      //Make sure the histogram exists
      if (!tpcPionMinusHisto.at(yIndex).at(mTm0Index))
	continue;

      //Pass the histogram to a local pointer
      yieldHisto = tpcPionMinusHisto.at(yIndex).at(mTm0Index);
 
      //Make sure it has enough entires
      if (yieldHisto->GetEntries() < 100)
	continue;

      //Compute the kinematics for this bin
      ComputeBinKinematics(mTm0Index,rapidity,PION);

      if (mTm0 > 1.0)
	break;
      
      //Make the Predictions for the particle species
      pionPredict = particleInfo->PredictZTPC(pTotal,PION,PION);
      kaonPredict = particleInfo->PredictZTPC(pTotal,PION,KAON);
      protonPredict = particleInfo->PredictZTPC(pTotal,PION,PROTON);
      electronPredict = particleInfo->PredictZTPC(pTotal,PION,ELECTRON);

      //Get the Previous Pion Mean
      Double_t pionMeanPrev = pion->mean[yIndex]->Eval(mTm0);

      //Draw the Yield Histogram
      yieldHisto->SetAxisRange(-1,2.5);
      yieldHisto->SetMarkerStyle(kOpenCircle);
      fittingCanvas->cd();
      yieldHisto->Draw("E");
      
      //Create the Fit and pass it to a local pointer
      if (tpcPionMinusFit.at(yIndex).at(mTm0Index))
	tpcPionMinusFit.at(yIndex).at(mTm0Index)->Delete();
      
      //The Final Fit depends on the status of the electron
      if (!killElectron){
		
	tpcPionMinusFit.at(yIndex).at(mTm0Index) = new TF1("","gaus(0)+gaus(3)",-.2,electronPredict+.125);
	yieldFit = tpcPionMinusFit.at(yIndex).at(mTm0Index);

	//Set Parameter Names
	yieldFit->SetParNames(Form("%s_{Amp.}",particleInfo->GetParticleSymbol(PION).Data()),
			      "#mu_{#pi}","#sigma_{#pi}","e_{Amp.}",
			      "#mu_{e}","#sigma_{e}");
	
	//Pion Parameters
	yieldFit->SetParameter(0,yieldHisto->GetBinContent(yieldHisto->FindBin(pionMeanPrev)));
	yieldFit->FixParameter(1,pionMeanPrev);
	yieldFit->FixParameter(2,pion->widthFit[yIndex]->Eval(mTm0));

	//Electron Parameters
	Double_t eAmpSeed = yieldHisto->GetBinContent(yieldHisto->FindBin(electron->meanFit[yIndex]->Eval(mTm0)));
	if (mTm0 >= electron->amp[yIndex]->GetX()[0] &&
	    mTm0 <= electron->amp[yIndex]->GetX()[electron->amp[yIndex]->GetN()-1])
	  eAmpSeed = electron->amp[yIndex]->Eval(mTm0);
	yieldFit->FixParameter(3,eAmpSeed);
	yieldFit->FixParameter(4,electron->meanFit[yIndex]->Eval(mTm0));
	yieldFit->FixParameter(5,electron->widthFit[yIndex]->Eval(mTm0));	
      }//End if electron has NOT been killed

      else {

   	tpcPionMinusFit.at(yIndex).at(mTm0Index) =
	  new TF1("","gaus(0)",-1,pionMeanPrev+2*pion->widthFit[yIndex]->Eval(mTm0));
	yieldFit = tpcPionMinusFit.at(yIndex).at(mTm0Index);

	//Set Parameter Names
	yieldFit->SetParNames(Form("%s_{Amp.}",particleInfo->GetParticleSymbol(PION).Data()),
			      "#mu_{#pi}","#sigma_{#pi}");
	
	//Pion Parameters
	yieldFit->SetParameter(0,yieldHisto->GetBinContent(yieldHisto->FindBin(pionMeanPrev)));
	yieldFit->FixParameter(1,pionMeanPrev);
	yieldFit->FixParameter(2,pion->widthFit[yIndex]->Eval(mTm0));

      }//End if Electron has been killed

      //Fit the Yield Histogram
      yieldHisto->Fit(yieldFit,"RQ");
      fittingCanvas->Update();      

    }//End Loop Over mTm0Index

    cout <<"Finished Final Loop of Pion Minus Fitting Procedure" <<endl;
    
  }//End Loop Over yIndex


}

//--------------------------------------------------------------------
void FitPionPlus(){

  //Local Variables
  TH1D *yieldHisto = NULL;
  TF1 *yieldFit = NULL;

  for (Int_t yIndex=0; yIndex<nRapidityBins; yIndex++){

    //If the user has specified a rapidityIndex then only do that index
    if (rapidityIndex >= 0 && yIndex != rapidityIndex)
      continue;
    
    //Compute the Rapidity
    rapidity = GetRapidityRangeCenter(yIndex);
    
    cout <<"Working on Rapidity Index: " <<yIndex <<" Rapidity: " <<rapidity <<endl;

    //-----ROUND 1 - Parameterize the Mean of the Proton
    for (Int_t mTm0Index=2; mTm0Index<nmTm0Bins; mTm0Index++){

      //Make sure the histogram exists
      if (!tpcPionPlusHisto.at(yIndex).at(mTm0Index))
	continue;

      //Pass the histogram to a local pointer
      yieldHisto = tpcPionPlusHisto.at(yIndex).at(mTm0Index);
      yieldHisto->SetAxisRange(-1,2.5);

      //Make sure it has enough entires
      if (yieldHisto->GetEntries() < 100)
	continue;

      //Compute the kinematics for this bin
      ComputeBinKinematics(mTm0Index,rapidity,PION);
      
      //Make the Predictions for the particle species
      pionPredict = particleInfo->PredictZTPC(pTotal,PION,PION);
      kaonPredict = particleInfo->PredictZTPC(pTotal,PION,KAON);
      protonPredict = particleInfo->PredictZTPC(pTotal,PION,PROTON);
      electronPredict = particleInfo->PredictZTPC(pTotal,PION,ELECTRON);

      //Stop at this mTm0
      if (mTm0 > .5)
	break;

      //Find the Min Bin on either side of the proton peak
      Double_t minBelow = FindMinBinCenter(yieldHisto,protonPredict-.3,protonPredict);
      Double_t minAbove = FindMinBinCenter(yieldHisto,protonPredict,protonPredict+0.3);

      //Stop if the min or max is the proton prediction
      if (minBelow == protonPredict || minAbove == protonPredict)
	break;

      //Stop if the Overlapping with Pion
      if ( minBelow < pionPredict)
	break;
      /*
      if (fabs(minBelow-pionPredict) < 2*pion->width[yIndex]->Eval(mTm0))
	break;
      */
      //Draw the Histogram
      fittingCanvas->cd();
      yieldHisto->Draw("E");
      
      //Create the Fit and pass it to a local pointer
      if (!tpcPionPlusFit.at(yIndex).at(mTm0Index)){
	tpcPionPlusFit.at(yIndex).at(mTm0Index) = new TF1("","gaus(0)",minBelow,minAbove);
	yieldFit = tpcPionPlusFit.at(yIndex).at(mTm0Index);
      }

      //Set the Fit Parameters
      yieldFit->SetParameter(0,yieldHisto->GetBinContent(yieldHisto->FindBin(protonPredict)));
      yieldFit->SetParameter(1,protonPredict);
      yieldFit->SetParameter(2,.08);
      yieldFit->SetParLimits(1,protonPredict*.8,protonPredict*1.2);
      yieldFit->SetParLimits(2,.065,.17);
      
            
      //Fit the Histogram
      yieldHisto->Fit(yieldFit,"R");
            
      //Draw the Prediction Lines
      lineHeight = yieldHisto->GetBinContent(yieldHisto->FindBin(pionPredict));
      DrawPredictionLines(fittingCanvas);
      fittingCanvas->Update();

      //Add Points to Graph
      AddPointToParGraph(proton->mean[yIndex],yieldFit,1,mTm0Index);
      AddPointToParGraph(proton->width[yIndex],yieldFit,2,mTm0Index);

      //Draw the Parameter Graphs
      fitParCanvas->cd(8);
      proton->mean[yIndex]->Draw("APZ");
      fitParCanvas->cd(9);
      proton->width[yIndex]->Draw("APZ");
      fitParCanvas->Update();
      
      //gSystem->Sleep(2000);
      
    }//End Loop Over mTm0 ROUND 1

    //Fit the Proton Mean and Width
    proton->meanFit[yIndex] = new TF1(Form("proton_MeanFit_%02d",yIndex),
				      particleInfo,&ParticleInfo::ConfoundMeanFitTPC,.03,1.0,6);
    proton->meanFit[yIndex]->FixParameter(0,PION);
    proton->meanFit[yIndex]->FixParameter(1,PROTON);
    proton->meanFit[yIndex]->FixParameter(2,rapidity);
    proton->meanFit[yIndex]->SetParameter(3,0.0);
    proton->meanFit[yIndex]->SetParameter(4,1.0);
    proton->meanFit[yIndex]->SetParameter(5,1);
    proton->meanFit[yIndex]->SetParLimits(4,.5,1.5);
    proton->meanFit[yIndex]->SetParLimits(5,.6,1.2);

    proton->widthFit[yIndex] = new TF1(Form("proton_WidthFit_%02d",yIndex),
				       SwooshFunc,.05,1.0,4);
    proton->widthFit[yIndex]->SetParameters(.2,.5,.3,.1);
    proton->widthFit[yIndex]->SetParLimits(3,.0,3);

    Double_t max = proton->width[yIndex]->GetX()[proton->width[yIndex]->GetN()-1];
    proton->widthFit[yIndex]->SetParLimits(2,.1,max);

    fitParCanvas->cd(8);
    proton->mean[yIndex]->Fit(proton->meanFit[yIndex],"RQ");
    fitParCanvas->cd(9);
    proton->width[yIndex]->Fit(proton->widthFit[yIndex],"RQ");
    fitParCanvas->Update();

    //----- ROUND - Final Fitting
    for (Int_t mTm0Index=2; mTm0Index<nmTm0Bins; mTm0Index++){
      
      //Make sure the histogram exists
      if (!tpcPionPlusHisto.at(yIndex).at(mTm0Index))
	continue;
      
      //Pass the histogram to a local pointer
      yieldHisto = tpcPionPlusHisto.at(yIndex).at(mTm0Index);
      yieldHisto->SetAxisRange(-1,2.5);
      
      //Make sure it has enough entires
      if (yieldHisto->GetEntries() < 100)
	continue;
      
      //Compute the kinematics for this bin
      ComputeBinKinematics(mTm0Index,rapidity,PION);
      
      //Make the Predictions for the particle species
      pionPredict = particleInfo->PredictZTPC(pTotal,PION,PION);
      kaonPredict = particleInfo->PredictZTPC(pTotal,PION,KAON);
      protonPredict = particleInfo->PredictZTPC(pTotal,PION,PROTON);
      electronPredict = particleInfo->PredictZTPC(pTotal,PION,ELECTRON);
      
      //Stop at this mTm0
      if (mTm0 > .5)
	break;

      //Draw the Histogram
      fittingCanvas->cd();
      yieldHisto->Draw("E");

      //Delete the Previous fit
      if (tpcPionPlusFit.at(yIndex).at(mTm0Index))
	tpcPionPlusFit.at(yIndex).at(mTm0Index)->Delete();

      //Fit Range
      Double_t min = FindMinBinCenter(yieldHisto,pionPredict-.5,pionPredict);
      Double_t max = TMath::Max(protonPredict,electronPredict);
      max = FindMinBinCenter(yieldHisto,max,max+.5);

      //The final fit depends on the status of the electron
      if (!killElectron){
	
	tpcPionPlusFit.at(yIndex).at(mTm0Index) = new TF1("","gaus(0)+gaus(3)+gaus(6)",min,max);
	yieldFit = tpcPionPlusFit.at(yIndex).at(mTm0Index);
	
	yieldFit->SetParNames(Form("%s_{Amp.}",
				   particleInfo->GetParticleSymbol(PION,1).Data()),
			      "#mu_{#pi}","#sigma_{#pi}","e_{Amp.}",
			      "#mu_{e}","#sigma_{e}",
			      "p_{Amp.}","#mu_{p}","#sigma_{p}");
	
	//Electron Parameters
	yieldFit->FixParameter(3,electron->amp[yIndex]->Eval(mTm0));
	yieldFit->FixParameter(4,electron->meanFit[yIndex]->Eval(mTm0));
	yieldFit->FixParameter(5,electron->widthFit[yIndex]->Eval(mTm0));
	
	//Proton Parameters
	Double_t protonAmpSeed =
	  yieldHisto->GetBinContent(yieldHisto->FindBin(proton->meanFit[yIndex]->Eval(mTm0)));
	yieldFit->SetParameter(6,protonAmpSeed);
	yieldFit->FixParameter(7,proton->meanFit[yIndex]->Eval(mTm0));
	yieldFit->FixParameter(8,proton->widthFit[yIndex]->Eval(mTm0));

	yieldFit->SetParLimits(6,0,protonAmpSeed*1.05);
			       
      }//End if electron is not killed
      else {

	max = FindMinBinCenter(yieldHisto,pionPredict,pionPredict+.5);
	tpcPionPlusFit.at(yIndex).at(mTm0Index) = new TF1("","gaus(0)",min,max);
	yieldFit = tpcPionPlusFit.at(yIndex).at(mTm0Index);
	
	yieldFit->SetParNames(Form("%s_{Amp.}",particleInfo->GetParticleSymbol(PION,1).Data()),
			      "#mu_{#pi}","#sigma_{#pi}");
		
      }

      yieldFit->SetName(Form("TPCPionPlusFit_%02d_%02d",yIndex,mTm0Index));

      //Pion Parameters
      Double_t pionAmpSeed = yieldHisto->GetBinContent(yieldHisto->FindBin(pion->mean[yIndex]->Eval(mTm0)));
      yieldFit->SetParameter(0,pionAmpSeed);
      yieldFit->FixParameter(1,pion->mean[yIndex]->Eval(mTm0));
      yieldFit->FixParameter(2,pion->widthFit[yIndex]->Eval(mTm0));

      //Fit the Yield Histogram
      yieldHisto->Fit(yieldFit,"RQ");
      fittingCanvas->Update();
      
      //gSystem->Sleep(2000);
    }//End Loop Over mTm0Index
    
  }//End Loop Over yIndex
  

}

//--------------------------------------------------------------------
void DrawPredictionLines(TCanvas *canvas){

  canvas->cd();
  
  PredictionLine(pionLine,pionPredict,lineHeight)->Draw("SAME");
  PredictionLine(kaonLine,kaonPredict,lineHeight)->Draw("SAME");
  PredictionLine(protonLine,protonPredict,lineHeight)->Draw("SAME");
  PredictionLine(electronLine,electronPredict,lineHeight)->Draw("SAME");

}

//--------------------------------------------------------------------
void ComputeBinKinematics(Int_t mTm0Index, Double_t y, Int_t PID){

  mTm0 = GetmTm0RangeCenter(mTm0Index);
  mT   = mTm0 + particleInfo->GetParticleMass(PID);
  pZ   = ComputepZ(mT,y);
  pT   = ConvertmTm0ToPt(mTm0,particleInfo->GetParticleMass(PID));
  pTotal = ComputepTotal(pT,pZ);
  
}

//--------------------------------------------------------------------
Bool_t AddPointToSpectra(TH1D *yieldHisto, TF1 *yieldFit, TGraphErrors *rawSpectra,
			 Int_t yIndex, Int_t mTm0Index, Int_t PID, Int_t CHARGE, TString IMAGEDIR){
  
  
  //Adds a point to the spectra
  //returns false on fail, true on success

  //Make sure both the histogram and fit exist
  if (!yieldHisto || ! yieldFit)
    return false;

  //Draw the Final Version of the Fit
  fittingCanvas->Clear();
  fittingCanvas->cd();
  fittingCanvas->SetTopMargin(.05);
  fittingCanvas->SetRightMargin(.05);
  yieldHisto->SetMarkerStyle(kOpenCircle);
  yieldHisto->SetTitle("");
  yieldHisto->SetAxisRange(-2.0,3.0);
  yieldHisto->Draw("E");

  //Draw the Fit
  yieldFit->SetLineWidth(3);
  yieldFit->SetLineColor(kBlack);
  yieldFit->SetNpx(10000);
  yieldFit->Draw("SAME");  

  //Create a Label
  TPaveText label(.12,.8,.35,.92,"BL NDC");
  label.SetTextAlign(11);
  label.AddText(Form("Z(%s) Yield Extraction Fit",particleInfo->GetParticleSymbol(PID,CHARGE).Data()));
  label.AddText(Form("  %.2f #leq y_{%s} < %.2f | %.2f #leq m_{T}-m_{%s} < %.2f",
                     GetRapidityRangeLow(yIndex),particleInfo->GetParticleSymbol(PID,CHARGE).Data(),
                     GetRapidityRangeHigh(yIndex),GetmTm0RangeLow(mTm0Index),
                     particleInfo->GetParticleSymbol(PID,CHARGE).Data(),GetmTm0RangeHigh(mTm0Index)));
  label.GetLine(0)->SetTextSize(.03);
  label.GetLine(1)->SetTextSize(.02);
  label.SetFillColor(kWhite);
  label.SetBorderSize(0);
  label.Draw("SAME");

  //Make A Function for Each of the Particles
  TF1 pionGaus("pion","gaus(0)",-20,20);
  pionGaus.FixParameter(0,yieldFit->GetParameter(0));
  pionGaus.FixParameter(1,yieldFit->GetParameter(1));
  pionGaus.FixParameter(2,yieldFit->GetParameter(2));
  pionGaus.SetParError(0,yieldFit->GetParError(0));
  pionGaus.SetParError(1,yieldFit->GetParError(1));
  pionGaus.SetParError(2,yieldFit->GetParError(2));
  pionGaus.SetLineColor(particleInfo->GetParticleColor(PID));
  pionGaus.SetLineWidth(3);
  pionGaus.SetLineStyle(7);
  pionGaus.SetNpx(10000);
  pionGaus.Draw("SAME");
  
  TF1 electronGaus("electron","gaus(0)",-20,20);
  if (!killElectron){
    electronGaus.FixParameter(0,yieldFit->GetParameter(3));
    electronGaus.FixParameter(1,yieldFit->GetParameter(4));
    electronGaus.FixParameter(2,yieldFit->GetParameter(5));
    electronGaus.SetParError(0,yieldFit->GetParError(3));
    electronGaus.SetParError(1,yieldFit->GetParError(4));
    electronGaus.SetParError(2,yieldFit->GetParError(5));
    electronGaus.SetLineColor(particleInfo->GetParticleColor(ELECTRON));
    electronGaus.SetLineWidth(3);
    electronGaus.SetLineStyle(7);
    electronGaus.SetNpx(10000);
    electronGaus.Draw("SAME");
  }

  //Set NParameter Switch
  //Controls Whether protons will be drawn
  Bool_t parSwitch = false;
  if (yieldFit->GetNpar() > 6 && !killElectron)
    parSwitch = true;

  TF1 protonGaus("proton","gaus(0)",-20,20);
  if (parSwitch){
    protonGaus.FixParameter(0,yieldFit->GetParameter(6));
    protonGaus.FixParameter(1,yieldFit->GetParameter(7));
    protonGaus.FixParameter(2,yieldFit->GetParameter(8));
    protonGaus.SetParError(0,yieldFit->GetParError(6));
    protonGaus.SetParError(1,yieldFit->GetParError(7));
    protonGaus.SetParError(2,yieldFit->GetParError(8));
    protonGaus.SetLineColor(particleInfo->GetParticleColor(PROTON));
    protonGaus.SetLineWidth(3);
    protonGaus.SetLineStyle(7);
    protonGaus.SetNpx(10000);
    protonGaus.Draw("SAME");

  }

  //Get the Stats Box and Set its attributes
  TPaveStats *statsBox = (TPaveStats*)yieldHisto->FindObject("stats");
  if (PID == 1){
    statsBox->SetX1NDC(.7);
    statsBox->SetY1NDC(.50);
    statsBox->SetX2NDC(.97);
    statsBox->SetY2NDC(.93);
  }
  else {
    statsBox->SetX1NDC(.14);
    statsBox->SetY1NDC(.12);
    statsBox->SetX2NDC(.37);
    statsBox->SetY2NDC(.78);
  }
  gPad->Update();
  
  //Make A Legend
  TLegend leg;
  leg.SetOption("brNDC");
  if (parSwitch){
    leg.SetX1NDC(.65);
    leg.SetY1NDC(.89);
    leg.SetX2NDC(.95);
    leg.SetY2NDC(.94);
  }
  else {
    leg.SetX1NDC(.15);
    leg.SetY1NDC(.5);
    leg.SetX2NDC(.22);
    leg.SetY2NDC(.75);
  }
  leg.SetNColumns(parSwitch ? 5 : 1);
  leg.AddEntry(yieldFit,"Fit","L");
  leg.AddEntry("pion","#pi","L");
  if (!killElectron)
    leg.AddEntry("electron","e","L");
  if (parSwitch && !killElectron){
    leg.AddEntry("proton","p","L");
  }
  leg.SetFillColor(kWhite);
  leg.SetBorderSize(0);
  leg.Draw("SAME");

  fittingCanvas->Update();

  //Extract the Yield
  Double_t amp = pionGaus.GetParameter(0);
  Double_t ampError = pionGaus.GetParError(0);
  Double_t width = pionGaus.GetParameter(2);
  Double_t widthError = pionGaus.GetParError(2);
  Double_t pi = 3.14159;

  Double_t zBinWidth = (Double_t)yieldHisto->GetBinWidth(2);
  Double_t mTm0BinCenter = GetmTm0RangeCenter(mTm0Index);
  Double_t nEvents = (Double_t)nEventsHisto->GetBinContent(nEventsHisto->FindBin(centralityIndex));

  //Set the Normalization Factor
  Double_t normFactor = (1.0/(mTm0BinCenter+particleInfo->GetParticleMass(PID))) * (1.0/mTm0BinWidth) * (1.0/rapidityBinWidth) *
    (1.0/nEvents) * (1.0/(2.0*pi));

  //Compute the Raw Yield
  Double_t rawYield, rawYieldError;
  rawYield = ( (amp * width * sqrt(2.0*pi)) / zBinWidth ) * normFactor;
  rawYieldError = ( (amp * width * sqrt(pow(ampError/amp,2)+pow(widthError/width,2)) *
		     sqrt(2.0*pi)) / zBinWidth) * normFactor;

                                                                                                  
  //If this is a Pion Plus Spectra Stop when the pion and proton peak are close together
  Double_t deltaMean = fabs(pionGaus.GetParameter(1)-protonGaus.GetParameter(1));
  Double_t sumWidths = pionGaus.GetParameter(2)+protonGaus.GetParameter(2);
  if (parSwitch && deltaMean < 1.5*sumWidths){
    return false;
  }



  //Add Point to Spectrum
  rawSpectra->SetPoint(rawSpectra->GetN(),GetmTm0RangeCenter(mTm0Index),rawYield);
  rawSpectra->SetPointError(rawSpectra->GetN()-1,0,rawYieldError);

  //Draw the Spectra
  spectraCanvas->cd();
  rawSpectra->Draw("APZ");
  spectraCanvas->Update();

  return true;

}
