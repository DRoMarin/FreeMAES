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
        subscribers = 0;
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
        subscribers = 0;
        for (int i = 0; i < AGENT_LIST_SIZE; i++)
        {
            Agent_Handle[i] = (Agent_AID)NULL;
            tasksEnv[Agent_Handle[i]] = NULL;
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
            xTaskCreate(AMS_task, agentAMS.agent.agent_name, agentAMS.resources.stackSize, (void *)&parameters, configMAX_PRIORITIES - 1, &agentAMS.agent.aid);
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

    /****************************************************************************
    * Class: Agent Platform                                                     *
    * Function: Agent initialion                                                *
    ****************************************************************************/
    void Agent_Platform::agent_init(Agent a, void behaviour(void *pvParameters))
    {
        if (xTaskGetCurrentTaskHandle() == NULL)
        {
            QueueHandle_t msgQueue;
            SemaphoreHandle_t msgSemaphore;

            // Mailbox = Queue + Semaphore

            msgQueue = xQueueCreate(1, sizeof(MSG_TYPE));
            msgSemaphore = xSemaphoreCreateBinary();
            a.agent.mailbox_handle = xQueueCreateSet(2);
            xQueueAddToSet(msgQueue, a.agent.mailbox_handle);
            xQueueAddToSet(msgSemaphore, a.agent.mailbox_handle);

            // Task

            xTaskCreate(behaviour, a.agent.agent_name, a.resources.stackSize, NULL, -1, &a.agent.aid);
            tasksEnv.insert(pair<TaskHandle_t, Agent *>(a.agent.aid, &a));
        }
    }

    /****************************************************************************
    * Class: Agent Platform                                                     *
    * Function: Agent initialion with user arguments                            *
    ****************************************************************************/

    void Agent_Platform::agent_init(Agent a, void behaviour(void *pvParameters), void *pvParameters)
    {
        if (xTaskGetCurrentTaskHandle() == NULL)
        {
            Agent_AID temp;
            QueueHandle_t msgQueue;
            SemaphoreHandle_t msgSemaphore;

            // Mailbox = Queue + Semaphore

            msgQueue = xQueueCreate(1, sizeof(MSG_TYPE));
            msgSemaphore = xSemaphoreCreateBinary();
            a.agent.mailbox_handle = xQueueCreateSet(2);
            xQueueAddToSet(msgQueue, a.agent.mailbox_handle);
            xQueueAddToSet(msgSemaphore, a.agent.mailbox_handle);

            // Task

            xTaskCreate(behaviour, a.agent.agent_name, a.resources.stackSize, pvParameters, -1, &a.agent.aid);
            tasksEnv.insert(pair<TaskHandle_t, Agent *>(a.agent.aid, &a));
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
        Agent *description;
        description = (Agent *)tasksEnv[aid];
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
                    Agent *description;
                    description = (Agent *)tasksEnv[aid];
                    description->agent.AP = agentAMS.agent.aid;
                    Agent_Handle[subscribers] = aid;
                    subscribers++;
                    vTaskPrioritySet(aid, description->agent.priority);
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
                    Agent *description;
                    suspend_agent(aid);
                    description = (Agent *)tasksEnv[aid];
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

            if (error = NO_ERRORS)
            {
                Agent *description;
                Mailbox_Handle m;

                description = (Agent *)tasksEnv[aid];
                description->agent.aid = NULL;

                m = description->agent.mailbox_handle;
                vQueueDelete(m);
                vTaskDelete(aid);
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
                Agent *description;
                description = (Agent *)tasksEnv[aid];
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
    void Agent_Platform::restart(Agent_AID aid, void behaviour(void *pvParameters))
    {
        if (uxTaskPriorityGet(xTaskGetCurrentTaskHandle()) == configMAX_PRIORITIES - 1)
        {
            Agent *a;
            a = (Agent *)tasksEnv[aid];
            Mailbox_Handle m;

            // delete Task and Mailbox

            m = a->agent.mailbox_handle;
            vTaskDelete(aid);
            vQueueDelete(m);

            QueueHandle_t msgQueue;
            SemaphoreHandle_t msgSemaphore;

            // Mailbox = Queue + Semaphore

            msgQueue = xQueueCreate(1, sizeof(MSG_TYPE));
            msgSemaphore = xSemaphoreCreateBinary();
            a->agent.mailbox_handle = xQueueCreateSet(2);
            xQueueAddToSet(msgQueue, a->agent.mailbox_handle);
            xQueueAddToSet(msgSemaphore, a->agent.mailbox_handle);

            // Task

            xTaskCreate(behaviour, a->agent.agent_name, a->resources.stackSize, NULL, -1, &a->agent.aid);
            tasksEnv.insert(pair<TaskHandle_t, Agent *>(a->agent.aid, a));
        }
    }

        namespace
    {

        void AMS_task(void *parameters){
            AMSparameter *amsParameters = (AMSparameter*) parameters;
            Agent_Platform service = amsParameters->services;
            USER_DEF_COND * cond = amsParameters->cond;
            Agent_Msg msg;
            
            UBaseType_t error_msg = 0;
            while (1)
            {
                //msg.receive();
            }
            
        }

    }
} // namespace MAES