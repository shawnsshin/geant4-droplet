#include "/home/sshin/dev/droplet/opt_droplet_sim/include/SteppingAction.hh"
#include "/home/sshin/dev/droplet/opt_droplet_sim/include/GeometryParameters.hh"

#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Positron.hh"
#include "G4VProcess.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"

#include "G4OpticalPhoton.hh"
#include "G4AnalysisManager.hh"
#include "G4PhysicalConstants.hh" 
#include "G4OpBoundaryProcess.hh"
#include <vector>
#include <map>
#include <set>
#include <mutex>

// Global counters for escape efficiency
static G4int totalCherenkovPhotonsDroplet = 0;
static G4int totalCherenkovPhotonsEverywhere = 0;
static G4int totalCherenkovPhotonsSilicon = 0;

static G4int escapedPhotons = 0;
static G4int escapedPhotonsaboveChip = 0;
static G4int escapedPhotonsbelowChip = 0;
static G4int escapedPhotonsatChip = 0;

// New counters to track escape by exit material
static G4int escapedFromDroplet = 0;
static G4int escapedFromChip = 0;

static G4int trappedInTracer = 0;
static G4int trappedInSilicon = 0;

static std::set<G4int> escapedPhotonIDs;  // Track which photons have already been counted as escaped
static std::mutex counterMutex;

SteppingAction::SteppingAction()
{
}

void SteppingAction::UserSteppingAction(const G4Step* step)
{
    G4Track* track = step->GetTrack();

    // ----------------------------------------------------------
    // Global safety guard: kill any track after too many steps
    // ----------------------------------------------------------
    if (track->GetCurrentStepNumber() > 10000)
    {
        // For optical photons, check if they're outside droplet before killing
        if (track->GetDefinition() == G4OpticalPhoton::Definition()) {
            G4int trackID = track->GetTrackID();
            {
                G4String currentMaterial = track->GetMaterial()->GetName();
                
                std::lock_guard<std::mutex> lock(counterMutex);
                // Track where photon ended up
                if (currentMaterial == "Tracer") {
                    trappedInTracer++;
                } else if (currentMaterial == "G4_Si") {
                    trappedInSilicon++;
                } else {
                    escapedPhotons++;
                    if (track->GetPosition().z() < zChip) {
                        escapedPhotonsbelowChip++;
                    } else if (track->GetPosition().z() > zChip) {
                        escapedPhotonsaboveChip++;
                    } else {
                        escapedPhotonsatChip++;
                    }
                }
            }
        }
        track->SetTrackStatus(fStopAndKill);
    }

    // ----------------------------------------------------------
    //  Your existing positron-specific cut
    // ----------------------------------------------------------
    if (track->GetDefinition() == G4Positron::PositronDefinition())
    {
        G4double eKin = track->GetKineticEnergy();

        // Kill positron if it's below threshold (195 keV)
        if (eKin < 0.195 * MeV)
        {
            track->SetTrackStatus(fStopAndKill);
        }
    }


    const auto& secondaries = *step->GetSecondaryInCurrentStep();
    for (const auto* sec : secondaries)
    {
        if (sec->GetDefinition() == G4OpticalPhoton::Definition())
        {
            const auto* proc = sec->GetCreatorProcess();
            if (proc && proc->GetProcessName() == "Cerenkov")
            {
                std::lock_guard<std::mutex> lock(counterMutex);

                totalCherenkovPhotonsEverywhere++;
                
                G4String birthMaterial = track->GetMaterial()->GetName();
                if (birthMaterial == "Tracer") {
                    totalCherenkovPhotonsDroplet++;
                } else if (birthMaterial == "G4_Si") {
                    totalCherenkovPhotonsSilicon++;
                }
            }
        }
    }

    // Track photon boundary crossings to detect when they escape from droplet/chip to world
    if (track->GetDefinition() == G4OpticalPhoton::Definition()) {
        G4int trackID = track->GetTrackID();
        G4StepPoint* prePoint = step->GetPreStepPoint();
        G4StepPoint* postPoint = step->GetPostStepPoint();
        
        // Safety checks
        if (!prePoint || !postPoint || !prePoint->GetMaterial() || !postPoint->GetMaterial()) {
            return;
        }
        
        G4String preMaterial = prePoint->GetMaterial()->GetName();
        G4String postMaterial = postPoint->GetMaterial()->GetName();
        
        // Detect escape: transition from droplet/chip to world
        if ((preMaterial == "Tracer" || preMaterial == "G4_Si") && 
            (postMaterial != "Tracer" && postMaterial != "G4_Si")) {
            
            std::lock_guard<std::mutex> lock(counterMutex);
            
            // Only count if this photon hasn't been counted as escaped already
            if (escapedPhotonIDs.find(trackID) == escapedPhotonIDs.end()) {
                escapedPhotonIDs.insert(trackID);
                escapedPhotons++;
                
                if (track->GetPosition().z() < zChip) {
                    escapedPhotonsbelowChip++;
                } else if (track->GetPosition().z() > zChip) {
                    escapedPhotonsaboveChip++;
                } else {
                    escapedPhotonsatChip++;
                }
                
                // Count escape by exit material
                if (preMaterial == "Tracer") {
                    escapedFromDroplet++;
                } else if (preMaterial == "G4_Si") {
                    escapedFromChip++;
                }
            }
        }
    }
    
    // Simple photon fate tracking - check final position when track ends
    if (track->GetDefinition() == G4OpticalPhoton::Definition() && 
        track->GetTrackStatus() != fAlive)
    {
        G4int trackID = track->GetTrackID();
        
        // Process all optical photons (assume they're Cherenkov since that's what we generate)
        {
            // Check if photon naturally ended outside droplet
            G4String finalMaterial = track->GetMaterial()->GetName();
            G4String processName = "Unknown";
            if (track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()) {
                processName = track->GetStep()->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName();
            }
            
            // Debug: print death info for first few photons
            static G4int deathDebugCount = 0;
            if (deathDebugCount < 10) {
                G4cout << "DEBUG: Photon " << trackID << " died in material '" 
                       << finalMaterial << "' via process '" << processName 
                       << "' after " << track->GetCurrentStepNumber() << " steps" << G4endl;
                deathDebugCount++;
            }
            
            std::lock_guard<std::mutex> lock(counterMutex);
            
            // Track where photon ended up
            if (finalMaterial == "Tracer") {
                trappedInTracer++;
            } else if (finalMaterial == "G4_Si") {
                trappedInSilicon++;
            }
            
            // Clean up tracking set
            escapedPhotonIDs.erase(trackID);
        }
    }
}

