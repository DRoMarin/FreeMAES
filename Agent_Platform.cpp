#include "maes-rtos.h"

/*    Author: Daniel Rojas Marín    */

namespace MAES
{
	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: Agent constructor                                               *
	****************************************************************************/
	Agent_Platform::Agent_Platform(char* name)
	{
		agentAMS.agent.agent_name = name;
		ptr_cond = &cond;
		subscribers = 0;
		agentAMS.agent.priority = configMAX_PRIORITIES - 1;
		for (UBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
		{
			Agent_Handle[i] = (Agent_AID)NULL;
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: Agent constructor with User Conditions                          *
	****************************************************************************/
	Agent_Platform::Agent_Platform(char* name, USER_DEF_COND* user_cond)
	{
		agentAMS.agent.agent_name = name;
		ptr_cond = user_cond;
		subscribers = 0;
		for (UBaseType_t i = 0; i < AGENT_LIST_SIZE; i++)
		{
			Agent_Handle[i] = (Agent_AID)NULL;
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: Agent Platform boot                                             *
	****************************************************************************/
	bool Agent_Platform::boot()
	{
		if (xTaskGetCurrentTaskHandle() == NULL)
		{
			AMSparameter* parametersForTask = &parameters;

			// Mailbox = Queue

			agentAMS.agent.mailbox_handle = xQueueCreate(1, sizeof(MSG_TYPE));

			// Task
			parametersForTask->services = this;
			parametersForTask->cond = ptr_cond;
			agentAMS.resources.stackSize = 1024;
			//#if configENABLE_MPU == 1

			//           StaticTask_t xTaskBuffer;
			//           StackType_t xStack[agentAMS.resources.stackSize];
			//           agentAMS.agent.aid = xTaskCreateStatic(AMS_task, agentAMS.agent.agent_name, agentAMS.resources.stackSize, (void *)&parameters, configMAX_PRIORITIES - 1, xStack, &xTaskBuffer);

			//#else

			xTaskCreate(AMS_task, agentAMS.agent.agent_name, agentAMS.resources.stackSize, (void*)&parametersForTask, (configMAX_PRIORITIES - 1), &agentAMS.agent.aid);

			//#endif
			systemVars* ptr_env = &env;
			ptr_env->set_TaskEnv(agentAMS.agent.aid, &agentAMS);

			if (agentAMS.agent.aid != NULL)
			{ //for (UBaseType_t i = 0; i < sizeof(tasksEnv); i++)
				for (auto const& [handle, agentPtr] : ptr_env->getEnv())
				{
					//auto handle = tasksEnv[i].first;
					//auto agentPtr = tasksEnv[i].first;

					if (handle == NULL || agentAMS.agent.aid == NULL)
					{
						break;
					}
					register_agent(handle);
				}
				return NO_ERRORS;
			}
			else
			{
				/* System_abort */
				return INVALID;
			}
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: Agent initialion                                                *
	****************************************************************************/
	void Agent_Platform::agent_init(Agent* a, void behaviour(void* pvParameters))
	{
		auto obs = xTaskGetCurrentTaskHandle();
		if (xTaskGetCurrentTaskHandle() == 0)
		{
			// Mailbox

			a->agent.mailbox_handle = xQueueCreate(1, sizeof(MSG_TYPE));

			// Task
			a->resources.function = behaviour;
			a->resources.taskParameters = NULL;

			//            #if configENABLE_MPU == 1
			//                StaticTask_t xTaskBuffer;
			//                StackType_t xStack[agentAMS.resources.stackSize];
			//                a.agent.aid = xTaskCreateStatic(behaviour, a.agent.agent_name, a.resources.stackSize, (void *)&parameters, 0, xStack, &xTaskBuffer);
			//            #else
			xTaskCreate(behaviour, a->agent.agent_name, a->resources.stackSize, a->resources.taskParameters, 0, &a->agent.aid);
			//            #endif
			systemVars* ptr_env = &env;
			ptr_env->set_TaskEnv(a->agent.aid, a);
			vTaskSuspend(a->agent.aid);
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: Agent initialion with user arguments                            *
	****************************************************************************/

	void Agent_Platform::agent_init(Agent* a, void behaviour(void* pvParameters), void* pvParameters)
	{
		if (xTaskGetCurrentTaskHandle() == 0)
		{
			// Mailbox = Queue

			a->agent.mailbox_handle = xQueueCreate(1, sizeof(MSG_TYPE));

			// Task
			a->resources.function = behaviour;
			a->resources.taskParameters = pvParameters;

			//            #if configENABLE_MPU == 1
			//                StaticTask_t xTaskBuffer;
			//                StackType_t xStack[agentAMS.resources.stackSize];
			//                a.agent.aid = xTaskCreateStatic(behaviour, a.agent.agent_name, a.resources.stackSize, (void *)&parameters, 0, xStack, &xTaskBuffer);
			//            #else
			xTaskCreate(behaviour, a->agent.agent_name, a->resources.stackSize, a->resources.taskParameters, 1, &a->agent.aid);
			//            #endif
			systemVars* ptr_env = &env;
			ptr_env->set_TaskEnv(a->agent.aid, a);
			vTaskSuspend(a->agent.aid);
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: Agent_search                                                    *
	****************************************************************************/
	bool Agent_Platform::agent_search(Agent_AID aid)
	{
		for (UBaseType_t i = 0; i < subscribers; i++)
		{
			if (Agent_Handle[i] == aid)
			{
				return true;
			}
		}
		return false;
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: Agent_wait                                                      *
	****************************************************************************/
	void Agent_Platform::agent_wait(TickType_t ticks)
	{
		vTaskDelay(ticks);
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: Agent_yield                                                     *
	****************************************************************************/
	void Agent_Platform::agent_yield()
	{
		taskYIELD();
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: get_running_agent                                               *
	****************************************************************************/
	Agent_AID Agent_Platform::get_running_agent()
	{
		return xTaskGetCurrentTaskHandle();
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: get_state                                                       *
	****************************************************************************/
	AGENT_MODE Agent_Platform::get_state(Agent_AID aid)
	{
		if (agent_search(aid))
		{
			eTaskState state;
			state = eTaskGetState(aid);
			switch (state)
			{
			case eReady:
				/* code */
				return ACTIVE;

			case eBlocked:
				/* code */
				return WAITING;

			case eSuspended:
				/* code */
				return SUSPENDED;

			case eDeleted:
				/* code */
				return TERMINATED;

			default:
				return NO_MODE;
			}
		}
		else
		{
			return NO_MODE;
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: get_Agent_description                                           *
	****************************************************************************/
	Agent_info Agent_Platform::get_Agent_description(Agent_AID aid)
	{
		Agent* description;
		description = env.get_TaskEnv(aid);

		return description->agent;
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: get_AP_description                                              *
	****************************************************************************/
	AP_Description Agent_Platform::get_AP_description()
	{
		AP_Description description;
		description.AMS_AID = agentAMS.agent.aid;
		description.AP_name = agentAMS.agent.agent_name;
		description.subscribers = subscribers;
		return description;
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: register_agent                                                  *
	****************************************************************************/
	ERROR_CODE Agent_Platform::register_agent(Agent_AID aid)
	{
		if (aid == NULL)
		{
			return HANDLE_NULL;
		}
		else if (xTaskGetCurrentTaskHandle() == NULL || uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
		{
			if (!agent_search(aid))
			{
				if (subscribers < AGENT_LIST_SIZE)
				{
					Agent* description;
					description = env.get_TaskEnv(aid);
					description->agent.AP = agentAMS.agent.aid;
					Agent_Handle[subscribers] = aid;
					subscribers++;
					vTaskPrioritySet(aid, description->agent.priority);
					vTaskResume(aid);
					auto watcher = uxTaskPriorityGet(aid);
					return NO_ERRORS;
				}
				else
				{
					return LIST_FULL;
				}
			}
			else
			{
				return DUPLICATED;
			}
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: deregister_agent                                                *
	****************************************************************************/
	ERROR_CODE Agent_Platform::deregister_agent(Agent_AID aid)
	{
		if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
		{
			UBaseType_t i = 0;
			while (i < AGENT_LIST_SIZE)
			{
				if (Agent_Handle[i] == aid)
				{
					Agent* description;
					suspend_agent(aid);
					description = (Agent*)env.get_TaskEnv(aid);
					description->agent.AP = NULL;

					while (i < AGENT_LIST_SIZE - 1)
					{
						Agent_Handle[i] = Agent_Handle[i + 1];
						i++;
					}
					Agent_Handle[AGENT_LIST_SIZE - 1] = NULL;
					subscribers--;
					break;
				}
				i++;
			}
			if (i == AGENT_LIST_SIZE)
			{
				return NOT_FOUND;
			}
		}
		else
		{
			return NO_ERRORS;
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: kill_agent                                                      *
	****************************************************************************/
	ERROR_CODE Agent_Platform::kill_agent(Agent_AID aid)
	{
		if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
		{
			ERROR_CODE error;
			error = deregister_agent(aid);

			if (error == NO_ERRORS)
			{
				Agent* description;
				Mailbox_Handle m;

				description = (Agent*)env.get_TaskEnv(aid);
				description->agent.aid = NULL;

				m = description->agent.mailbox_handle;
				vQueueDelete(m);
				vTaskDelete(aid);
				systemVars* ptr_env = &env;
				ptr_env->erase_TaskEnv(aid);
			}
			return error;
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: suspend_agent                                                   *
	****************************************************************************/
	ERROR_CODE Agent_Platform::suspend_agent(Agent_AID aid)
	{
		if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
		{
			if (agent_search(aid))
			{
				vTaskSuspend(aid);
				return (NO_ERRORS); /* code */
			}
			else
			{
				return NOT_FOUND;
			}
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: resume_agent                                                    *
	****************************************************************************/
	ERROR_CODE Agent_Platform::resume_agent(Agent_AID aid)
	{
		if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
		{
			if (agent_search(aid))
			{
				Agent* description;
				description = (Agent*)env.get_TaskEnv(aid);
				vTaskResume(aid);
				vTaskPrioritySet(aid, description->agent.priority);
				return NO_ERRORS;
			}
			else
			{
				return NOT_FOUND;
			}
		}
		else
		{
			return INVALID;
		}
	}

	/****************************************************************************
	* Class: Agent Platform                                                     *
	* Function: resume_agent                                                    *
	****************************************************************************/
	void Agent_Platform::restart(Agent_AID aid)
	{
		if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
		{
			Agent* a;
			a = (Agent*)env.get_TaskEnv(aid);
			Mailbox_Handle m;

			// delete Task and Mailbox

			m = a->agent.mailbox_handle;
			vTaskDelete(aid);
			vQueueDelete(m);
			systemVars* ptr_env = &env;
			ptr_env->erase_TaskEnv(aid);

			// Mailbox = Queue

			a->agent.mailbox_handle = xQueueCreate(1, sizeof(MSG_TYPE));

			// Task

#if configENABLE_MPU == 1
			{
				StaticTask_t xTaskBuffer;
				StackType_t xStack[agentAMS.resources.stackSize];
				a.agent.aid = xTaskCreateStatic(a->resources.function, a->agent.agent_name, a->resources.stackSize, a->resources.taskParameters, 0, xStack, &xTaskBuffer);
			}
#else
			{
				xTaskCreate(a->resources.function, a->agent.agent_name, a->resources.stackSize, a->resources.taskParameters, 0, &a->agent.aid);
			}
#endif

			xTaskCreate(a->resources.function, a->agent.agent_name, a->resources.stackSize, a->resources.taskParameters, 0, &a->agent.aid);
			ptr_env->set_TaskEnv(a->agent.aid, a);
		}
	}
	/***********************      AMS task overload      ***********************/
	namespace
	{
		void AMS_task(void* pvParameters)
		{
			AMSparameter* amsParameters = &parameters;
			Agent_Platform* services = amsParameters->services;
			USER_DEF_COND* cond = amsParameters->cond;

			Agent_Msg msg;

			UBaseType_t error_msg = 0;
			while (1)
			{
				msg.receive(portMAX_DELAY);
				if (msg.get_msg_type() == REQUEST)
				{
					if (strcmp(msg.get_msg_content(), "KILL") == 0)
					{
						if (cond->kill_cond())
						{
							error_msg = services->kill_agent(msg.get_target_agent());
							if (error_msg == NO_ERRORS)
							{
								msg.set_msg_type(CONFIRM);
							}
							else
							{
								msg.set_msg_type(REFUSE);
							}
						}
						else
						{
							msg.set_msg_type(REFUSE);
						}
						msg.send(msg.get_sender(), 0);
					} //KILL Case

					else if (strcmp(msg.get_msg_content(), "REGISTER") == 0)
					{
						if (cond->register_cond())
						{
							error_msg = services->register_agent(msg.get_target_agent());
							if (error_msg == NO_ERRORS)
							{
								msg.set_msg_type(CONFIRM);
							}
							else
							{
								msg.set_msg_type(REFUSE);
							}
						}
						else
						{
							msg.set_msg_type(REFUSE);
						}
						msg.send(msg.get_sender(), 0);
					} //REGISTER Case

					else if (strcmp(msg.get_msg_content(), "DEREGISTER") == 0)
					{
						if (cond->deregister_cond())
						{
							error_msg = services->deregister_agent(msg.get_target_agent());
							if (error_msg == NO_ERRORS)
							{
								msg.set_msg_type(CONFIRM);
							}
							else
							{
								msg.set_msg_type(REFUSE);
							}
						}
						else
						{
							msg.set_msg_type(REFUSE);
						}
						msg.send(msg.get_sender(), 0);
					} //DEREGISTER Case

					else if (strcmp(msg.get_msg_content(), "SUSPEND") == 0)
					{
						if (cond->suspend_cond())
						{
							error_msg = services->suspend_agent(msg.get_target_agent());
							if (error_msg == NO_ERRORS)
							{
								msg.set_msg_type(CONFIRM);
							}
							else
							{
								msg.set_msg_type(REFUSE);
							}
						}
						else
						{
							msg.set_msg_type(REFUSE);
						}
						msg.send(msg.get_sender(), 0);
					} //SUSPEND Case

					else if (strcmp(msg.get_msg_content(), "RESUME") == 0)
					{
						if (cond->resume_cond())
						{
							error_msg = services->resume_agent(msg.get_target_agent());
							if (error_msg == NO_ERRORS)
							{
								msg.set_msg_type(CONFIRM);
							}
							else
							{
								msg.set_msg_type(REFUSE);
							}
						}
						else
						{
							msg.set_msg_type(REFUSE);
						}
						msg.send(msg.get_sender(), 0);
					} //RESUME Case

					else if (strcmp(msg.get_msg_content(), "RESTART") == 0)
					{
						if (cond->restart_cond())
						{
							services->restart(msg.get_target_agent());
						}
						else
						{
							msg.set_msg_type(REFUSE);
						}
						msg.send(msg.get_sender(), 0);
					} //RESTART Case

					else
					{
						msg.set_msg_type(NOT_UNDERSTOOD);
						msg.send(msg.get_sender(), 0);
					}
				} //end if
				else
				{
					msg.set_msg_type(NOT_UNDERSTOOD);
					msg.send(msg.get_sender(), 0);
				}
			} // end while
		}
	} // namespace
} // namespace MAES