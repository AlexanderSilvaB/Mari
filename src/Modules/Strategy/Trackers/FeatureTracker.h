#ifndef _FEATURE_TRACKER_H_
#define _FEATURE_TRACKER_H_

#include "Core/Module.h"
#include "Eigen/Eigen"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/CartesianCoord.h"

class FeatureTracker : public InnerModule
{
    private:
        // Declare aqui as variáveis globais do módulo
    public:
        FeatureTracker(SpellBook *spellBook);
        ~FeatureTracker();

        void OnStart();
        void OnStop();
        void Tick(float ellapsedTime, const SensorValues &sensor);
};

#endif