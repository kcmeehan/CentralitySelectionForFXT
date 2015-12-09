/**************************************************************
 **  In this file you will implement your event, vertex,
 **  and track cuts in each of the following functions.
 **
 **************************************************************/

#include <TrackInfo.h>
#include <PrimaryVertexInfo.h>
#include <EventInfo.h>
#include <ParticleInfo.h>

//_______________________________________________________________
Bool_t isGoodEvent(EventInfo *event){

  //Returns true if this event passes all your cuts


  return true;

}

//_______________________________________________________________
Bool_t IsGoodVertex(PrimaryVertexInfo *vertex){

  //Returns true if this vertex passes all your cuts
  if (vertex->zVertex < -200 || vertex->zVertex >= -150)
    return false;

  Double_t rVertex = sqrt(pow(vertex->xVertex,2) + pow(vertex->yVertex,2));
  if (rVertex < 2 || rVertex > 5)
    return false;

  return true;
}

//_______________________________________________________________
Bool_t IsGoodTrack(PrimaryVertexInfo *track){

  //Returns true if this track passes all your cuts
  
  if ( (Double_t)track->nHitsFit / (Double_t)track->nHitsPoss < .52)
    return false;

  return true;
}
