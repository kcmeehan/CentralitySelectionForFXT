//A header file for the user accessible cuts

Bool_t IsGoodEvent(EventInfo *event);
Bool_t IsGoodVertex(PrimaryVertexInfo *vertex);
Bool_t IsGoodTrack(TrackInfo *track);
std::vector<double> GetCentralityPercents();
std::vector<int> GetCentralityCuts();
Int_t GetCentralityVariable(PrimaryVertexInfo *vertex);
Int_t GetCentralityBin(Int_t centralityVariable);
