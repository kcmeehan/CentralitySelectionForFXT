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

using namespace std;

void DrawVertexQA(TString inputFile, Bool_t vertexCuts){

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
  TCanvas *cVertexNoCuts = new TCanvas("cVertexNoCuts","cVertexNoCuts",1200,1200);
  cVertexNoCuts->Divide(2,2);
  cVertexNoCuts->cd(1);
  gPad->SetLogy();
  gStyle->SetOptStat(11);
  FullVzHistoNoCuts->GetXaxis()->SetTitle("V_{z} (cm)");
  FullVzHistoNoCuts->GetYaxis()->SetTitle("# of Vertices");
  FullVzHistoNoCuts->GetYaxis()->SetTitleOffset(1.3);
  FullVzHistoNoCuts->Draw();
  cVertexNoCuts->cd(2);
  gPad->SetLogy();
  VzHistoNoCuts->SetTitle("V_{z} Zoomed In");
  VzHistoNoCuts->GetXaxis()->SetTitle("V_{z} (cm)");
  VzHistoNoCuts->GetYaxis()->SetTitle("# of Vertices");
  VzHistoNoCuts->GetYaxis()->SetTitleOffset(1.3);
  VzHistoNoCuts->Draw();
  cVertexNoCuts->cd(3);
  gPad->SetLogz();
  VxVyHistoNoCuts->GetXaxis()->SetTitle("V_{x} (cm)");
  VxVyHistoNoCuts->GetYaxis()->SetTitle("V_{y} (cm)");
  VxVyHistoNoCuts->Draw("colz");
  cVertexNoCuts->cd(4);
  gPad->SetLogz();
  VyVzHistoNoCuts->GetXaxis()->SetTitle("V_{z} (cm)");
  VyVzHistoNoCuts->GetYaxis()->SetTitle("V_{y} (cm)");
  VyVzHistoNoCuts->Draw("colz");

  if(!vertexCuts){
    TCanvas *cMult= new TCanvas("cMult","cMult",1200,600);
    cMult->Divide(2,1);
    cMult->cd(1);
    gPad->SetLogy();
    multHistoNoCuts->SetStats(0);
    multHistoNoCuts->GetXaxis()->SetTitle("Charged Particle Multiplicity");
    multHistoNoCuts->GetYaxis()->SetTitle("Vertices");
    multHistoNoCuts->GetYaxis()->SetTitleOffset(1.3);
    multHistoNoCuts->Draw();
    cMult->cd(2);
    gPad->SetLogz();
    TOFvsPiMultHistNoCuts->SetStats(0);
    TOFvsPiMultHistNoCuts->GetXaxis()->SetTitle("Pion Multiplicity");
    TOFvsPiMultHistNoCuts->GetYaxis()->SetTitle("# of TOF Matched Tracks");
    TOFvsPiMultHistNoCuts->GetYaxis()->SetTitleOffset(1.3);
    TOFvsPiMultHistNoCuts->Draw("colz"); 
  }

  TCanvas *tofmatchNoCuts = new TCanvas("tofmatchNoCuts","tofmatchNoCuts",600,600);
  ntofMatchHistNoCuts->SetStats(0);
  ntofMatchHistNoCuts->GetXaxis()->SetTitle("# of TOF Matched Tracks");
  ntofMatchHistNoCuts->GetYaxis()->SetTitle("Vertices");
  ntofMatchHistNoCuts->Draw();

  if(vertexCuts){
    TCanvas *cVertex = new TCanvas("cVertex","cVertex",1800,600);
    cVertex->Divide(3,1);
    cVertex->cd(1);
    gPad->SetLogy();
    VzHisto->GetXaxis()->SetTitle("V_{z} (cm)");
    VzHisto->GetYaxis()->SetTitle("# of Vertices");
    VzHisto->GetYaxis()->SetTitleOffset(1.3);
    VzHisto->Draw();
    cVertex->cd(2);
    gPad->SetLogz();
    VxVyHisto->GetXaxis()->SetTitle("V_{x} (cm)");
    VxVyHisto->GetYaxis()->SetTitle("V_{y} (cm)");
    VxVyHisto->Draw("colz");
    cVertex->cd(3);
    gPad->SetLogz();
    VyVzHisto->GetXaxis()->SetTitle("V_{z} (cm)");
    VyVzHisto->GetYaxis()->SetTitle("V_{y} (cm)");
    VyVzHisto->Draw("colz");
  
    TCanvas *tofmatch = new TCanvas("tofmatch","tofmatch",600,600);
    ntofMatchHist->SetStats(0);
    ntofMatchHist->GetXaxis()->SetTitle("# of TOF Matched Tracks");
    ntofMatchHist->GetYaxis()->SetTitle("Vertices");
    ntofMatchHist->Draw();

    TCanvas *cTOFvsPiMult = new TCanvas("cTOFvsPiMult","cTOFvsPiMult",1200,600);
    cTOFvsPiMult->Divide(2,1);
    cTOFvsPiMult->cd(1);
    gPad->SetLogz();
    TOFvsPiMultHistNoCuts->SetStats(0);
    TOFvsPiMultHistNoCuts->GetXaxis()->SetTitle("Pion Multiplicity");
    TOFvsPiMultHistNoCuts->GetYaxis()->SetTitle("# of TOF Matched Tracks");
    TOFvsPiMultHistNoCuts->GetYaxis()->SetTitleOffset(1.3);
    TOFvsPiMultHistNoCuts->Draw("colz");
    cTOFvsPiMult->cd(2);
    gPad->SetLogz();
    TOFvsPiMultHist->SetStats(0);
    TOFvsPiMultHist->GetXaxis()->SetTitle("Pion Multiplicity");
    TOFvsPiMultHist->GetYaxis()->SetTitle("# of TOF Matched Tracks");
    TOFvsPiMultHist->GetYaxis()->SetTitleOffset(1.3);
    TOFvsPiMultHist->Draw("colz");

    TCanvas *cMult = new TCanvas("cMult","cMult",1800,600);
    cMult->Divide(3,1);
    cMult->cd(1);
    gPad->SetLogy();
    multHistoNoCuts->SetStats(0);
    multHistoNoCuts->GetXaxis()->SetTitle("Charged Particle Multiplicity");
    multHistoNoCuts->GetYaxis()->SetTitle("Vertices");
    multHistoNoCuts->GetYaxis()->SetTitleOffset(1.3);
    multHistoNoCuts->Draw();
    cMult->cd(2);
    gPad->SetLogy();
    multHisto->SetStats(0);
    multHisto->GetXaxis()->SetTitle("Charged Particle Multiplicity");
    multHisto->GetYaxis()->SetTitle("Vertices");
    multHisto->GetYaxis()->SetTitleOffset(1.3);
    multHisto->Draw();
    cMult->cd(3);
    gPad->SetLogy();
    piMult->SetStats(0);
    piMult->GetXaxis()->SetTitle("Pion Multiplicity");
    piMult->GetYaxis()->SetTitle("Vertices");
    piMult->GetYaxis()->SetTitleOffset(1.3);
    piMult->Draw();
  }
 
}




