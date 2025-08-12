#ifndef STEPPINGACTION_HH
#define STEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "G4ProcessManager.hh"

#include "G4OpticalPhoton.hh"
#include "G4VProcess.hh"
#include "G4SystemOfUnits.hh"
#include "G4OpBoundaryProcess.hh" 

class DetectorConstruction;
class EventAction;

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction();
    ~SteppingAction() override = default;

    void UserSteppingAction(const G4Step* step) override;
    static void PrintFinalStatistics();

  private:
    // const DetectorConstruction* fDetConstruction = nullptr;
    EventAction* fEventAction = nullptr;
};

// class PrintEveryProcess : public G4UserSteppingAction {
// public:
//     void UserSteppingAction(const G4Step* step) override
//     {
//         // Filter: optical photons only
//         if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::Definition()) return;

//         const auto* post  = step->GetPostStepPoint();
//         const auto* proc  = post->GetProcessDefinedStep();      // nullptr ⇒ Transportation

//         G4int    id      = step->GetTrack()->GetTrackID();
//         G4int    stepNo  = step->GetTrack()->GetCurrentStepNumber();
//         G4String pname   = proc ? proc->GetProcessName() : "Transportation";
//         G4double ke_eV   = post->GetKineticEnergy() / eV;

//         G4cout << "Trk " << id
//                << "  Step " << stepNo
//                << "  Process: " << pname
//                << "  E=" << ke_eV << " eV"
//                << G4endl;
//     }
// };

class PrintEveryProcess : public G4UserSteppingAction {
public:
    void UserSteppingAction(const G4Step* step) override
    {
        // ── optical photons only ───────────────────────────────────────────
        if (step->GetTrack()->GetDefinition() != G4OpticalPhoton::Definition()) return;

        const auto* post  = step->GetPostStepPoint();
        const auto* proc  = post->GetProcessDefinedStep();   // nullptr ⇒ Transportation

        G4int    id      = step->GetTrack()->GetTrackID();
        G4int    stepNo  = step->GetTrack()->GetCurrentStepNumber();
        G4String pname   = proc ? proc->GetProcessName() : "Transportation";

        // ── if this step ends at a boundary, append OpBoundary status ─────
        if (post->GetStepStatus() == fGeomBoundary) {
            // grab (once) the OpBoundaryProcess pointer
            static const G4OpBoundaryProcess* boundary = nullptr;
            if (!boundary) {
                auto* pm = step->GetTrack()->GetDefinition()->GetProcessManager();
                for (int i = 0; i < pm->GetProcessListLength(); ++i) {
                    auto* p = (*pm->GetProcessList())[i];
                    if (p->GetProcessName() == "OpBoundary") {
                        boundary = static_cast<const G4OpBoundaryProcess*>(p);
                        break;
                    }
                }
            }
            if (boundary) {
                switch (boundary->GetStatus()) {
                    case Transmission:            pname += " (Trans)";  break;
                    case FresnelReflection:       pname += " (Refl)";   break;
                    case TotalInternalReflection: pname += " (TIR)";    break;
                    case Detection:               pname += " (Det)";    break;
                    case Absorption:              pname += " (Abs)";    break;
                    default: /* keep original */  break;
                }
            }
        }

        G4double ke_eV = post->GetKineticEnergy() / eV;

        G4cout << "Trk "  << id
               << "  Step " << stepNo
               << "  Process: " << pname
               << "  E=" << ke_eV << " eV"
               << G4endl;
    }
};

#endif