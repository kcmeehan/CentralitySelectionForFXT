#include "../inc/globalDefinitions.h"


void DrawAllRapiditySpectra(TString spectraFile, TString system, Double_t energy, Int_t speciesIndex, 
			    Int_t charge, Int_t centIndex, Double_t midY, Double_t startmTm0 = 0.0,
			    Bool_t corrected=false){

  Double_t scaleFactor = 2.5;
  Int_t yMax = 19;//nRapidityBins;
  
  std::vector <double> maxmTm0(nRapidityBins,0);
  //Hack for AuAu
  
  maxmTm0.at(19) = .5;
  maxmTm0.at(18) = .55;
  maxmTm0.at(17) = .8;
  maxmTm0.at(16) = .85;
  maxmTm0.at(15) = .9;
  maxmTm0.at(14) = .9;
  maxmTm0.at(13) = .925;
  

  //Hack for AlAu
  //maxmTm0.at(18) = .525;
  
  gSystem->Load("../bin/utilityFunctions_cxx.so");
  gSystem->Load("../bin/ParticleInfo_cxx.so");
  ParticleInfo *particleInfo = new ParticleInfo();

  //Create the Canvas and Frame
  TCanvas *canvas = new TCanvas("canvas","canvas",20,20,600,800);
  canvas->SetTopMargin(.05);
  canvas->SetRightMargin(.05);
  canvas->SetLeftMargin(.15);
  canvas->SetLogy();
  canvas->SetTicks(1,1);

  TH1F *frame = canvas->DrawFrame(0,.00001,1.35,20000000);
  frame->GetXaxis()->SetTitle(Form("m_{T}-m_{%s} (GeV/c^{2})",
				   particleInfo->GetParticleSymbol(speciesIndex).Data()));
  frame->GetYaxis()->SetTitle("#frac{1}{N_{Evt}}#times#frac{1}{2#pim_{T}}#times#frac{d^{2}N}{dm_{T}dy} (GeV/c^{2})^{2}");
  frame->GetYaxis()->SetTitleOffset(1.8);

  
  //Load the root file
  TFile *file = new TFile(spectraFile,"READ");

  //Create the Spectra Name
  TString type = "raw";
  TString Type = "Raw";
  TString name = "Uncorrected";
  if (corrected){
    type = "corrected";
    Type = "Corrected";
    name = "Corrected";
  }

  TString speciesName = particleInfo->GetParticleName(speciesIndex,charge);
  TString spectraName; = TString::Format("%sSpectra_%s_Cent%02d_yIndex_%02d",
					type.Data(),speciesName.Data(),centIndex);
  Int_t midRapidityIndex = GetRapidityIndex(midY);
  cout <<"Mid Rapidity Index: " <<midRapidityIndex <<endl;


  //Create an Array for the spectra and get the spectra from the file
  TGraphErrors *spectra[nRapidityBins];
  TGraphErrors *scaledSpectra[nRapidityBins];
  TPaveText *label[nRapidityBins];

  for (Int_t yIndex=0; yIndex<yMax; yIndex++){
    
    spectra[yIndex] = NULL;
    scaledSpectra[yIndex] = NULL;
     
    spectraName = TString::Format("%sSpectra_%s_Cent%02d_yIndex_%02d",
				  type.Data(),speciesName.Data(),centIndex,yIndex);
    
    spectra[yIndex] = file->Get(Form("%sSpectra_%s/%s",
				     Type.Data(),speciesName.Data(),
				     spectraName.Data()));
    

    //Skip if no spectrum was found for this rapidity bin
    if (spectra[yIndex] == NULL)
      continue;

    //Remove low mTm0 Points
    TGraphChop(spectra[yIndex],.07);
    
    //Remove high mTm0 Point
    if (maxmTm0.at(yIndex) != 0)
      TGraphChop(spectra[yIndex],maxmTm0.at(yIndex),false);

    
    //Scale the Spectra
    scaledSpectra[yIndex] = TGraphScale(spectra[yIndex],pow(scaleFactor,yIndex-midRapidityIndex));

    
    //Create the Label
    Double_t yLocation = scaledSpectra[yIndex]->GetY()[scaledSpectra[yIndex]->GetN()-1];
    Double_t xLocation = scaledSpectra[yIndex]->GetX()[scaledSpectra[yIndex]->GetN()-1]+.05;
    label[yIndex] = new TPaveText(xLocation,yLocation,xLocation+.2,yLocation,"NB");
    label[yIndex]->SetTextSize(0.028);
    label[yIndex]->AddText(Form("y = %.1f",GetRapidityRangeCenter(yIndex)));
    
    cout <<GetRapidityRangeCenter(yIndex) <<endl;

    //Set the Mid Rapidity Spectrum to Red
    if (yIndex == midRapidityIndex){
      scaledSpectra[yIndex]->SetMarkerColor(kRed);
      label[yIndex]->SetTextColor(kRed);
    }
    
    scaledSpectra[yIndex]->Draw("PZ");
    label[yIndex]->Draw("SAME");


    
  }//End Loop Over Spectra


  //Create the Title
  TPaveText *title = new TPaveText(.56,.81,.91,.91,"NBNDCBR");
  title->SetFillColor(kWhite);
  title->SetBorderSize(0);
  title->SetTextSize(.04);
  title->SetTextAlign(32);
  title->AddText(Form("%s Spectra %s",
  		      particleInfo->GetParticleSymbol(speciesIndex,charge).Data(),
		      system.Data()));
  title->AddText("Top 10% Central");
  title->AddText(Form("#sqrt{s_{NN}} = %.03g GeV",
  		      energy));
  title->GetLine(1)->SetTextSize(.03);
  title->GetLine(2)->SetTextSize(.03);
  title->Draw("SAME");

  TMarker *midYMarker = new TMarker(0,0,kFullCircle);
  midYMarker->SetMarkerColor(kRed);
  
  TMarker *notMidYMarker = new TMarker(0,0,kFullCircle);
  notMidYMarker->SetMarkerColor(kBlack);
  
  TLegend *leg = new TLegend(.16,.13,.47,.20);
  leg->SetLineColor(kWhite);
  leg->SetBorderSize(0);
  leg->SetFillColor(kWhite);
  leg->SetTextSize(.035);
  leg->AddEntry(midYMarker,"Mid Rapidity (Unscaled)","P");
  leg->AddEntry(notMidYMarker,Form("x %g^{#pm n}",scaleFactor),"P");
  leg->Draw("SAME");

}

