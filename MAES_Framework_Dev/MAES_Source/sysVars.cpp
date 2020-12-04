#include "maes-rtos.h"

/*    Author: Daniel Rojas Marín    */
namespace MAES
{
	Agent* sysVars::get_TaskEnv(Agent_AID aid)
	{
		return environment[aid];
	}

	void  sysVars::set_TaskEnv(Agent_AID aid, Agent* agent_ptr)
	{
		environment.insert(make_pair(aid, agent_ptr));
	}

	void  sysVars::erase_TaskEnv(Agent_AID aid)
	{
		environment.erase(aid);
	}

	map <TaskHandle_t, Agent*>  sysVars::getEnv()
	{
		return environment;
	}

	sysVars env;
}