#ifndef GEOMETRY_PARAMETERS_HH
#define GEOMETRY_PARAMETERS_HH

#include "G4NistManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "globals.hh"  // for G4double

extern const G4double xWorld;
extern const G4double yWorld;
extern const G4double zWorld;

extern const G4double xChip;
extern const G4double yChip;
extern const G4double zChip;

extern const G4double R;
extern const G4double zEllipsoid;
extern const G4double zCut;
extern const G4double zTranslate;

extern const G4double xyMold;
extern const G4double zMold;
extern const G4double xyInverse;
extern const G4double zInverse;
extern const G4double zSite;

extern const G4double dThickness;

#endif