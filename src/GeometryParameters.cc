#include "GeometryParameters.hh"

// World geometry parameters
const G4double xWorld = 5 * cm;
const G4double yWorld = 5 * cm;
const G4double zWorld = 5 * cm;


// Silicon wafer chip parameters
const G4double xChip = 6. * mm;
const G4double yChip = 6. * mm;
const G4double zChip = 0.5 * mm;


// Droplet shape parameters
const G4double contact_radius = 1.5 * mm; // 3 mm contact diameter
const G4double scale = 0.01149 * mm; // mm per pixel
const G4double R = 131.21 * scale * mm; // x and y semi-axes
const G4double zEllipsoid = 122.30 * scale * mm; // z semi-axis
// const G4double R = 1.5704040404040405 * mm; // x and y semi-axes
// const G4double zEllipsoid = 1.3303030303030303 * mm; // z semi-axis
const G4double zCut = -zEllipsoid * std::sqrt(1.0 - ((contact_radius * contact_radius) / (R * R))); // negative truncation height to model droplet on chip surface
const G4double zTranslate = - (zCut - zChip); // flush droplet to chip surface

// Converter parameters
const G4double xyMold = 6.0 * mm;
const G4double zMold = 2.2 * mm;
const G4double xyInverse = 2.0 * mm;  // semi-axes x and y
const G4double zInverse = 2.0 * mm;  // semi-axis z
const G4double zSite = -zInverse * std::sqrt(1.0 - ((2.0 * 2.0) / (xyInverse * xyInverse))); // ensure opening is 4 mm diameter

// Detector parameters
const G4double dThickness = 0.5 * mm;