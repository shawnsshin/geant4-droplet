#include "ActionInitialization.hh"

#include "PrimaryGenerator.hh"
#include "RunAction.hh"
#include "StackingAction.hh"
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

ActionInitialization::ActionInitialization()
{}

void ActionInitialization::BuildForMaster() const
{
    SetUserAction(new RunAction);
}

void ActionInitialization::Build() const
{
    PrimaryGenerator *generator = new PrimaryGenerator();
    SetUserAction(generator);

    SetUserAction(new StackingAction);

    RunAction *runaction = new RunAction();
    SetUserAction(runaction);

    SetUserAction(new SteppingAction());

    // SetUserAction(new PrintEveryProcess);
}