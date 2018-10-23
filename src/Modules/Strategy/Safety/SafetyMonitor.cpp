#include "SafetyMonitor.h"
#include "Core/Utils/Math.h"

SafetyMonitor::SafetyMonitor(SpellBook *spellBook)
    : InnerModule(spellBook)
{
    
}

void SafetyMonitor::OnStart()
{
    
}

void SafetyMonitor::Tick(float ellapsedTime, SensorValues &sensor)
{
	float ang[2] = {Rad2Deg(sensor.sensors[Sensors::InertialSensor_AngleX]),
	               Rad2Deg(sensor.sensors[Sensors::InertialSensor_AngleY])};

	spellBook->strategy.FallenBack = false;
    spellBook->strategy.FallenFront = false;
    spellBook->strategy.TurnOver = false;
    spellBook->strategy.Die = false;

	if(ang[1] < -FALLEN_ANG) 
    {
        spellBook->strategy.FallenBack = true;
        spellBook->behaviour.Fallen = true;
	}
    else if(ang[1] > FALLEN_ANG) 
    {
        spellBook->strategy.FallenFront = true;
        spellBook->behaviour.Fallen = true;
	} 
    else if (ang[0] > FALLEN_ANG || ang[0] < -FALLEN_ANG) 
    {
        spellBook->strategy.TurnOver = true;
	} 
    else if (ABS(ang[0]) > FALLING_ANG || ABS(ang[1]) > FALLING_ANG) 
    {
        spellBook->strategy.Die = true;
        spellBook->behaviour.Die = true;
	}
}

void SafetyMonitor::OnStop()
{
    
}

SafetyMonitor::~SafetyMonitor()
{
    
}