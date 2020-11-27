#include "maes-rtos.h"

/*    Author: Daniel Rojas Marín    */

namespace MAES
{
	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: Org constructor                                                 *
	****************************************************************************/
	Agent_Organization::Agent_Organization(ORG_TYPE type)
	{
		ptr_env = &env;
		description.org_type = type;
		description.members_num = 0;
		description.banned_num = 0;

		for (UBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
		{
			description.members[i] = NULL;
			description.banned[i] = NULL;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: isMember                                                        *
	****************************************************************************/
	ERROR_CODE Agent_Organization::isMember(Agent_AID aid)
	{
		for (UBaseType_t i = 0; i < description.members_num; i++)
		{
			if (description.members[i] == aid)
			{
				return FOUND;
			}
		}
		return NOT_FOUND;
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: isBanned                                                        *
	****************************************************************************/
	ERROR_CODE Agent_Organization::isBanned(Agent_AID aid)
	{
		for (UBaseType_t i = 0; i < description.banned_num; i++)
		{
			if (description.banned[i] == aid)
			{
				return FOUND;
			}
		}
		return NOT_FOUND;
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: Create                                                          *
	****************************************************************************/
	ERROR_CODE Agent_Organization::create()
	{
		if (xTaskGetCurrentTaskHandle() == NULL)
		{
			return INVALID;
		}
		else if (description.owner == NULL)
		{
			Agent* agent = (Agent*)ptr_env->get_TaskEnv(xTaskGetCurrentTaskHandle());

			if (agent->agent.AP != NULL)
			{
				agent->agent.role = PARTICIPANT;
				agent->agent.affiliation = OWNER;
				agent->agent.org = &description;
				description.owner = xTaskGetCurrentTaskHandle();
				description.members[description.members_num] = description.owner;
				description.members_num++;
				return NO_ERRORS;
			}
			else
			{
				return NOT_REGISTERED;
			}
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: add Agent                                                       *
	****************************************************************************/
	ERROR_CODE Agent_Organization::add_agent(Agent_AID aid)
	{
		Agent* agent = (Agent*)ptr_env->get_TaskEnv(aid);

		if (description.members_num == AGENT_LIST_SIZE)
			return LIST_FULL;
		else if (agent->agent.org != NULL || isBanned(aid) == FOUND || agent->agent.AP == NULL)
		{
			return INVALID;
		}
		else if (isMember(aid) == FOUND)
		{
			return DUPLICATED;
		}
		else if (description.owner == xTaskGetCurrentTaskHandle() || description.admin == xTaskGetCurrentTaskHandle())
		{
			description.members[description.members_num] = aid;
			description.members_num++;
			agent->agent.affiliation = MEMBER;
			agent->agent.role = PARTICIPANT;
			agent->agent.org = &description;
			return NO_ERRORS;
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: kick Agent                                                      *
	****************************************************************************/
	ERROR_CODE Agent_Organization::kick_agent(Agent_AID aid)
	{
		if (aid == xTaskGetCurrentTaskHandle())
			return INVALID;
		else if (description.owner == xTaskGetCurrentTaskHandle() || description.admin == xTaskGetCurrentTaskHandle())
		{
			UBaseType_t i = 0;
			Agent* agent = (Agent*)ptr_env->get_TaskEnv(aid);
			while (i < AGENT_LIST_SIZE)
			{
				if (description.members[i] == aid)
				{
					while (i < AGENT_LIST_SIZE - 1)
					{
						description.members[i] = description.members[i + 1];
						i++;
					}
					description.members[AGENT_LIST_SIZE - 1] = NULL;
					description.members_num--;
					agent->agent.role = NONE;
					agent->agent.affiliation = NON_MEMBER;
					agent->agent.org = NULL;
					break;
				}
				i++;
			}
			if (i == AGENT_LIST_SIZE)
			{
				return NOT_FOUND;
			}

			else
			{
				return NO_ERRORS;
			}
		}

		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: destroy                                                         *
	****************************************************************************/
	ERROR_CODE Agent_Organization::destroy()
	{
		if (description.owner == xTaskGetCurrentTaskHandle())
		{
			Agent* agent;
			for (UBaseType_t i = 0; i < description.members_num; i++)
			{
				agent = (Agent*)ptr_env->get_TaskEnv(description.members[i]);
				agent->agent.org = NULL;
				agent->agent.affiliation = NON_MEMBER;
				agent->agent.role = NONE;
				description.members[i] = NULL;
			}
			for (UBaseType_t i = 0; i < description.banned_num; i++)
			{
				description.banned[i] = NULL;
			}
			return NO_ERRORS;
		}

		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: change owner                                                    *
	****************************************************************************/
	ERROR_CODE Agent_Organization::change_owner(Agent_AID aid)
	{
		if (description.owner == xTaskGetCurrentTaskHandle() && isMember(aid) == FOUND)
		{
			Agent* agent = (Agent*)ptr_env->get_TaskEnv(description.owner);
			agent->agent.affiliation = MEMBER;
			agent->agent.role = VISITOR;
			agent = (Agent*)ptr_env->get_TaskEnv(aid);
			agent->agent.affiliation = OWNER;
			agent->agent.role = PARTICIPANT;
			description.owner = aid;
			return NO_ERRORS;
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: set admin                                                       *
	****************************************************************************/
	ERROR_CODE Agent_Organization::set_admin(Agent_AID aid)
	{
		if (description.owner == xTaskGetCurrentTaskHandle() && isMember(aid) == FOUND)
		{
			Agent* agent = (Agent*)ptr_env->get_TaskEnv(aid);
			if (agent->agent.affiliation != OWNER)
				agent->agent.affiliation = ADMIN;
			agent->agent.role = PARTICIPANT;
			description.admin = aid;
			return NO_ERRORS;
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: set moderator                                                   *
	****************************************************************************/
	ERROR_CODE Agent_Organization::set_moderator(Agent_AID aid)
	{
		if (description.owner == xTaskGetCurrentTaskHandle() && isMember(aid) == FOUND)
		{
			Agent* agent = (Agent*)ptr_env->get_TaskEnv(aid);
			agent->agent.role = MODERATOR;
			description.moderator = aid;
			return NO_ERRORS;
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: ban agent                                                       *
	****************************************************************************/
	ERROR_CODE Agent_Organization::ban_agent(Agent_AID aid)
	{
		if (description.banned_num == AGENT_LIST_SIZE)
		{
			return LIST_FULL;
		}

		else if (isBanned(aid) == FOUND)
		{
			return DUPLICATED;
		}

		else if (description.owner == xTaskGetCurrentTaskHandle() || description.admin == xTaskGetCurrentTaskHandle())
		{
			description.banned[description.banned_num] = aid;
			description.banned_num++;
			if (isMember(aid) == FOUND)
				kick_agent(aid);
			return NO_ERRORS;
		}

		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: remove ban                                                      *
	****************************************************************************/
	ERROR_CODE Agent_Organization::remove_ban(Agent_AID aid)
	{
		if (description.owner == xTaskGetCurrentTaskHandle() || description.admin == xTaskGetCurrentTaskHandle())
		{
			UBaseType_t i = 0;
			while (i < AGENT_LIST_SIZE)
			{
				if (description.banned[i] == aid)
				{
					while (i < AGENT_LIST_SIZE - 1)
					{
						description.banned[i] = description.banned[i + 1];
						i++;
					}
					description.banned[AGENT_LIST_SIZE - 1] = NULL;
					description.banned_num--;
					break;
				}
				i++;
			}

			if (i == AGENT_LIST_SIZE)
			{
				return NOT_FOUND;
			}

			else
			{
				return NO_ERRORS;
			}
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: clear ban list                                                  *
	****************************************************************************/
	void Agent_Organization::clear_ban_list()
	{
		if (description.owner == xTaskGetCurrentTaskHandle() || description.admin == xTaskGetCurrentTaskHandle())
		{
			for (UBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
			{
				description.banned[i] = NULL;
			}
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: set participant                                                 *
	****************************************************************************/
	ERROR_CODE Agent_Organization::set_participant(Agent_AID aid)
	{
		if ((description.owner == xTaskGetCurrentTaskHandle() || description.moderator == xTaskGetCurrentTaskHandle()) && isMember(aid) == FOUND)
		{
			Agent* agent = (Agent*)ptr_env->get_TaskEnv(aid);
			agent->agent.role = PARTICIPANT;
			return NO_ERRORS;
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: set visitor                                                     *
	****************************************************************************/
	ERROR_CODE Agent_Organization::set_visitor(Agent_AID aid)
	{
		if ((description.owner == xTaskGetCurrentTaskHandle() || description.moderator == xTaskGetCurrentTaskHandle()) && isMember(aid) == FOUND)
		{
			Agent* agent = (Agent*)ptr_env->get_TaskEnv(aid);
			agent->agent.role = VISITOR;
			return NO_ERRORS;
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: get size                                                        *
	****************************************************************************/
	UBaseType_t Agent_Organization::get_size()
	{
		return description.members_num;
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: get info                                                        *
	****************************************************************************/
	org_info Agent_Organization::get_info()
	{
		return description;
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: get org type                                                    *
	****************************************************************************/
	ORG_TYPE Agent_Organization::get_org_type()
	{
		return description.org_type;
	}

	/****************************************************************************
	* Class: Agent Organization                                                 *
	* Function: invite                                                          *
	****************************************************************************/
	MSG_TYPE Agent_Organization::invite(Agent_Msg msg, UBaseType_t password, Agent_AID target_agent, UBaseType_t timeout)
	{
		Agent* caller = (Agent*)ptr_env->get_TaskEnv(xTaskGetCurrentTaskHandle());

		if (caller->agent.affiliation == OWNER || caller->agent.affiliation == ADMIN)
		{
			msg.set_msg_type(PROPOSE);
			msg.set_msg_content("Join Organization");
			// msg.set_msg_int(password);
			msg.send(target_agent, timeout);

			msg.receive(timeout);

			if (msg.get_msg_type() == ACCEPT_PROPOSAL)
			{
				add_agent(target_agent);
			}
		}

		else
		{
			msg.set_msg_type(REFUSE);
		}
		return msg.get_msg_type();
	}
} // namespace MAES