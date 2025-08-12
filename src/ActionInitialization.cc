#include "/home/sshin/dev/droplet/opt_droplet_sim/include/ActionInitialization.hh"

#include "/home/sshin/dev/droplet/opt_droplet_sim/include/PrimaryGenerator.hh"
#include "/home/sshin/dev/droplet/opt_droplet_sim/include/RunAction.hh"
#include "/home/sshin/dev/droplet/opt_droplet_sim/include/StackingAction.hh"
#include "/home/sshin/dev/droplet/opt_droplet_sim/include/SteppingAction.hh"
#include "/home/sshin/dev/droplet/opt_droplet_sim/include/DetectorConstruction.hh"

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