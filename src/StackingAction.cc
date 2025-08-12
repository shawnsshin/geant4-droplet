
#include "StackingAction.hh"

#include "G4NeutrinoE.hh"
#include "G4Gamma.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"


G4ClassificationOfNewTrack StackingAction::ClassifyNewTrack(const G4Track* track)
{
  // keep primary particle
  if (track->GetParentID() == 0) return fUrgent;

  // // kill secondary neutrino
  // if (track->GetDefinition() == G4NeutrinoE::NeutrinoE())
  //   return fKill;
  // else
  //   return fUrgent;

  // ---- 1. kill electron-neutrinos --------------------------------------
  if (track->GetDefinition() == G4NeutrinoE::NeutrinoE())
      return fKill;

  // ---- 2. kill annihilation gammas -------------------------------------
  if (track->GetDefinition() == G4Gamma::GammaDefinition())
  {
      const G4VProcess* creator = track->GetCreatorProcess();
      if (creator && creator->GetProcessName() == "annihil")   // 511 keV γ
          return fKill;
  }

  // otherwise keep
  return fUrgent;
}