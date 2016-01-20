//Draws event QA plots
#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include <TMath.h>
#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TCanvas.h>
#include <TClonesArray.h>
#include <TSystem.h>
#include <TLatex.h>
#include <THistPainter.h>
#include <TAttLine.h>
#include <TLegend.h>
#include <TStyle.h>
#include "../src/submodules/datacollectorreaderlibs/EventInfo/EventInfo.h"

using namespace std;

void DrawEventQA(TString inputFile, Bool_t eventCuts){

//make plots pretty
  gStyle->SetPalette(1,0);
  //gStyle->SetOptStat(0);
  gStyle->SetOptFit(1);
  gStyle->SetOptDate(0);
  gStyle->SetPadGridX(0);
  gStyle->SetPadGridY(0);
  
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;
  
  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);

//obtain plots to draw
  TFile *file     = new TFile(inputFile,"READ");
  TTree *tree     = (TTree *)file->Get("DataTree");
 
//draw histograms
  TCanvas *cTrigIdsNoCuts = new TCanvas("cTriggerIdsNoCuts","cTriggerIdsNoCuts",600,600);
  htrigNoCuts->GetXaxis()->SetTitle("Offline Trigger Id");
  htrigNoCuts->GetYaxis()->SetTitle("Triggered Events");
  htrigNoCuts->GetYaxis()->SetTitleOffset(1.3);
  htrigNoCuts->Draw();
 
  TCanvas *cTriggerPlotsNoCuts = new TCanvas("cTriggerPlotsNoCuts","cTriggerPlotsNoCuts",1200,600);
  cTriggerPlotsNoCuts->Divide(2,1);
  cTriggerPlotsNoCuts->cd(1);
  gPad->SetLogy();
  tofMultHistNoCuts->SetStats(0);
  tofMultHistNoCuts->Draw();
  tofMultHistNoCuts->GetXaxis()->SetTitle("TOF Multiplicity");
  tofMultHistNoCuts->GetYaxis()->SetTitle("Number of Triggers");
  tofMultHistNoCuts->GetYaxis()->SetTitleOffset(1.3);
  tofMultHistNoCuts->Draw();
  cTriggerPlotsNoCuts->cd(2);
  gPad->SetLogy();
  hnPrimaryVerticesNoCuts->SetStats(0);
  hnPrimaryVerticesNoCuts->GetXaxis()->SetTitle("# of Vertices");
  hnPrimaryVerticesNoCuts->GetYaxis()->SetTitle("Events");
  hnPrimaryVerticesNoCuts->GetYaxis()->SetTitleOffset(1.3);
  hnPrimaryVerticesNoCuts->Draw();
 
  if(eventCuts){
    
    TCanvas *cTrigIds = new TCanvas("cTriggerIds","cTriggerIds",600,600);
    htrig->GetXaxis()->SetTitle("Offline Trigger Id");
    htrig->GetYaxis()->SetTitle("Triggered Events");
    htrig->GetYaxis()->SetTitleOffset(1.3);
    htrig->Draw();
  
    TCanvas *cTriggerPlots = new TCanvas("cTriggerPlots","cTriggerPlots",1200,600);
    cTriggerPlots->Divide(2,1);
    cTriggerPlots->cd(1);
    gPad->SetLogy();
    tofMultHist->SetStats(0);
    tofMultHist->Draw();
    tofMultHist->GetXaxis()->SetTitle("TOF Multiplicity");
    tofMultHist->GetYaxis()->SetTitle("Number of Triggers");
    tofMultHist->GetYaxis()->SetTitleOffset(1.3);
    tofMultHist->Draw();
    cTriggerPlots->cd(2);
    gPad->SetLogy();
    hnPrimaryVertices->SetStats(0);
    hnPrimaryVertices->GetXaxis()->SetTitle("# of Vertices");
    hnPrimaryVertices->GetYaxis()->SetTitle("Events");
    hnPrimaryVertices->GetYaxis()->SetTitleOffset(1.3);
    hnPrimaryVertices->Draw();
 
  }

}


