#include "maes-rtos.h"

/*    Author: Daniel Rojas Marín    */
namespace MAES
{
	Agent* sysVars::get_TaskEnv(Agent_AID aid)
	{
		UBaseType_t i = 0;
		while (i < AGENT_LIST_SIZE)
		{
			if (environment[i].first == aid)
			{
				return environment[i].second;
			}
			i++;
		}
		return NULL;		
		//return environment[aid];
	}

	void  sysVars::set_TaskEnv(Agent_AID aid, Agent* agent_ptr)
	{
		UBaseType_t i = 0;
		while (i < AGENT_LIST_SIZE)
		{
			if (environment[i].first == NULL)
			{
				environment[i].first = aid;
				environment[i].second = agent_ptr;
				break;
			}
			i++;
		}
		//environment.insert(make_pair(aid, agent_ptr));
	}

	void  sysVars::erase_TaskEnv(Agent_AID aid)
	{
		UBaseType_t i = 0;
		while (i < AGENT_LIST_SIZE)
		{
			if (environment[i].first == aid)
			{
				environment[i].first = NULL;
				environment[i].second = NULL;
				
				while (i < AGENT_LIST_SIZE - 1)
				{
					environment[i] = environment[i + 1];
					i++;
				}
				break;
			}
			i++;
		}
		//environment.erase(aid);
	}

	sysVar * sysVars::getEnv()
	{
		return environment;
	}

	sysVars env;
}