#include "DetectorConstruction.hh"
#include "SensitiveDetector.hh"
#include "GeometryParameters.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Ellipsoid.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4RotationMatrix.hh"
#include "G4Transform3D.hh"
#include "G4SDManager.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4PSEnergyDeposit.hh"
#include "G4PSDoseDeposit.hh"
#include "G4VisAttributes.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//
#include "G4SubtractionSolid.hh"
#include "G4UnionSolid.hh"
#include "G4IntersectionSolid.hh"
//

#include "G4Element.hh"
#include "G4Colour.hh"
#include "G4RunManager.hh"
#include "G4StateManager.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4AssemblyVolume.hh"

#include "G4OpticalSurface.hh"         // optical surface class
#include "G4LogicalBorderSurface.hh"

DetectorConstruction::DetectorConstruction()
{
}


G4VPhysicalVolume *DetectorConstruction::Construct()
{
    G4NistManager *nist  = G4NistManager::Instance();

    // Define World Materials
    G4Material *air = nist->FindOrBuildMaterial("G4_AIR");
    G4Material *water = nist->FindOrBuildMaterial("G4_WATER");
    G4Material *silicon = nist->FindOrBuildMaterial("G4_Si");
    G4Material *lead = nist->FindOrBuildMaterial("G4_Pb");
    G4Material *tungsten = nist->FindOrBuildMaterial("G4_W");
    G4Material *plexiglass = nist->FindOrBuildMaterial("G4_BERYLLIUM_OXIDE");

    // Define Elements
    G4Element* elC  = nist->FindOrBuildElement("C");
    G4Element* elH  = nist->FindOrBuildElement("H");
    G4Element* elO  = nist->FindOrBuildElement("O");
    G4Element* elSi = nist->FindOrBuildElement("Si");
    G4Element* elB  = nist->FindOrBuildElement("B");
    G4Element* elNa = nist->FindOrBuildElement("Na");
    G4Element* elAl = nist->FindOrBuildElement("Al");

    // Define PDMS
    G4Material* PDMS = new G4Material("PDMS", 0.97 * g/cm3, 4, kStateSolid);
    PDMS->AddElement(elC, 2);
    PDMS->AddElement(elH, 6);
    PDMS->AddElement(elO, 1);
    PDMS->AddElement(elSi, 1);

    // Define Borosilicate Glass
    G4Material* Borosilicate = new G4Material("Borosilicate", 2.23 * g/cm3, 5, kStateSolid);
    Borosilicate->AddElement(elSi, 0.406);
    Borosilicate->AddElement(elO,  0.539);
    Borosilicate->AddElement(elB,  0.080);
    Borosilicate->AddElement(elNa, 0.020);
    Borosilicate->AddElement(elAl, 0.015);

    // Define Fluorine-18
    G4Isotope *F18 = new G4Isotope("F18", 9, 18, 18.000938 * g / mole);
    G4Element *elF18 = new G4Element("Fluorine-18", "F18", 1);
    elF18->AddIsotope(F18, 100.0 * perCent);

    // Define Gallium-68
    G4Isotope *Ga68 = new G4Isotope("Ga68", 31, 68, 67.92798 * g / mole);
    G4Element *elGa68 = new G4Element("Gallium-68", "Ga68", 1);
    elGa68->AddIsotope(Ga68, 100.0 * perCent);

    // Define droplet of radiotracer
    G4Material *tracer = new G4Material("Tracer", 1 * g/cm3, 2, kStateLiquid);
    tracer->AddMaterial(water, 99.9999 * perCent);
    tracer->AddElement(elGa68, 0.0001 * perCent);

    // Define Optical Properties for Cherenkov Simulation

    const G4int nEntries = 52;
    G4double photonEnergy[nEntries] = { // 400 nm to 1000 nm
    1.240000*eV, 1.264790*eV, 1.277166*eV, 1.292070*eV, 1.304439*eV, 1.319430*eV,
    1.341701*eV, 1.378850*eV, 1.416010*eV, 1.458309*eV, 1.502848*eV, 1.550000*eV,
    1.569819*eV, 1.589744*eV, 1.609553*eV, 1.631794*eV, 1.676582*eV, 1.698863*eV,
    1.710345*eV, 1.722222*eV, 1.746479*eV, 1.771429*eV, 1.797101*eV, 1.823529*eV,
    1.850746*eV, 1.878788*eV, 1.907692*eV, 1.937500*eV, 1.968254*eV, 2.000000*eV,
    2.032787*eV, 2.066667*eV, 2.101695*eV, 2.137931*eV, 2.175439*eV, 2.214286*eV,
    2.254545*eV, 2.296296*eV, 2.339623*eV, 2.384615*eV, 2.431373*eV, 2.480000*eV,
    2.530612*eV, 2.583333*eV, 2.638298*eV, 2.695652*eV, 2.755556*eV, 2.818182*eV,
    2.883721*eV, 2.952381*eV, 3.024390*eV, 3.100000*eV
    };

    // Refractive Index


    constexpr G4double nBorosilicate = 1.500;
    constexpr G4double nPDMS = 1.433;
    constexpr G4double nWater = 1.333;
    constexpr G4double nAir = 1.;
    
    G4double rindexBorosilicate[nEntries];
    G4double rindexPDMS[nEntries];
    G4double rindexWater[nEntries];
    G4double rindexAir[nEntries];
    G4double rindexSilicon[nEntries] = {
    3.575, 3.572, 3.573, 3.574, 3.575, 3.577, 3.580, 3.585, 3.591, 3.598,
    3.606, 3.616, 3.621, 3.627, 3.633, 3.640, 3.655, 3.664, 3.669, 3.674,
    3.685, 3.696, 3.708, 3.721, 3.734, 3.748, 3.763, 3.779, 3.795, 3.813,
    3.831, 3.851, 3.871, 3.893, 3.916, 3.941, 3.967, 3.996, 4.026, 4.058,
    4.093, 4.131, 4.172, 4.217, 4.267, 4.322, 4.384, 4.453, 4.531, 4.618,
    4.718, 4.832
    };

    std::fill_n(rindexBorosilicate, nEntries, nBorosilicate);
    std::fill_n(rindexPDMS, nEntries, nPDMS);
    std::fill_n(rindexWater, nEntries, nWater);
    std::fill_n(rindexAir, nEntries, nAir);

    // Absorption Length
    G4double absWater[nEntries] = {
    0.02458*m,  0.02070*m,  0.02067*m,  0.02268*m,  0.03344*m,  0.05464*m,
    0.10152*m,  0.15748*m,  0.19841*m,  0.23753*m,  0.38168*m,  0.44643*m,
    0.40650*m,  0.37175*m,  0.35461*m,  0.34843*m,  0.35842*m,  0.50761*m,
    0.66845*m,  0.80906*m,  1.20773*m,  1.60256*m,  1.93798*m,  2.15054*m,
    2.27790*m,  2.43902*m,  2.94118*m,  3.21958*m,  3.42818*m,  3.62319*m,
    3.76932*m,  4.48029*m,  7.38552*m, 11.16071*m, 14.40922*m, 16.15509*m,
    17.76199*m, 21.05263*m, 23.58491*m, 25.51020*m, 30.30303*m, 48.30918*m,
    68.49315*m, 82.64463*m, 97.08738*m, 109.89011*m, 123.45679*m, 192.30769*m,
    265.95745*m, 320.51282*m, 375.93985*m, 450.45045*m
    };

    G4double absSilicon[nEntries] = {
    1.5625E-06*m, 1.9569E-06*m, 2.2727E-06*m, 2.6316E-06*m, 2.8985E-06*m, 3.2895E-06*m,
    3.7879E-06*m, 4.5455E-06*m, 5.8140E-06*m, 7.8740E-06*m, 9.0090E-06*m, 1.0204E-05*m,
    1.1111E-05*m, 1.1364E-05*m, 1.1765E-05*m, 1.2658E-05*m, 1.4286E-05*m, 1.4815E-05*m,
    1.5625E-05*m, 1.6129E-05*m, 1.6949E-05*m, 1.8182E-05*m, 1.9231E-05*m, 2.0408E-05*m,
    2.1739E-05*m, 2.3077E-05*m, 2.4390E-05*m, 2.5974E-05*m, 2.7778E-05*m, 2.9412E-05*m,
    3.0769E-05*m, 3.2967E-05*m, 3.5714E-05*m, 3.8462E-05*m, 4.1667E-05*m, 4.4843E-05*m,
    4.8780E-05*m, 5.4945E-05*m, 6.0976E-05*m, 6.6667E-05*m, 7.8740E-05*m, 8.9286E-05*m,
    1.0204E-04*m, 1.1905E-04*m, 1.4085E-04*m, 1.6667E-04*m, 1.9608E-04*m, 2.4155E-04*m,
    2.9155E-04*m, 3.4014E-04*m, 4.3860E-04*m, 5.4348E-04*m
    };


    // Attach Material Properties
    auto mptBorosilicate = new G4MaterialPropertiesTable();
    mptBorosilicate->AddProperty("RINDEX", photonEnergy, rindexBorosilicate, nEntries);
    Borosilicate->SetMaterialPropertiesTable(mptBorosilicate);

    auto mptPDMS = new G4MaterialPropertiesTable();
    mptPDMS->AddProperty("RINDEX", photonEnergy, rindexPDMS, nEntries);
    PDMS->SetMaterialPropertiesTable(mptPDMS);

    auto mptWater = new G4MaterialPropertiesTable();
    mptWater->AddProperty("RINDEX", photonEnergy, rindexWater, nEntries);
    mptWater->AddProperty("ABSLENGTH", photonEnergy, absWater, nEntries);
    tracer->SetMaterialPropertiesTable(mptWater);

    auto mptAir = new G4MaterialPropertiesTable();
    mptAir->AddProperty("RINDEX", photonEnergy, rindexAir, nEntries);
    air->SetMaterialPropertiesTable(mptAir);

    auto mptSilicon = new G4MaterialPropertiesTable();
    mptSilicon->AddProperty("RINDEX", photonEnergy, rindexSilicon, nEntries);
    mptSilicon->AddProperty("ABSLENGTH", photonEnergy, absSilicon, nEntries);
    silicon->SetMaterialPropertiesTable(mptSilicon);

    //
    // World
    //

    auto solidWorld = new G4Box("solidWorld", // name
                                0.5 * xWorld, 0.5 * yWorld, 0.5 * zWorld); // size

    auto logicWorld = new G4LogicalVolume(solidWorld, // corresponding solid volume
                                            air, // world material
                                            "logicalWorld");

    auto physWorld = new G4PVPlacement(nullptr, // no rotation
                                        G4ThreeVector(0., 0., 0.), // at (0, 0, 0)
                                        logicWorld, // corresponding logical volume
                                        "physWorld",
                                        nullptr,
                                        false,
                                        0,
                                        fCheckOverlaps);

    G4VisAttributes* invisibleAtt = new G4VisAttributes(G4Colour(0.0, 0.0, 0.0, 0.0));
    invisibleAtt->SetVisibility(false);
    logicWorld->SetVisAttributes(invisibleAtt);


    //
    // Silicon Wafer Chip
    //

    auto solidChip = new G4Box("solidChip",
                                0.5 * xChip, 0.5 * yChip, 0.5 * zChip);

    auto logicChip = new G4LogicalVolume(solidChip, silicon, "logicalChip");

    auto physChip = new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.5 * zChip), logicChip, "physChip", logicWorld, false, 0, fCheckOverlaps);

    // Chip color scheme
    G4VisAttributes* chipVis = new G4VisAttributes(G4Colour(0.6, 0.8, 0.0, 0.1));  // R, G, B, opaqueness
    chipVis->SetForceSolid(true);
    logicChip->SetVisAttributes(chipVis);


    //
    // Ellipsoidal Tracer Droplet
    //

    auto solidDroplet = new G4Ellipsoid("Droplet",
                                            R,  // x semi-axis
                                            R,  // y semi-axis
                                            zEllipsoid,  // z semi-axis
                                            zCut,  // cut below Z=0 → flat bottom
                                            zEllipsoid); // no cut on top (full rounded top)

    auto logicDroplet = new G4LogicalVolume(solidDroplet,
                                                tracer,
                                                "logicalDroplet");

    auto physDroplet = new G4PVPlacement(nullptr,
                                            G4ThreeVector(0, 0, zTranslate),  // raise so base sits on chip surface
                                            logicDroplet,
                                            "physDroplet",
                                            logicWorld,
                                            false,
                                            0,
                                            fCheckOverlaps);

    // Droplet color scheme
    G4VisAttributes *dropletVis = new G4VisAttributes(G4Color(0.0, 0.5, 0.8, 0.5));  // R, G, B, opaqueness
    dropletVis->SetForceSolid(true);
    logicDroplet->SetVisAttributes(dropletVis);

    // Optical Surface for Droplet-Air Interface

    G4OpticalSurface* dropletSurface = new G4OpticalSurface("DropletSurface");
    dropletSurface->SetType(dielectric_dielectric);
    dropletSurface->SetFinish(polished);
    dropletSurface->SetModel(unified);

    G4OpticalSurface* chipSurface = new G4OpticalSurface("DropletSurface");
    chipSurface->SetType(dielectric_dielectric);
    chipSurface->SetFinish(polished);
    chipSurface->SetModel(unified);

    new G4LogicalBorderSurface("DropletAirBorder", physDroplet, physWorld, dropletSurface);
    new G4LogicalBorderSurface("DropletChipBorder", physDroplet, physChip, dropletSurface);
    new G4LogicalBorderSurface("ChipAirBorder", physChip, physWorld, chipSurface);

    // //
    // // Converter Dome
    // //

    // auto mold = new G4Box("Mold", 0.5 * xyMold, 0.5 * xyMold, 0.5 * zMold);
    // auto ellipsoid_inverse = new G4Ellipsoid("Ellipsoid", xyInverse, xyInverse, zInverse);
    // G4ThreeVector ellipsoidShift(0, 0, 0.5 * -(zMold + zSite) * mm);  // adjust the Z shift to leave room for droplet

    // // Solid, logical, physical volume
    // auto solidConverter = new G4SubtractionSolid("solidConverter", mold, ellipsoid_inverse, nullptr, ellipsoidShift);
    // auto logicConverter = new G4LogicalVolume(solidConverter, PDMS, "logicalConverter");
    // auto physConverter = new G4PVPlacement(nullptr, G4ThreeVector(0., 0., zChip + 0.5 * zMold), logicConverter, "physConverter", logicWorld, false, 0, fCheckOverlaps);

    // // Converter color scheme
    // G4VisAttributes* converterVis = new G4VisAttributes(G4Colour(1.0, 0.8, 1.0, 0.1));  // R, G, B, opaqueness
    // converterVis->SetForceSolid(true);
    // logicConverter->SetVisAttributes(converterVis);


    // //
    // // Surface Properties
    // //

    // auto* pdmsSurf = new G4OpticalSurface("Converter_RoughSurface");
    // pdmsSurf->SetType(dielectric_dielectric);   // Converter ↔ air
    // pdmsSurf->SetModel(unified);                // micro-facet model
    // pdmsSurf->SetFinish(ground);                // makes it rough

    // auto pdmsSurface = new G4LogicalBorderSurface("Converter_to_Air_rough",
    //                         physConverter,          // 1st physical volume  (Converter)
    //                         physWorld,              // 2nd physical volume  (world air)
    //                         pdmsSurf);

    //
    // Cherenkov Detectors
    //

    auto solidDetector = new G4Box("solidDetector",
                                0.5 * xChip, 0.5 * yChip, 0.5 * dThickness);

    auto logicDetector = new G4LogicalVolume(solidDetector, air, "logicalDetector");

    auto physDetector = new G4PVPlacement(nullptr, G4ThreeVector(0., 0., 0.5 * dThickness + zMold + zChip + 0.0001), logicDetector, "physDetector", logicWorld, false, 0, fCheckOverlaps);


    // Chip color scheme
    G4VisAttributes* detectorVis = new G4VisAttributes(G4Colour(1.0, 0.55, 0.0, 0.35));  // R, G, B, opaqueness
    detectorVis->SetForceSolid(true);
    logicDetector->SetVisAttributes(detectorVis);

    return physWorld;
}

void DetectorConstruction::ConstructSDandField()
{
    auto sensDet = new SensitiveDetector("SensitiveDetector");
    G4SDManager::GetSDMpointer()->AddNewDetector(sensDet);
    SetSensitiveDetector("logicalDetector", sensDet);
}
