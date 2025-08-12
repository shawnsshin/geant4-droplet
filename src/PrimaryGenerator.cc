#include "PrimaryGenerator.hh"

#include "GeometryParameters.hh"
#include "G4ChargedGeantino.hh"
#include "G4RandomDirection.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "G4RunManager.hh"


PrimaryGenerator::PrimaryGenerator()
{
    fParticleGun = new G4ParticleGun(1);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4ParticleDefinition* particle = particleTable->FindParticle("chargedgeantino");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticlePosition(G4ThreeVector(0. * mm, 0. * mm, zChip * mm));
    fParticleGun->SetParticleEnergy(1 * eV);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1., 0., 0.));
}

PrimaryGenerator::~PrimaryGenerator()
{
    delete fParticleGun;
}

void PrimaryGenerator::GeneratePrimaries(G4Event *anEvent)
{

    G4ParticleDefinition* particle = fParticleGun->GetParticleDefinition();
    if (particle == G4ChargedGeantino::ChargedGeantino()) {
        // // fluorine
        G4int Z = 9, A = 18;
        // gallium
        // G4int Z = 31, A = 68;
        G4double ionCharge = 0. * eplus;
        G4double excitEnergy = 0. * keV;

        G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(Z, A, excitEnergy);
        fParticleGun->SetParticleDefinition(ion);
        fParticleGun->SetParticleCharge(ionCharge);
    }


    // Randomized starting position in full ellipsoid
    G4double u, costheta, sintheta, phi, r;
    G4double x, y, z;

    do {
        u = G4UniformRand();
        costheta = 2.0 * G4UniformRand() - 1.0;
        sintheta = std::sqrt(1.0 - costheta * costheta);
        phi = 2.0 * CLHEP::pi * G4UniformRand();
        r = std::cbrt(u);  // uniform in volume

        // Scale to ellipsoid dimensions
        x = R * r * sintheta * std::cos(phi);
        y = R * r * sintheta * std::sin(phi);
        z = zEllipsoid * r * costheta;
    } while (z < zCut);  // reject points below truncated base

    // Translate ellipsoid to chip surface
    G4ThreeVector pos = G4ThreeVector(x, y, z + zTranslate);  // match placement of droplet

    // Assign to particle
    fParticleGun->SetParticlePosition(pos);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0., 0., 0.));  // stationary
    fParticleGun->SetParticleEnergy(0. * keV);

    // Create vertex
    fParticleGun->GeneratePrimaryVertex(anEvent);
}