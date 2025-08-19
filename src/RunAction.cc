#include "RunAction.hh"
#include "SteppingAction.hh"

#include "G4AnalysisManager.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "globals.hh"
#include <fstream>
#include <sstream>

RunAction::RunAction()
{
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetNtupleMerging(true);

    // analysisManager->CreateNtuple("TopFace", "KE");
    // analysisManager->CreateNtupleDColumn("fKE");
    // analysisManager->FinishNtuple(0);

    // analysisManager->CreateH1("CherenkovEnergy",
    //               "Cherenkov photons;E_{photon} [eV];Counts",
    //               100,                    // #bins
    //               1.0*eV, 6.0*eV);        // min, max

    const G4double λmin = 200*nm, λmax = 1240*nm;     // 6 eV ↔ 1 eV
    
    // Histograms disabled for escape efficiency only analysis

    analysisManager->CreateNtuple("TopFace", "KE");
    analysisManager->CreateNtupleDColumn("fWavelength");
    analysisManager->FinishNtuple(0);

    // analysisManager->CreateH1("edep", "Energy Deposition", 100, 0., 2.5 * MeV);
    // analysisManager->CreateH1("tracklength", "Track Length", 100, 0., 100. * mm);

}

std::string RunAction::GenerateUniqueFilename(const std::string& baseFilename) {
    std::string baseName = baseFilename.substr(0, baseFilename.find_last_of('.'));
    std::string extension = baseFilename.substr(baseFilename.find_last_of('.'));
    
    int counter = 1;
    std::string newFilename;
    
    do {
        std::ostringstream oss;
        oss << baseName << "_" << counter << extension;
        newFilename = oss.str();
        counter++;
        std::ifstream file(newFilename);
        if (!file.good()) {
            break;
        }
    } while (true);
    
    return newFilename;
}

void RunAction::BeginOfRunAction(const G4Run*)
{
    auto analysisManager = G4AnalysisManager::Instance();

    std::string filename = GenerateUniqueFilename("root_output/output_G8.root");
    analysisManager->OpenFile(filename);
}

void RunAction::EndOfRunAction(const G4Run*)
{
    auto analysisManager = G4AnalysisManager::Instance();

    // Print final photon statistics
    SteppingAction::PrintFinalStatistics();

    analysisManager->Write();
    analysisManager->CloseFile();
}