#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "globals.hh"

class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

class SensitiveDetector : public G4VSensitiveDetector
{
    public:
        SensitiveDetector(const G4String& name);
        ~SensitiveDetector() override = default;

        // methods from base class
        void Initialize(G4HCofThisEvent* hitCollection) override;
        G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
        void EndOfEvent(G4HCofThisEvent* hitCollection) override;

    private:
        G4double fEnergy;

};

#endif