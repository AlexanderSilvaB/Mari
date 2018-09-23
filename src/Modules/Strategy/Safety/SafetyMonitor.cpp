#include "SafetyMonitor.h"

SafetyMonitor::SafetyMonitor(SpellBook *spellBook)
{
    this->spellBook = spellBook;
}

void SafetyMonitor::OnStart()
{
    
}

void SafetyMonitor::Tick(float ellapsedTime, SensorValues &sensor)
{
	float ang[2] = {Rad2Deg(sensor.sensors[Sensors::InertialSensor_AngleX]),
	               Rad2Deg(sensor.sensors[Sensors::InertialSensor_AngleY])};

	spellBook->strategySpell.FallenBack = false;
    spellBook->strategySpell.FallenFront = false;
    spellBook->strategySpell.TurnOver = false;
    spellBook->strategySpell.Die = false;

	if(ang[1] < -FALLEN_ANG) 
    {
        spellBook->strategySpell.FallenBack = true;
	}
    else if(ang[1] > FALLEN_ANG) 
    {
        spellBook->strategySpell.FallenFront = true;
	} 
    else if (ang[0] > FALLEN_ANG || ang[0] < -FALLEN_ANG) 
    {
        spellBook->strategySpell.TurnOver = true;
	} 
    else if (ABS(ang[0]) > FALLING_ANG || ABS(ang[1]) > FALLING_ANG) 
    {
        spellBook->strategySpell.Die = true;
	}
}

void SafetyMonitor::OnStop()
{
    
}

SafetyMonitor::~SafetyMonitor()
{
    
}