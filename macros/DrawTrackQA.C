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

void DrawTrackQA(TString inputFile, Bool_t extended, Bool_t rapBinned){

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
 
//draw histograms -- BASIC QA plots
  TCanvas *cEtaPhi = new TCanvas("cEtaPhi","cEtaPhi",1200,600);
	cEtaPhi->Divide(2,1);
	cEtaPhi->cd(1);
	gPad->SetLogz();
	etaphiHist->SetStats(0);
	etaphiHist->GetXaxis()->SetTitle("#eta");
	etaphiHist->GetYaxis()->SetTitle("#phi");
	etaphiHist->Draw("colz"); 
	TLine *lmid = new TLine(yMid,-4,yMid,4);
	lmid->SetLineWidth(5);
	lmid->Draw("same");
	cEtaPhi->cd(2);
	gPad->SetLogz();
	etaphiHistTOF->SetStats(0);
	etaphiHistTOF->GetXaxis()->SetTitle("#eta");
	etaphiHistTOF->GetYaxis()->SetTitle("#phi");
	etaphiHistTOF->Draw("colz");  

	TCanvas *cPID = new TCanvas("cPID","",1200,1200);
	cPID->Divide(2,2);
	cPID->cd(1);
	gPad->SetLogz();
	dEdxHist->SetStats(0);
	dEdxHist->GetXaxis()->SetTitle("|p|/q (GeV/c)");
	dEdxHist->GetYaxis()->SetTitle("dE/dx (keV/cm)");
	dEdxHist->GetYaxis()->SetTitleOffset(1.1);
	dEdxHist->Draw("colz");
	cPID->cd(2);
	gPad->SetLogz();
	dEdxHistZoom->SetStats(0);
	dEdxHistZoom->SetTitle("Energy Loss in TPC Zoomed In");
	dEdxHistZoom->GetXaxis()->SetTitle("|p|/q (GeV/c)");
	dEdxHistZoom->GetYaxis()->SetTitle("dE/dx (keV/cm)");
	dEdxHistZoom->Draw("colz");
	cPID->cd(3);
	gPad->SetLogz();
	tofBeta->SetStats(0);
	tofBeta->GetXaxis()->SetTitle("|p|/q (GeV/c)");
	tofBeta->GetYaxis()->SetTitle("1/#beta");
	tofBeta->Draw("colz");
	cPID->cd(4);
	gPad->SetLogz();
	tofBetaZoom->SetStats(0);
	tofBetaZoom->SetTitle("TOF 1/#beta Zoomed In");
	tofBetaZoom->GetXaxis()->SetTitle("|p|/q (GeV/c)");
	tofBetaZoom->GetYaxis()->SetTitle("1/#beta");
	tofBetaZoom->Draw("colz");

  TCanvas *cAccept = new TCanvas("cAccept","",1200,600);
	cAccept->Divide(2,1); 
	cAccept->cd(1);
	gPad->SetLogz();
	pimAccept->SetStats(0);
	pimAccept->GetXaxis()->SetTitle("y");
	pimAccept->GetYaxis()->SetTitle("p_{T} (GeV/c)");
	pimAccept->GetYaxis()->SetTitleOffset(1.3);
	pimAccept->Draw("colz");
	TLine *lmid2 = new TLine(yMid,0,yMid,2.5);
	lmid2->SetLineWidth(5);
	lmid2->Draw("same");
	cAccept->cd(2);
	gPad->SetLogz();
	proAccept->SetStats(0);
	proAccept->GetXaxis()->SetTitle("y");
	proAccept->GetYaxis()->SetTitle("p_{T} (GeV/c)");
	proAccept->GetYaxis()->SetTitleOffset(1.3);
	proAccept->Draw("colz");
	TLine *lmid3 = new TLine(yMid,0,yMid,4);
	lmid3->SetLineWidth(5);
	lmid3->Draw("same");

  TCanvas *cdca= new TCanvas("cdca","",1200,600);
	cdca->Divide(2,1);
	cdca->cd(1);
	gPad->SetLogy();
	hdca->SetStats(0);
	hdca->GetXaxis()->SetTitle("global dca magnitude (cm)");
	hdca->GetYaxis()->SetTitle("Primary Tracks");
	hdca->GetYaxis()->SetTitleOffset(1.3);
	hdca->Draw();
	cdca->cd(2);
	gPad->SetLogz();
	hdcaEta->SetStats(0);
	hdcaEta->GetXaxis()->SetTitle("#eta");
	hdcaEta->GetYaxis()->SetTitle("Global DCA Magnitude");
	hdcaEta->GetYaxis()->SetTitleOffset(1.3);
	hdcaEta->Draw("colz");

	TCanvas *cHits = new TCanvas("cHits","",1200,1200);
	cHits->Divide(2,2);
	cHits->cd(1);
	hnHits->SetStats(0);
	hnHits->GetXaxis()->SetTitle("nHits");
	hnHits->GetYaxis()->SetTitle("# of Tracks");
	hnHits->GetYaxis()->SetTitleOffset(1.5);
	hnHits->Draw();
	cHits->cd(2);
	hnHitsFit->SetStats(0);
	hnHitsFit->GetXaxis()->SetTitle("nHitsFit");
	hnHitsFit->GetYaxis()->SetTitle("# of Tracks");
	hnHitsFit->GetYaxis()->SetTitleOffset(1.5);
	hnHitsFit->Draw();
	cHits->cd(3);
	hnHitsPoss->SetStats(0);
	hnHitsPoss->GetXaxis()->SetTitle("nHitsPoss");
	hnHitsPoss->GetYaxis()->SetTitle("# of Tracks");
	hnHitsPoss->GetYaxis()->SetTitleOffset(1.5);
	hnHitsPoss->Draw();
	cHits->cd(4);
	hnHitsRat->SetStats(0);
	hnHitsRat->GetXaxis()->SetTitle("nHitsFit/nHitsPoss");
	hnHitsRat->GetYaxis()->SetTitle("# of Tracks");
	hnHitsRat->GetYaxis()->SetTitleOffset(1.5);
	hnHitsRat->Draw();
	
	TCanvas *cHitdedx = new TCanvas("cHitdedx","",600,600);
	hnHitsdEdx->SetStats(0);
	hnHitsdEdx->GetXaxis()->SetTitle("nHitsdEdx");
	hnHitsdEdx->GetYaxis()->SetTitle("# of Tracks");
	hnHitsdEdx->GetYaxis()->SetTitleOffset(1.7);
	hnHitsdEdx->Draw();


  if (extended) {

    TCanvas *cHitPos = new TCanvas("cHitPos","",1200,1200);
		cHitPos->Divide(2,2); 
		cHitPos->cd(1);
		gPad->SetLogz();
		firstXYeast->SetStats(0);
		firstXYeast->GetXaxis()->SetTitle("X (cm)");
		firstXYeast->GetYaxis()->SetTitle("Y (cm)");
		firstXYeast->GetYaxis()->SetTitleOffset(1.3);
		firstXYeast->Draw("colz");
		cHitPos->cd(2);
		gPad->SetLogz();
		firstXYwest->SetStats(0);
		firstXYwest->GetXaxis()->SetTitle("X (cm)");
		firstXYwest->GetYaxis()->SetTitle("Y (cm)");
		firstXYwest->GetYaxis()->SetTitleOffset(1.3);
		firstXYwest->Draw("colz");
		cHitPos->cd(3);
		gPad->SetLogz();
		lastXYeast->SetStats(0);
		lastXYeast->GetXaxis()->SetTitle("X (cm)");
		lastXYeast->GetYaxis()->SetTitle("Y (cm)");
		lastXYeast->GetYaxis()->SetTitleOffset(1.3);
		lastXYeast->Draw("colz");
		cHitPos->cd(4);
		gPad->SetLogz();
		lastXYwest->SetStats(0);
		lastXYwest->GetXaxis()->SetTitle("X (cm)");
		lastXYwest->GetYaxis()->SetTitle("Y (cm)");
		lastXYwest->GetYaxis()->SetTitleOffset(1.3);
		lastXYwest->Draw("colz");

    TCanvas *cTrackEnd= new TCanvas("cTrackEnd","",1200,1200);
		cTrackEnd->Divide(2,2);
		cTrackEnd->cd(1);
		firstZ->SetStats(0);
		firstZ->GetXaxis()->SetTitle("Z Position of First Hit of Track (cm)");
		firstZ->GetYaxis()->SetTitle("# of Tracks");
		firstZ->GetYaxis()->SetTitleOffset(1.3);
		firstZ->Draw();
		cTrackEnd->cd(2);
		lastZ->SetStats(0);
		lastZ->GetXaxis()->SetTitle("Z Position of Last Hit of Track (cm)");
		lastZ->GetYaxis()->SetTitle("# of Tracks");
		lastZ->GetYaxis()->SetTitleOffset(1.3);
		lastZ->Draw();
		cTrackEnd->cd(3);
		hTrackLength->SetStats(0);
		hTrackLength->GetXaxis()->SetTitle("Track Length (cm)");
		hTrackLength->GetYaxis()->SetTitle("# of Tracks");
		hTrackLength->GetYaxis()->SetTitleOffset(1.3);
		hTrackLength->Draw();
		cTrackEnd->cd(4);
		gPad->SetLogz();
		lengthVsEta->SetStats(0);
		lengthVsEta->GetXaxis()->SetTitle("#eta");
		lengthVsEta->GetYaxis()->SetTitle("Track Length (cm)");
		lengthVsEta->GetYaxis()->SetTitleOffset(1.3);
		lengthVsEta->Draw("colz");

		TCanvas *cRatEta = new TCanvas("cRatEta","",600,600);
		gPad->SetLogz(); 
		nRatVsEta->SetStats(0);
		nRatVsEta->GetXaxis()->SetTitle("#eta");
		nRatVsEta->GetYaxis()->SetTitle("nHitsFit/nHitsPoss");
		nRatVsEta->Draw("colz");

    TCanvas *cptBins = new TCanvas("cptBins","",1200,1200); 
		cptBins->Divide(2,2,0.001,0.001);
		cptBins->cd(1);
		gPad->SetLogy();
		pTbin1->GetXaxis()->SetTitle("p_{T} (GeV/c)");
		pTbin1->GetYaxis()->SetTitle("1/p_{t}*dN/dp_{T}");
		pTbin1->GetYaxis()->SetTitleOffset(1.3);
		pTbin1->Draw();
		cptBins->cd(2);
		gPad->SetLogy();
		pTbin2->GetXaxis()->SetTitle("p_{T} (GeV/c)");
		pTbin2->GetYaxis()->SetTitle("1/p_{T}*dN/dp_{T}");
		pTbin2->GetYaxis()->SetTitleOffset(1.3);
		pTbin2->Draw();
		cptBins->cd(3);
		gPad->SetLogy();
		pTbin3->GetXaxis()->SetTitle("p_{T} (GeV/c)");
		pTbin3->GetYaxis()->SetTitle("1/p_{T}*dN/dp_{T}");
		pTbin3->GetYaxis()->SetTitleOffset(1.3);
		pTbin3->Draw();
		cptBins->cd(4);
		gPad->SetLogy();
		pTbin4->GetXaxis()->SetTitle("p_{T} (GeV/c)");
		pTbin4->GetYaxis()->SetTitle("1/p_{T}*dN/dp_{T}");
		pTbin4->GetYaxis()->SetTitleOffset(1.3);
		pTbin4->Draw();
		
		TCanvas *cmtBins = new TCanvas("cmtBins","",1200,1200); 
		cmtBins->Divide(2,2,0.001,0.001);
		cmtBins->cd(1);
		gPad->SetLogy();
		mTbin1->GetXaxis()->SetTitle("m_{T} (GeV/c)");
		mTbin1->GetYaxis()->SetTitle("1/m_{t}*dN/dm_{T}");
		mTbin1->GetYaxis()->SetTitleOffset(1.3);
		mTbin1->Draw();
		cmtBins->cd(2);
		gPad->SetLogy();
		mTbin2->GetXaxis()->SetTitle("m_{T} (GeV/c)");
		mTbin2->GetYaxis()->SetTitle("1/m_{T}*dN/dm_{T}");
		mTbin2->GetYaxis()->SetTitleOffset(1.3);
		mTbin2->Draw();
		cmtBins->cd(3);
		gPad->SetLogy();
		mTbin3->GetXaxis()->SetTitle("m_{T} (GeV/c)");
		mTbin3->GetYaxis()->SetTitle("1/m_{T}*dN/dm_{T}");
		mTbin3->GetYaxis()->SetTitleOffset(1.3);
		mTbin3->Draw();
		cmtBins->cd(4);
    gPad->SetLogy();
		mTbin4->GetXaxis()->SetTitle("m_{T} (GeV/c)");
		mTbin4->GetYaxis()->SetTitle("1/m_{T}*dN/dm_{T}");
		mTbin4->GetYaxis()->SetTitleOffset(1.3);
		mTbin4->Draw();
    
	}

	if (rapBinned) {
    TCanvas *cTPCetaSlices = new TCanvas("cTPCetaSlices","cTPCetaSlices",1500,900);
		cTPCetaSlices->Divide(3,2);
		cTPCetaSlices->cd(1);
		gPad->SetLogz();
		hdEdx1->SetStats(0);
		hdEdx1->GetXaxis()->SetTitle("|p|/q (GeV/c)");
		hdEdx1->GetYaxis()->SetTitle("dE/dx (keV/cm)");
		hdEdx1->Draw("colz");
		cTPCetaSlices->cd(2);
		gPad->SetLogz();
		hdEdx2->SetStats(0);
		hdEdx2->GetXaxis()->SetTitle("|p|/q (GeV/c)");
		hdEdx2->GetYaxis()->SetTitle("dE/dx (keV/cm)");
		hdEdx2->Draw("colz");
		cTPCetaSlices->cd(3);
		gPad->SetLogz();
		hdEdx3->SetStats(0);
		hdEdx3->GetXaxis()->SetTitle("|p|/q (GeV/c)");
		hdEdx3->GetYaxis()->SetTitle("dE/dx (keV/cm)");
		hdEdx3->Draw("colz");
		cTPCetaSlices->cd(4);
		gPad->SetLogz();
		hdEdx4->SetStats(0);
		hdEdx4->GetXaxis()->SetTitle("|p|/q (GeV/c)");
		hdEdx4->GetYaxis()->SetTitle("dE/dx (keV/cm)");
		hdEdx4->Draw("colz");
		cTPCetaSlices->cd(5);
		gPad->SetLogz();
		hdEdx5->SetStats(0);
		hdEdx5->GetXaxis()->SetTitle("|p|/q (GeV/c)");
		hdEdx5->GetYaxis()->SetTitle("dE/dx (keV/cm)");
		hdEdx5->Draw("colz");
		cTPCetaSlices->cd(6);
		gPad->SetLogz();
		hdEdx6->SetStats(0);
		hdEdx6->GetXaxis()->SetTitle("|p|/q (GeV/c)");
		hdEdx6->GetYaxis()->SetTitle("dE/dx (keV/cm)");
		hdEdx6->Draw("colz");

    TCanvas *cRatetaSlices = new TCanvas("cRatetaSlices","",1500,900);
		cRatetaSlices->Divide(3,2);
		cRatetaSlices->cd(1);
		hnHitsRat1->SetStats(0);
		hnHitsRat1->GetXaxis()->SetTitle("nHitsFit/nHitsPoss");
		hnHitsRat1->GetYaxis()->SetTitle("# of Tracks");
		hnHitsRat1->GetYaxis()->SetTitleOffset(1.7);
		hnHitsRat1->Draw();
		cRatetaSlices->cd(2);
		hnHitsRat2->SetStats(0);
		hnHitsRat2->GetXaxis()->SetTitle("nHitsFit/nHitsPoss");
		hnHitsRat2->GetYaxis()->SetTitle("# of Tracks");
		hnHitsRat2->GetYaxis()->SetTitleOffset(1.7);
		hnHitsRat2->Draw();
		cRatetaSlices->cd(3);
		hnHitsRat3->SetStats(0);
		hnHitsRat3->GetXaxis()->SetTitle("nHitsFit/nHitsPoss");
		hnHitsRat3->GetYaxis()->SetTitle("# of Tracks");
		hnHitsRat3->GetYaxis()->SetTitleOffset(1.7);
		hnHitsRat3->Draw();
		cRatetaSlices->cd(4);
		hnHitsRat4->SetStats(0);
		hnHitsRat4->GetXaxis()->SetTitle("nHitsFit/nHitsPoss");
		hnHitsRat4->GetYaxis()->SetTitle("# of Tracks");
		hnHitsRat4->GetYaxis()->SetTitleOffset(1.7);
		hnHitsRat4->Draw();
		cRatetaSlices->cd(5);
		hnHitsRat5->SetStats(0);
		hnHitsRat5->GetXaxis()->SetTitle("nHitsFit/nHitsPoss");
		hnHitsRat5->GetYaxis()->SetTitle("# of Tracks");
		hnHitsRat5->GetYaxis()->SetTitleOffset(1.7);
		hnHitsRat5->Draw();
		cRatetaSlices->cd(6);
		hnHitsRat6->SetStats(0);
		hnHitsRat6->GetXaxis()->SetTitle("nHitsFit/nHitsPoss");
		hnHitsRat6->GetYaxis()->SetTitle("# of Tracks");
		hnHitsRat6->GetYaxis()->SetTitleOffset(1.7);
		hnHitsRat6->Draw();

    TCanvas *cFitetaSlices = new TCanvas("cFitetaSlices","",1500,900);
		cFitetaSlices->Divide(3,2);
		cFitetaSlices->cd(1);
		hnHitsFit1->SetStats(0);
		hnHitsFit1->GetXaxis()->SetTitle("nHitsFit");
		hnHitsFit1->GetYaxis()->SetTitle("# of Tracks");
		hnHitsFit1->GetYaxis()->SetTitleOffset(1.7);
		hnHitsFit1->Draw();
		cFitetaSlices->cd(2);
		hnHitsFit2->SetStats(0);
		hnHitsFit2->GetXaxis()->SetTitle("nHitsFit");
		hnHitsFit2->GetYaxis()->SetTitle("# of Tracks");
		hnHitsFit2->GetYaxis()->SetTitleOffset(1.7);
		hnHitsFit2->Draw();
		cFitetaSlices->cd(3);
		hnHitsFit3->SetStats(0);
		hnHitsFit3->GetXaxis()->SetTitle("nHitsFit");
		hnHitsFit3->GetYaxis()->SetTitle("# of Tracks");
		hnHitsFit3->GetYaxis()->SetTitleOffset(1.7);
		hnHitsFit3->Draw();
		cFitetaSlices->cd(4);
		hnHitsFit4->SetStats(0);
		hnHitsFit4->GetXaxis()->SetTitle("nHitsFit");
		hnHitsFit4->GetYaxis()->SetTitle("# of Tracks");
		hnHitsFit4->GetYaxis()->SetTitleOffset(1.7);
		hnHitsFit4->Draw();
		cFitetaSlices->cd(5);
		hnHitsFit5->SetStats(0);
		hnHitsFit5->GetXaxis()->SetTitle("nHitsFit");
		hnHitsFit5->GetYaxis()->SetTitle("# of Tracks");
		hnHitsFit5->GetYaxis()->SetTitleOffset(1.7);
		hnHitsFit5->Draw();
		cFitetaSlices->cd(6);
		hnHitsFit6->SetStats(0);
		hnHitsFit6->GetXaxis()->SetTitle("nHitsFit");
		hnHitsFit6->GetYaxis()->SetTitle("# of Tracks");
		hnHitsFit6->GetYaxis()->SetTitleOffset(1.7);
		hnHitsFit6->Draw();

    TCanvas *cPossetaSlices = new TCanvas("cPossetaSlices","",1500,900);
    cPossetaSlices->Divide(3,2);
    cPossetaSlices->cd(1);
    hnHitsPoss1->SetStats(0);
    hnHitsPoss1->GetXaxis()->SetTitle("nHitsPoss");
    hnHitsPoss1->GetYaxis()->SetTitle("# of Tracks");
    hnHitsPoss1->GetYaxis()->SetTitleOffset(1.7);
    hnHitsPoss1->Draw();
    cPossetaSlices->cd(2);
    hnHitsPoss2->SetStats(0);
    hnHitsPoss2->GetXaxis()->SetTitle("nHitsPoss");
    hnHitsPoss2->GetYaxis()->SetTitle("# of Tracks");
    hnHitsPoss2->GetYaxis()->SetTitleOffset(1.7);
    hnHitsPoss2->Draw();
    cPossetaSlices->cd(3);
    hnHitsPoss3->SetStats(0);
    hnHitsPoss3->GetXaxis()->SetTitle("nHitsPoss");
    hnHitsPoss3->GetYaxis()->SetTitle("# of Tracks");
    hnHitsPoss3->GetYaxis()->SetTitleOffset(1.7);
    hnHitsPoss3->Draw();
    cPossetaSlices->cd(4);
    hnHitsPoss4->SetStats(0);
    hnHitsPoss4->GetXaxis()->SetTitle("nHitsPoss");
    hnHitsPoss4->GetYaxis()->SetTitle("# of Tracks");
    hnHitsPoss4->GetYaxis()->SetTitleOffset(1.7);
    hnHitsPoss4->Draw();
    cPossetaSlices->cd(5);
    hnHitsPoss5->SetStats(0);
    hnHitsPoss5->GetXaxis()->SetTitle("nHitsPoss");
    hnHitsPoss5->GetYaxis()->SetTitle("# of Tracks");
    hnHitsPoss5->GetYaxis()->SetTitleOffset(1.7);
    hnHitsPoss5->Draw();
    cPossetaSlices->cd(6);
    hnHitsPoss6->SetStats(0);
    hnHitsPoss6->GetXaxis()->SetTitle("nHitsPoss");
    hnHitsPoss6->GetYaxis()->SetTitle("# of Tracks");
    hnHitsPoss6->GetYaxis()->SetTitleOffset(1.7);
    hnHitsPoss6->Draw();

	}
 
}




