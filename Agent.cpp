#include "maes-rtos.h"
#include <iostream>
#include <map>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/*    Author: Daniel Rojas Marín    */

namespace MAES
{
	/****************************************************************************
	* Class: Agent                                                              *
	* Function: Agent constructor                                               *
	****************************************************************************/
	Agent::Agent(char* name, UBaseType_t pri, uint16_t sizeStack)
	{
		if (agent.priority >= (UBaseType_t)(configMAX_PRIORITIES - 1))
			agent.priority = configMAX_PRIORITIES - 2;
		agent.aid = NULL;
		agent.mailbox_handle = NULL;
		agent.agent_name = name;
		agent.priority = pri;
		resources.stackSize = sizeStack;
		agent.AP = NULL;
		agent.org = NULL;
		agent.affiliation = NON_MEMBER;
		agent.role = NONE;
	}

	/****************************************************************************
	* Class: Agent                                                              *
	* Function: Default Agent constructor                                       *
	****************************************************************************/
	Agent::Agent()
	{
	}

	/****************************************************************************
	* Class: Agent                                                              *
	* Function: get Agent's aid                                                 *
	****************************************************************************/
	Agent_AID Agent::AID()
	{
		return agent.aid;
	}
} // namespace MAES