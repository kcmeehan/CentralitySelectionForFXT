#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

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
#include <TVector3.h>
#include <TMath.h>

//after- verify that you need all these classes
#include "../submodules/datacollectorreaderlibs/TrackInfo/TrackInfo.h"
#include "../submodules/datacollectorreaderlibs/PrimaryVertexInfo/PrimaryVertexInfo.h"
#include "../submodules/datacollectorreaderlibs/EventInfo/EventInfo.h"
#include "ParticleInfo.h"
#include "UserCuts.h"

using namespace std;

//_____MAIN____________________
void trackQAmaker(TString inputDataFile, TString outputFile, Bool_t trackCuts = false, Int_t nEvents = -1){
//This function takes your input data file and produces an output file with vertex qa plots
//with no vertex cuts or with vertex-level cuts. Note that no track qa plots are made 
//you must use trackQAmaker.cxx for track qa. The purpose of this function is to allow 
//the user to optimize vertex cuts.

TFile *file     = new TFile(inputDataFile,"READ");
TTree *tree     = (TTree *)file->Get("DataTree");

TFile *outFile  = new TFile(outputFile,"RECREATE");

TrackInfo *track = NULL;
PrimaryVertexInfo *primaryVertex = NULL;
EventInfo *event = NULL;

tree->FindBranch("EventInfo")->SetAddress(&event);

TH2D *etaphiHist = new TH2D("etaphiHist","#phi vs #eta",500,-2.,0.5,500,-4,4);
TH2D *etaphiHistTOF = new TH2D("etaphiHistTOF","#phi vs #eta",500,-2.,0.5,500,-4,4);
TH2D *dEdxHist   = new TH2D("dEdxHist","Energy Loss in TPC",500,-6,6,500,0,100);
TH2D *dEdxHistZoom   = new TH2D("dEdxHistZoom","Energy Loss in TPC",1000,-2,2,1000,0,10);
TH2D *dEdxHistZoomPos   = new TH2D("dEdxHistZoomPos","Energy Loss in TPC",1000,0,4,1000,0,10);
TH2D *tofBeta    = new TH2D("tofBeta","TOF 1/#beta",500,-6,6,500,0,5);
TH2D *tofBetaZoom    = new TH2D("tofBetaZoom","TOF 1/#beta",300,-3,3,200,0.6,2);
TH2D *hphir = new TH2D("hphir","Phi vs. R of Last Hit",500,0,500,500,-4,4);
TH1D *hdca = new TH1D("hdca","dca",100,0,100);
TH2D *hdcaEta = new TH2D("hdcaEta","dca vs eta",500,-2.,0.5,100,0,10);
TH1D *hTrackLength = new TH1D("hTrackLength","Track Length",500,0,500);
TH2D *lengthVsEta  = new TH2D("lengthVsEta","Track Length vs. #eta",500,-2.,0.5,500,0,500);
TH2D *nRatVsEta    = new TH2D("nRatVsEta","nHitsRatio vs. #eta",500,-2.,0.5,50,0.0,1.05);
TH1D *firstZ       = new TH1D("firstZ","First Hit",1000,-225,225);
TH1D *lastZ        = new TH1D("lastZ","Last Hit",1000,-225,225);
TH2D *firstXY      = new TH2D("firstXY","First Hit",800,-200,200,800,-200,200);
TH2D *lastXYeast   = new TH2D("lastXYeast","Last Hit",1000,-250,250,1000,-250,250);
TH2D *lastXYwest   = new TH2D("lastXYwest","Last Hit",1000,-250,250,1000,-250,250);
TH2D *firstXYeast  = new TH2D("firstXYeast","First Hit",800,-200,200,800,-200,200);
TH2D *firstXYwest  = new TH2D("firstXYwest","First Hit",800,-200,200,800,-200,200);
TH2D *firstZvsDCA  = new TH2D("firstZvsDCA","DCA vs FirstZ",1000,-225,225,100,0,100);
TH1D *hnHits       = new TH1D("hnHits","nHits",60,0,60);
TH1D *hnHitsFit    = new TH1D("hnHitsFit","nHits Fit",60,0,60);
TH1D *hnHitsPoss   = new TH1D("hnHitsPoss","nHits Poss",70,0,70);
TH1D *hnHitsRat    = new TH1D("hnHitsRat","nHitsFit/nHitsPoss",101,-0.05,1.05);
TH1D *hnHitsdEdx   = new TH1D("hnHitsdEdx","nHitsdEdx",60,0,60);
TH2D *pimAccept    = new TH2D("pimAccept","#pi^{-} Acceptance",500,-2,-0,500,0,2.5);
TH2D *pipAccept    = new TH2D("pipAccept","#pi^{+} Acceptance",500,-2,-0,500,0,2.5);
TH2D *proAccept    = new TH2D("proAccept","Proton Acceptance",500,-2,-0,500,0,4);

//rapidity binning
TH1D *pTbin1       = new TH1D("pTbin1","p_{T} for Protons in -0.5 < y < 0",500,0,4);
TH1D *pTbin2       = new TH1D("pTbin2","p_{T} for Protons in -1.0 < y < -0.5",500,0,4);
TH1D *pTbin3       = new TH1D("pTbin3","p_{T} for Protons in -1.5 < y < -1.0",500,0,4);
TH1D *pTbin4       = new TH1D("pTbin4","p_{T} for Protons in -1.9 < y < -1.5",500,0,4);

TH1D *mTbin1       = new TH1D("mTbin1","m_{T} for Protons in -0.5 < y < 0",500,0,4);
TH1D *mTbin2       = new TH1D("mTbin2","m_{T} for Protons in -1.0 < y < -0.5",500,0,4);
TH1D *mTbin3       = new TH1D("mTbin3","m_{T} for Protons in -1.5 < y < -1.0",500,0,4);
TH1D *mTbin4       = new TH1D("mTbin4","m_{T} for Protons in -1.9 < y < -1.5",500,0,4);

TH2D *hdEdx1 = new TH2D("hdEdx1","dE/dx -0.3 < #eta < 0",500,-6,6,500,0,10);
TH2D *hdEdx2 = new TH2D("hdEdx2","dE/dx -0.6 < #eta < -0.3",500,-6,6,500,0,10);
TH2D *hdEdx3 = new TH2D("hdEdx3","dE/dx -0.9 < #eta < -0.6",500,-6,6,500,0,10);
TH2D *hdEdx4 = new TH2D("hdEdx4","dE/dx -1.2 < #eta < -0.9",500,-6,6,500,0,10);
TH2D *hdEdx5 = new TH2D("hdEdx5","dE/dx -1.5 < #eta < -1.2",500,-6,6,500,0,10);
TH2D *hdEdx6 = new TH2D("hdEdx6","dE/dx -1.8 < #eta < -1.5",500,-6,6,500,0,10);

TH1D *hnHitsRat1 = new TH1D("hnHitsRat1","nHitsFit/nHitsPoss -0.3 < #eta < 0",101,-0.05,1.5);
TH1D *hnHitsRat2 = new TH1D("hnHitsRat2","nHitsFit/nHitsPoss -0.6 < #eta < -0.3",101,-0.05,1.5);
TH1D *hnHitsRat3 = new TH1D("hnHitsRat3","nHitsFit/nHitsPoss -0.9 < #eta < -0.6",101,-0.05,1.5);
TH1D *hnHitsRat4 = new TH1D("hnHitsRat4","nHitsFit/nHitsPoss -1.2 < #eta < -0.9",101,-0.05,1.5);
TH1D *hnHitsRat5 = new TH1D("hnHitsRat5","nHitsFit/nHitsPoss -1.5 < #eta < -1.2",101,-0.05,1.5);
TH1D *hnHitsRat6 = new TH1D("hnHitsRat6","nHitsFit/nHitsPoss -1.8 < #eta < -1.5",101,-0.05,1.5);

TH1D *hnHitsFit1 = new TH1D("hnHitsFit1","nHitsFit -0.3 < #eta < 0",60,0,60);
TH1D *hnHitsFit2 = new TH1D("hnHitsFit2","nHitsFit -0.6 < #eta < -0.3",60,0,60);
TH1D *hnHitsFit3 = new TH1D("hnHitsFit3","nHitsFit -0.9 < #eta < -0.6",60,0,60);
TH1D *hnHitsFit4 = new TH1D("hnHitsFit4","nHitsFit -1.2 < #eta < -0.9",60,0,60);
TH1D *hnHitsFit5 = new TH1D("hnHitsFit5","nHitsFit -1.5 < #eta < -1.2",60,0,60);
TH1D *hnHitsFit6 = new TH1D("hnHitsFit6","nHitsFit -1.8 < #eta < -1.5",60,0,60);

TH1D *hnHitsPoss1 = new TH1D("hnHitsPoss1","nHitsPoss -0.3 < #eta < 0",60,0,60);
TH1D *hnHitsPoss2 = new TH1D("hnHitsPoss2","nHitsPoss -0.6 < #eta < -0.3",60,0,60);
TH1D *hnHitsPoss3 = new TH1D("hnHitsPoss3","nHitsPoss -0.9 < #eta < -0.6",60,0,60);
TH1D *hnHitsPoss4 = new TH1D("hnHitsPoss4","nHitsPoss -1.2 < #eta < -0.9",60,0,60);
TH1D *hnHitsPoss5 = new TH1D("hnHitsPoss5","nHitsPoss -1.5 < #eta < -1.2",60,0,60);
TH1D *hnHitsPoss6 = new TH1D("hnHitsPoss6","nHitsPoss -1.8 < #eta < -1.5",60,0,60);


Double_t x(0), y(0), z(0), refMultUser(0);
Int_t nPions, newNtof, pvEntries;
Double_t entries;
if(nEvents > 0) entries = nEvents;
else entries = tree->GetEntries();
for(Int_t i=0; i<entries; i++){//loop over triggers
  tree->GetEntry(i);
	if (!IsGoodEvent(event)) continue;
	pvEntries = event->primaryVertexArray->GetEntries();
  for (Int_t j=0; j<pvEntries; j++){//loop over vertices
	  primaryVertex = (PrimaryVertexInfo *)event->primaryVertexArray->At(j);
    if (!IsGoodVertex(primaryVertex)) continue;
	    for(Int_t k = 0; k<primaryVertex->trackArray->GetEntries();k++){//loop over tracks
			  track = (TrackInfo *)primaryVertex->trackArray->At(k);
				if (trackCuts) {
          if (!IsGoodTrack(track)) continue;
				}
        //hits variables
				Double_t nHitsFit = track->nHitsFit;
			  Double_t nHitsPoss = track->nHitsPoss;
				Double_t nHitsRatio = nHitsFit/nHitsPoss;
        hnHits->Fill(track->nHits);
        hnHitsFit->Fill(nHitsFit);
        hnHitsPoss->Fill(nHitsPoss);
        hnHitsRat->Fill(nHitsRatio);
        hnHitsdEdx->Fill(track->dEdxHits);         
        Double_t firstHitx = track->firstPoint.x();
        Double_t firstHity = track->firstPoint.y();
        Double_t firstHitz = track->firstPoint.z();
        Double_t lastHitx = track->lastPoint.x();
        Double_t lastHity = track->lastPoint.y();
        Double_t lastHitz = track->lastPoint.z();
        Double_t dcaD = track->globalDCA.Mag();
        Double_t rLastHit = sqrt(lastHitx*lastHitx + lastHity*lastHity);
        hphir->Fill(rLastHit,phi);
        hdca->Fill(dcaD);
        firstZ->Fill(firstHitz);
        lastZ->Fill(lastHitz);
        firstXY->Fill(firstHitx,firstHity); 
        if(lastHitz < 0){
         lastXYwest->Fill(lastHitx,lastHity);
         firstXYwest->Fill(firstHitx,firstHity);
        }
        if(lastHitz > 0){
          lastXYeast->Fill(lastHitx,lastHity);
          firstXYeast->Fill(firstHitx,firstHity);
        }
        firstZvsDCA->Fill(firstHitz,dcaD);

        //kinematic variables
        Double_t eta=track->eta;
        Double_t phi=track->phi;
        Double_t q = track->charge;
        Double_t pT = track->pT;
        Double_t p = sqrt(pow(pT,2) + pow(track->pZ,2));
        Double_t length = track->length;
        hdcaEta->Fill(eta,dcaD);
        hTrackLength->Fill(length);
        lengthVsEta->Fill(eta,length);
        nRatVsEta->Fill(eta,nHitsRatio);

        //pid variables and acceptance plots 
        Double_t dEdx = track->dEdx*1000000;
        Double_t nSigmaPi = track->nSigmaPion;
        Double_t nSigmaPro = track->nSigmaProton;
        Double_t yPi = TMath::ATanH(track->pZ / sqrt(mPion*mPion + p*p));
        Double_t yPro = TMath::ATanH(track->pZ / sqrt(mProton*mProton + p*p));
        Double_t mT = sqrt(mPion*mPion + pT*pT);
        Double_t mTpro = sqrt(mProton*mProton + pT*pT);
        if(q < 0 && abs(nSigmaPi) < 2) pimAccept->Fill(yPi,pT);
        if(q > 0 && abs(nSigmaPi) < 2 && nSigmaPro < -1) pipAccept->Fill(yPi,pT);
        if(q > 0 && abs(nSigmaPro) < 2){
          proAccept->Fill(yPro,pT);
          if(-0.5 < yPro && yPro < 0){
            pTbin1->Fill(pT,1./pT);
            mTbin1->Fill(mTpro,1./mTpro);
          }
          if(-1.0 < yPro && yPro < -0.5){
            pTbin2->Fill(pT,1./pT);
            mTbin2->Fill(mTpro,1./mTpro);
          }
          if(-1.5 < yPro && yPro < -1.0){
            pTbin3->Fill(pT,1./pT);
            mTbin3->Fill(mTpro,1./mTpro);
          }
          if(-1.9 < yPro && yPro < -1.5){
            pTbin4->Fill(pT,1./pT);
            mTbin4->Fill(mTpro,1./mTpro);
          }
        }
        etaphiHist->Fill(track->eta,track->phi);
        if(track->tofMatchFlag > 0) etaphiHistTOF->Fill(track->eta,track->phi);
        dEdxHist->Fill(q*p,dEdx);
        dEdxHistZoom->Fill(q*p,dEdx);
        dEdxHistZoomPos->Fill(q*p,dEdx);
        tofBeta->Fill(q*p,1.0/track->tofBeta);
        tofBetaZoom->Fill(q*p,1.0/track->tofBeta);
        
        //rapidity binning 
        if(eta < 0 && eta > -0.3){
          hdEdx1->Fill(q*p,track->dEdx*1000000);
          hnHitsRat1->Fill(nHitsRatio);
          hnHitsFit1->Fill(nHitsFit);
          hnHitsPoss1->Fill(nHitsPoss);
        }
        if(eta < -0.3 && eta > -0.6){
          hdEdx2->Fill(q*p,track->dEdx*1000000);
          hnHitsRat2->Fill(nHitsRatio);
          hnHitsFit2->Fill(nHitsFit);
          hnHitsPoss2->Fill(nHitsPoss);
        }
        if(eta < -0.6 && eta > -0.9){
          hdEdx3->Fill(p*q,track->dEdx*1000000);
          hnHitsRat3->Fill(nHitsRatio);
          hnHitsFit3->Fill(nHitsFit);
          hnHitsPoss3->Fill(nHitsPoss);
        }
        if(eta < -0.9 && eta > -1.2){
          hdEdx4->Fill(p*q,track->dEdx*1000000);
          hnHitsRat4->Fill(nHitsRatio);
          hnHitsFit4->Fill(nHitsFit);
          hnHitsPoss4->Fill(nHitsPoss);
        }
        if(eta < -1.2 && eta > -1.5){
         hdEdx5->Fill(p*q,track->dEdx*1000000);
         hnHitsRat5->Fill(nHitsRatio);
         hnHitsFit5->Fill(nHitsFit);
         hnHitsPoss5->Fill(nHitsPoss);
        }
        if(eta < -1.5 && eta > -1.8){
          hdEdx6->Fill(p*q,track->dEdx*1000000);
          hnHitsRat6->Fill(nHitsRatio);
          hnHitsFit6->Fill(nHitsFit);
          hnHitsPoss6->Fill(nHitsPoss);
        }
        
		  }//end loop over tracks
 }//end of loop over vertices
}//end of loop over triggers

file->Close();
outFile->Write();

}//end of function
