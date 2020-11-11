/*  MAES is a framework for Real Time Embedded Systems
    designed by C. Chan-Zheng at TU Delft.
    This is a library which implements the framework
    components to be compatible with FreeRTOS           */


// Agent information and Resources
#include <iostream>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define Agent_AID TaskHandle_t
#define Mailbox_Handle QueueSetHandle_t


#define AGENT_LIST_SIZE 10

class Agent{
    private:

    typedef struct Agent_info
    {

        Agent_AID aid;
        Mailbox_Handle mailbox_handle;
        const char * agent_name;
        UBaseType_t priority;
        Agent_AID AP;
        org_info *org;
        UBaseType_t affilition;
        UBaseType_t role;

    }Agent_info;
    friend class Agent_Platform;
    friend class Message;


    typedef struct
    {
        uint16_t stackSize;

    }Agent_resources;
};

// Agent Platform Class

class Agent_Platform{

    private:
        Agent agentAMS;
        Agent_AID Agent_Handle[AGENT_LIST_SIZE];
        UBaseType_t subbscribers;
        //USER_DEF_COND cond;
        //USER_DEF_COND *ptr_cond;
    
    public:
    //
};

// Agent Organization Class

typedef struct org_info
        {
            enum org_type;
            UBaseType_t member_num;
            UBaseType_t banned_num;
            Agent_AID members[AGENT_LIST_SIZE];
            Agent_AID banned[AGENT_LIST_SIZE];
            Agent_AID owner;
            Agent_AID admin;
            Agent_AID moderator;

    }org_info;

class Agent_Org
{
    //public:
    //
};

// Message Class
class Message
{
    private:
    typedef struct MsgObj{
        Agent_AID sender_agent;
        Agent_AID target_agent;
        //MSG_TYPE type;
        char * content; 
    }MsgObj;

    public:
    //
    ;
};

