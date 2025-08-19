#ifndef RUNACTION_HH
#define RUNACTION_HH

#include "G4UserRunAction.hh"
#include <string>

class G4Run;

class RunAction : public G4UserRunAction
{
  public:
    RunAction();
    ~RunAction() override = default;

    void BeginOfRunAction(const G4Run*) override;
    void EndOfRunAction(const G4Run*) override;

  private:
    std::string GenerateUniqueFilename(const std::string& baseFilename);
};

#endif