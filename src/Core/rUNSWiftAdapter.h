#ifndef _RUNSWIFTADAPTER_H_
#define _RUNSWIFTADAPTER_H_

#include <string>
#include "blackboard/Adapter.hpp"
#include "InitManager.h"

class rUNSWiftAdapter
{
    public:
        rUNSWiftAdapter()
        {
            blackboard = InitManager::GetBlackboard();
        }
        virtual ~rUNSWiftAdapter()
        {

        }
        virtual void Start()
        {

        }
        virtual void Stop()
        {

        }
        virtual void Tick()
        {

        }
        virtual void Tick(ActionCommand::All &request)
        {

        }
        virtual void ReadOptions(const boost::program_options::variables_map& config)
        {

        }
    protected:
        Blackboard* blackboard;
};

#endif
