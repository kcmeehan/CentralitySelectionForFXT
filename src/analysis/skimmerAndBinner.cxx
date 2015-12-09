//Applies cuts to skim the data at the event, vertex, and track level. Then bins the
//tracks in centrality, rapidity, and transverse mass (pT) for various particle species.

#include <iostream>

#include <TString.h>
#include <TFile.h>
#include <TTree.h>
#include <TH1D.h>
#include <TH3D.h>

#include "TrackInfo.h"
#include "PrimaryVertexInfo.h"
#include "EventInfo.h"

void skimmerAndBinner(TString inputDataFile, TString centralityFile, TString outputFile=""){

  //Create Pointers needed for reading the tree
  TrackInfo *track = NULL;
  PrimaryVertexInfo *primaryVertex = NULL;
  EventInfo *event = NULL;

  cout <<centralityFile <<endl;

}
