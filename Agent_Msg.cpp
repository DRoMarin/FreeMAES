#include "maes-rtos.h"

/*    Author: Daniel Rojas Marín    */

namespace MAES
{
    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: Agent constructor                                               *
    ****************************************************************************/
    Agent_Msg::Agent_Msg()
    {
        caller = xTaskGetCurrentTaskHandle();
        clear_all_receiver();
        subscribers = 0;
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: isRegistered                                                    *
    ****************************************************************************/
    bool Agent_Msg::isRegistered(Agent_AID aid)
    {
        Agent *description_receiver = (Agent *)tasksEnv[aid];
        Agent *description_sender = (Agent *)tasksEnv[caller];

        if (description_receiver->agent.AP == description_sender->agent.AP)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: get_mailbox                                                     *
    ****************************************************************************/
    Mailbox_Handle Agent_Msg::get_mailbox(Agent_AID aid)
    {
        Agent *description;
        description = (Agent *)tasksEnv[aid];
        return description->agent.mailbox_handle;
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: add_receiver                                                    *
    ****************************************************************************/
    ERROR_CODE Agent_Msg::add_receiver(Agent_AID aid_receiver)
    {
        if (isRegistered(aid_receiver))
        {
            if (aid_receiver == NULL)
            {
                return HANDLE_NULL;
            }
            if (subscribers < MAX_RECEIVERS)
            {
                receivers[subscribers] = aid_receiver;
                subscribers++;
                return NO_ERRORS;
            }
            else
            {
                return LIST_FULL;
            }
        }
        else
        {
            return NOT_FOUND;
        }
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: remove_receiver                                                 *
    ****************************************************************************/
    ERROR_CODE Agent_Msg::remove_receiver(Agent_AID aid)
    {
        UBaseType_t i = 0;
        while (i < MAX_RECEIVERS)
        {
            if (receivers[i] == aid)
            {
                while (i < MAX_RECEIVERS - 1)
                {
                    receivers[i] = receivers[i + 1];
                    i++;
                }
                receivers[MAX_RECEIVERS - 1] = NULL;
                subscribers--;
                break;
            }
            i++;
        }
        if (i == MAX_RECEIVERS)
        {
            return NOT_FOUND;
        }
        else
        {
            return NO_ERRORS;
        }
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: clear_all_receivers                                             *
    ****************************************************************************/
    void Agent_Msg::clear_all_receiver()
    {
        UBaseType_t i = 0;
        while (i < MAX_RECEIVERS)
        {
            receivers[i] = NULL;
            i++;
        }
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: refersh_list                                                    *
    ****************************************************************************/
    void Agent_Msg::refresh_list()
    {
        Agent *agent_caller, *agent_receiver;
        agent_caller = (Agent *)tasksEnv[caller];

        for (UBaseType_t i = 0; i < subscribers; i++)
        {
            agent_receiver = (Agent *)tasksEnv[receivers[i]];
            if (isRegistered(receivers[i]) || agent_caller->agent.org != agent_receiver->agent.org)
            {
                remove_receiver(receivers[i]);
            }
        }
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: receive                                                         *
    ****************************************************************************/
    MSG_TYPE Agent_Msg::receive(TickType_t timeout)
    {
        Agent *a = tasksEnv[caller];

        if (xQueueReceive(get_mailbox(caller), &msg.type, timeout) != pdPASS)
        {
            return NO_RESPONSE;
        }
        else
        {
            return msg.type;
        }
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: send                                                            *
    ****************************************************************************/
    ERROR_CODE Agent_Msg::send(Agent_AID aid_receiver, TickType_t timeout)
    {
        msg.target_agent = aid_receiver;
        msg.sender_agent = caller;

        Agent *agent_caller, *agent_receiver;
        agent_caller = (Agent *)tasksEnv[caller];
        agent_receiver = (Agent *)tasksEnv[aid_receiver];

        if (!isRegistered(aid_receiver))
        {
            return NOT_REGISTERED;
        }
        else
        {
            if (agent_caller->agent.org == NULL && agent_receiver->agent.org == NULL)
            {
                if (xQueueSend(get_mailbox(aid_receiver), &msg.type, timeout) != pdPASS)
                {
                    return TIMEOUT;
                }
                else
                {
                    return NO_ERRORS;
                }
            }
            else if (agent_caller->agent.org == agent_receiver->agent.org)
            {
                if (agent_caller->agent.org->org_type == TEAM && agent_caller->agent.role == PARTICIPANT || (agent_caller->agent.org->org_type == HIERARCHY && agent_receiver->agent.role == MODERATOR))
                {
                    if (xQueueSend(get_mailbox(aid_receiver), &msg.type, timeout) != pdPASS)
                    {
                        return TIMEOUT;
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
            else if (agent_caller->agent.affiliation == ADMIN || agent_caller->agent.role == OWNER)
            {
                if (xQueueSend(get_mailbox(aid_receiver), &msg.type, timeout) != pdPASS)
                {
                    return TIMEOUT;
                }
                else
                {
                    return NO_ERRORS;
                }
            }
            else
            {
                return NOT_REGISTERED;
            }
        }
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: send                                                            *
    ****************************************************************************/
    ERROR_CODE Agent_Msg::send()
    {
        UBaseType_t i = 0;
        ERROR_CODE error_code;
        ERROR_CODE error = NO_ERRORS;

        while (receivers[i] != NULL)
        {
            error_code = send(receivers[i], portMAX_DELAY);
            if (error_code != NO_ERRORS)
            {
                error = error_code;
            }
            i++;
        }
        return error;
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: set_msg_type                                                    *
    ****************************************************************************/
    void Agent_Msg::set_msg_type(MSG_TYPE msg_type)
    {
        msg.type = msg_type;
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: set_msg_content                                                 *
    ****************************************************************************/
    void Agent_Msg::set_msg_content(char *msg_content)
    {
        msg.content = msg_content;
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: get_msg                                                         *
    ****************************************************************************/
    MsgObj *Agent_Msg::get_msg()
    {
        MsgObj *ptr = &msg;
        return ptr;
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: get_msg_type                                                    *
    ****************************************************************************/
    MSG_TYPE Agent_Msg::get_msg_type()
    {
        return msg.type;
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: get_msg_content                                                 *
    ****************************************************************************/
    char *Agent_Msg::get_msg_content()
    {
        return msg.content;
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Function: set_agent_content                                               *
    ****************************************************************************/
    Agent_AID Agent_Msg::get_target_agent()
    {
        return msg.target_agent;
    }

    /****************************************************************************
    * Class: Agent Message                                                      *
    * Functions: agent states messages                                          *
    ****************************************************************************/

    ERROR_CODE Agent_Msg::registration(Agent_AID target_agent)
    {
        Agent_AID AMS;
        Agent *agent_caller;
        Agent *agent_target;
        agent_caller = (Agent *)tasksEnv[caller];
        agent_target = (Agent *)tasksEnv[target_agent];

        if (target_agent == NULL)
        {
            return HANDLE_NULL;
        }
        else if (agent_caller->agent.org == NULL || (agent_caller->agent.org != NULL && (agent_caller->agent.role == OWNER || agent_caller->agent.role == ADMIN)))
        {
            if (agent_caller->agent.org == agent_target->agent.org)
            {
                msg.type = REQUEST;
                msg.content = "REGISTER";
                msg.target_agent = target_agent;
                msg.sender_agent = xTaskGetCurrentTaskHandle();

                //Get the AMS info*/
                AMS = agent_caller->agent.AP;
                /*Sending request*/
                if (xQueueSend(get_mailbox(AMS), &msg.type, portMAX_DELAY) != pdPASS)
                {
                    return INVALID;
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
        else
        {
            return INVALID;
        }
    }

    ERROR_CODE Agent_Msg::deregistration(Agent_AID target_agent)
    {
        Agent_AID AMS;
        Agent *agent_caller;
        Agent *agent_target;
        agent_caller = (Agent *)tasksEnv[caller];
        agent_target = (Agent *)tasksEnv[target_agent];

        if (target_agent == NULL)
        {
            return HANDLE_NULL;
        }
        else if (agent_caller->agent.org == NULL || (agent_caller->agent.org != NULL && (agent_caller->agent.role == OWNER || agent_caller->agent.role == ADMIN)))
        {
            if (agent_caller->agent.org == agent_target->agent.org)
            {
                msg.type = REQUEST;
                msg.content = "DEREGISTER";
                msg.target_agent = target_agent;
                msg.sender_agent = xTaskGetCurrentTaskHandle();

                //Get the AMS info*/
                AMS = agent_caller->agent.AP;
                /*Sending request*/
                if (xQueueSend(get_mailbox(AMS), &msg.type, portMAX_DELAY) != pdPASS)
                {
                    return INVALID;
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
        else
        {
            return INVALID;
        }
    }

    ERROR_CODE Agent_Msg::suspend(Agent_AID target_agent)
    {
        Agent_AID AMS;
        Agent *agent_caller;
        Agent *agent_target;
        agent_caller = (Agent *)tasksEnv[caller];
        agent_target = (Agent *)tasksEnv[target_agent];

        if (target_agent == NULL)
        {
            return HANDLE_NULL;
        }
        else if (agent_caller->agent.org == NULL || (agent_caller->agent.org != NULL && (agent_caller->agent.role == OWNER || agent_caller->agent.role == ADMIN)))
        {
            if (agent_caller->agent.org == agent_target->agent.org)
            {
                msg.type = REQUEST;
                msg.content = "SUSPEND";
                msg.target_agent = target_agent;
                msg.sender_agent = xTaskGetCurrentTaskHandle();

                //Get the AMS info*/
                AMS = agent_caller->agent.AP;
                /*Sending request*/
                if (xQueueSend(get_mailbox(AMS), &msg.type, portMAX_DELAY) != pdPASS)
                {
                    return INVALID;
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
        else
        {
            return INVALID;
        }
    }

    ERROR_CODE Agent_Msg::resume(Agent_AID target_agent)
    {
        Agent_AID AMS;
        Agent *agent_caller;
        Agent *agent_target;
        agent_caller = (Agent *)tasksEnv[caller];
        agent_target = (Agent *)tasksEnv[target_agent];

        if (target_agent == NULL)
        {
            return HANDLE_NULL;
        }
        else if (agent_caller->agent.org == NULL || (agent_caller->agent.org != NULL && (agent_caller->agent.role == OWNER || agent_caller->agent.role == ADMIN)))
        {
            if (agent_caller->agent.org == agent_target->agent.org)
            {
                msg.type = REQUEST;
                msg.content = "RESUME";
                msg.target_agent = target_agent;
                msg.sender_agent = xTaskGetCurrentTaskHandle();

                //Get the AMS info*/
                AMS = agent_caller->agent.AP;
                /*Sending request*/
                if (xQueueSend(get_mailbox(AMS), &msg.type, portMAX_DELAY) != pdPASS)
                {
                    return INVALID;
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
        else
        {
            return INVALID;
        }
    }

    ERROR_CODE Agent_Msg::kill(Agent_AID &target_agent)
    {
        Agent_AID AMS;
        Agent *agent_caller;
        Agent *agent_target;
        agent_caller = (Agent *)tasksEnv[caller];
        agent_target = (Agent *)tasksEnv[target_agent];

        if (target_agent == NULL)
        {
            return HANDLE_NULL;
        }
        else if (agent_caller->agent.org == NULL || (agent_caller->agent.org != NULL && (agent_caller->agent.role == OWNER || agent_caller->agent.role == ADMIN)))
        {
            if (agent_caller->agent.org == agent_target->agent.org)
            {
                msg.type = REQUEST;
                msg.content = "KILL";
                msg.target_agent = target_agent;
                msg.sender_agent = xTaskGetCurrentTaskHandle();

                //Get the AMS info*/
                AMS = agent_caller->agent.AP;
                /*Sending request*/
                if (xQueueSend(get_mailbox(AMS), &msg.type, portMAX_DELAY) != pdPASS)
                {
                    return INVALID;
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
        else
        {
            return INVALID;
        }
    }

    ERROR_CODE Agent_Msg::restart()
    {
        Agent_AID AMS;
        Agent *agent_caller;
        Agent *agent_target;
        agent_caller = (Agent *)tasksEnv[caller];

        msg.type = REQUEST;
        msg.content = "RESTART";
        msg.target_agent = xTaskGetCurrentTaskHandle();
        msg.sender_agent = xTaskGetCurrentTaskHandle();

        AMS = agent_caller->agent.AP;

        if (xQueueSend(get_mailbox(AMS), &msg.type, portMAX_DELAY) != pdPASS)
        {
            return INVALID;
        }
        else
        {
            return NO_ERRORS;
        }
    }

}; // namespace MAES