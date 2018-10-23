#include "Localizer.h"

#include "Core/InitManager.h"
#include "Core/Utils/Math.h"

Localizer::Localizer(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
    
}

void Localizer::Tick(float ellapsedTime)
{
    Blackboard *blackboard = InitManager::GetBlackboard();
    const ActionCommand::All active = readFrom(motion, active);
    const Odometry &newOdometry = readFrom(motion, odometry);

    spellBook->perception.localization.X += newOdometry.forward * ellapsedTime;
    spellBook->perception.localization.Y += newOdometry.left * ellapsedTime;
    spellBook->perception.localization.Theta += newOdometry.turn * ellapsedTime;

    cout << "Localization: [" << spellBook->perception.localization.X << ", " << spellBook->perception.localization.Y << ", " << Rad2Deg(spellBook->perception.localization.Theta) << "º]" << endl;
}
