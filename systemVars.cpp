#include "maes-rtos.h"

/*    Author: Daniel Rojas Marín    */
namespace MAES
{
	Agent* systemVars::get_TaskEnv(Agent_AID aid)
	{
		return environment[aid];
	}

	void  systemVars::set_TaskEnv(Agent_AID aid, Agent* agent_ptr)
	{
		environment.insert(make_pair(aid, agent_ptr));
	}

	void  systemVars::erase_TaskEnv(Agent_AID aid)
	{
		environment.erase(aid);
	}

	map <TaskHandle_t, Agent*>  systemVars::getEnv()
	{
		return environment;
	}

	systemVars env;
}