void SteppingAction::PrintFinalStatistics()
{
    if (totalCherenkovPhotonsDroplet > 0) {
        G4double efficiency = (G4double)escapedPhotons / (G4double)totalCherenkovPhotonsDroplet * 100.0;
        G4double tracerFraction = (G4double)trappedInTracer / (G4double)totalCherenkovPhotonsDroplet * 100.0;
        G4double siliconFraction = (G4double)trappedInSilicon; // / (G4double)totalCherenkovPhotonsEverywhere * 100.0;
        
        G4cout << "=========================================" << G4endl;
        G4cout << "=== FINAL PHOTON STATISTICS ===" << G4endl;
        G4cout << "=========================================" << G4endl;
        G4cout << "Total Cherenkov photons (everywhere): " << totalCherenkovPhotonsEverywhere << G4endl;
        G4cout << "Total Cherenkov photons (from droplet): " << totalCherenkovPhotonsDroplet << G4endl;
        G4cout << "Total Cherenkov photons (from chip): " << totalCherenkovPhotonsSilicon << G4endl;
        G4cout << "Total Escaped Cherenkov Photons: " << escapedPhotons << G4endl;
        G4cout << "Escaped Photons below Chip: " << escapedPhotonsbelowChip << G4endl;
        G4cout << "Escaped Photons above Chip: " << escapedPhotonsaboveChip << G4endl;
        G4cout << "Escaped Photons at Chip: " << escapedPhotonsatChip << G4endl;
        G4cout << "Escaped from Droplet: " << escapedFromDroplet << G4endl;
        G4cout << "Escaped from Chip: " << escapedFromChip << G4endl;
        G4cout << "Trapped in Droplet: " << trappedInTracer << G4endl;
        G4cout << "Trapped in Chip: " << trappedInSilicon << G4endl;
        G4cout << "=========================================" << G4endl;
    } else {
        G4cout << "No Cherenkov photons were generated in the droplet during this run." << G4endl;
    }
}