#include "Localizer.h"

#include "Core/InitManager.h"
#include "Core/Utils/Math.h"

Localizer::Localizer(SpellBook *spellBook)
    :   InnerModule(spellBook)
{
       
}

void Localizer::Tick(float ellapsedTime, CameraFrame &top, CameraFrame &bottom, cv::Mat &combinedImage)
{
    spellBook->perception.vision.BGR = true;
    spellBook->perception.vision.HSV = true;
    
    Blackboard *blackboard = InitManager::GetBlackboard();
    const ActionCommand::All active = readFrom(motion, active);
    const Odometry &newOdometry = readFrom(motion, odometry);

    spellBook->perception.vision.localization.X += (newOdometry.forward/1000.0f) * ellapsedTime;
    spellBook->perception.vision.localization.Y += (newOdometry.left/1000.0f) * ellapsedTime;
    spellBook->perception.vision.localization.Theta += newOdometry.turn * ellapsedTime;

    //cout << "Localization: [" << spellBook->perception.vision.localization.X << ", " << spellBook->perception.vision.localization.Y << ", " << Rad2Deg(spellBook->perception.vision.localization.Theta) << "º]" << endl;
}
