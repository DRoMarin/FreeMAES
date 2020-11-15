#include "maes-rtos.h"

/*    Author: Daniel Rojas Marín    */

namespace MAES
{
    /****************************************************************************
    * Class: Agent Platform                                                     *
    * Function: Agent constructor                                               *
    ****************************************************************************/

    Agent_Platform::Agent_Platform(char *name)
    {
        agentAMS.agent.agent_name = name;
        ptr_cond = &cond;
        subbscribers = 0;
        for (int i = 0; i < AGENT_LIST_SIZE; i++)
        {
            Agent_Handle[i] = (Agent_AID)NULL;
            tasksEnv[Agent_Handle[i]] = NULL;
        }
    }

    /****************************************************************************
    * Class: Agent Platform                                                     *
    * Function: Agent constructor with User Conditions                          *
    ****************************************************************************/
    Agent_Platform::Agent_Platform(char *name, USER_DEF_COND *user_cond)
    {
        agentAMS.agent.agent_name = name;
        ptr_cond = user_cond;
        subbscribers = 0;
        for (int i = 0; i < AGENT_LIST_SIZE; i++)
        {
            Agent_Handle[i] = (Agent_AID)NULL;
            tasksEnv[Agent_Handle[i]] = NULL;
        }
    }

    /****************************************************************************
    * Class: Agent Platform                                                     *
    * Function: Agent constructor with User Conditions                          *
    ****************************************************************************/
    bool Agent_Platform::boot()
    {
        if (xTaskGetCurrentTaskHandle() == NULL)
        {
            Agent_AID temp;
            AMSparameter parameters;

            QueueHandle_t msgQueue;
            SemaphoreHandle_t msgSemaphore;

            // Mailbox = Queue + Semaphore

            msgQueue = xQueueCreate(1, sizeof(MSG_TYPE));
            msgSemaphore = xSemaphoreCreateBinary();
            agentAMS.agent.mailbox_handle = xQueueCreateSet(2);
            xQueueAddToSet(msgQueue, agentAMS.agent.mailbox_handle);
            xQueueAddToSet(msgSemaphore, agentAMS.agent.mailbox_handle);

            // Task
            parameters.services = this;
            parameters.cond = ptr_cond;
            xTaskCreate(AMS_task, agentAMS.agent.agent_name, agentAMS.resources.stackSize, (void *)&parameters, -1, &agentAMS.agent.aid);
            tasksEnv.insert(pair<TaskHandle_t, Agent *>(agentAMS.agent.aid, &agentAMS));

            if (agentAMS.agent.aid != NULL)
            {

                for (auto const &[handle, agentPtr] : tasksEnv)
                {
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

} // namespace MAES