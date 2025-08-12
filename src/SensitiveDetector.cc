#include "SensitiveDetector.hh"

#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"

#include "G4Positron.hh"
#include "G4OpticalPhoton.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh" 
#include "G4OpBoundaryProcess.hh"

SensitiveDetector::SensitiveDetector(const G4String& name)
    : G4VSensitiveDetector(name)
{}

void SensitiveDetector::Initialize(G4HCofThisEvent*)
{
    // reset the per-event accumulator (optional)
    fEnergy = 0.;
}

// G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
// {
//     // ── 0. accept ONLY positrons ────────────────────────────────────────────
//     const auto* track = step->GetTrack();
//     if (track->GetParticleDefinition() != G4Positron::PositronDefinition())
//         return false;
    
//     // Record the energy ONLY at the first step inside the SD volume
//     if (step->GetPreStepPoint()->GetStepStatus() != fGeomBoundary) return false;

//     G4double eKin = step->GetPreStepPoint()->GetKineticEnergy();   // MeV

//     // ---- OPTION A : write a row *per crossing* -----------------------------
//     auto man = G4AnalysisManager::Instance();
//     man->FillNtupleDColumn(0, eKin);     // ntuple 0, column 0
//     man->AddNtupleRow();

//     // ---- OPTION B : accumulate and write once per event --------------------
//     // fEnergy += eKin;

//     return true;
// }

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    const auto* track = step->GetTrack();

    // ── 0. accept ONLY optical photons ──────────────────────────────────────
    if (track->GetParticleDefinition() != G4OpticalPhoton::OpticalPhotonDefinition())
        return false;

    // ── 1. record ONLY the first point where the photon enters the SD volume
    if (step->GetPreStepPoint()->GetStepStatus() != fGeomBoundary) return false;
    
    // ── 2. convert photon energy to wavelength (nm) ─────────────────────────
    const G4double ePhoton = step->GetPreStepPoint()->GetKineticEnergy();   // in MeV
    G4double lambda_nm = (h_Planck * c_light) / ePhoton / nm;                                 // wavelength in nm

    // ---- OPTION A : write one row *per photon* ----------------------------
    auto man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(0, lambda_nm);   // ntuple 0, column 0 = λ [nm]
    man->AddNtupleRow();

    // ---- OPTION B : fill a histogram instead (uncomment if desired) -------
    // man->FillH1(0, lambda);            // histogram 0

    // ---- OPTION C : accumulate counts in wavelength bins yourself ---------
    // fSpectrum->Fill(lambda);

    return true;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent*)
{
    // Uncomment if you chose OPTION B above
    /*
    auto man = G4AnalysisManager::Instance();
    man->FillNtupleDColumn(0, fEnergy);   // one value per event
    man->AddNtupleRow();
    */
}
