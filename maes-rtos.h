/*  MAES is a framework for Real Time Embedded Systems
    designed by C. Chan-Zheng at TU Delft.
    This is a library which implements the framework
    components to be compatible with FreeRTOS           */



#include <iostream>
#include <vector>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
using namespace std;

#define Agent_AID TaskHandle_t
#define Mailbox_Handle QueueSetHandle_t
#define AGENT_LIST_SIZE 10


enum MSG_TYPE {ACCEPT_PROP, AGREE,          CANCEL,           CALL_PROP,
               CONFIRM,     DISCONFIRM,     FAILURE,          INFORM_IF,
               INFORM_REF,  NOT_UNDERSTOOD, PROPAGATE,        PROPOSE,
               PROXY,       QUERY_IF,       QUERY_REF,        REFUSE,
               REJECT_PROP, REQUEST_WHEN,   REQUEST_WHENEVER, SUBSCRIBE,};

enum ERROR_CODE {NO_ERRORS,     FOUND,     HANDLE_FULL, LIST_FULL, 
                DUPLICATED,     NOT_FOUND, TIMEOUT,     INVALID,
                NOT_REGISTERED};

////////////////////////////////////////////////

// Agents Information and Resources
typedef struct Agent_info
{
    Agent_AID aid;
    Mailbox_Handle mailbox_handle;
    char* agent_name;
    UBaseType_t priority;
    Agent_AID AP;
    org_info* org;
    UBaseType_t affilition;
    UBaseType_t role;
}Agent_info;

typedef struct
 {
    uint16_t stackSize;
}Agent_resources; 

// Otganization Information
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

/////////////////////////////////////////////

class Agent{

    private:
    friend class Agent_Platform;
    friend class Agent_Organization;
    friend class Agent_Message;
    Agent_info agent_info;
    Agent_resources agent_resources;

    public:
    Agent(char* x, UBaseType_t y, uint16_t z){ //Constructor
        agent_info.agent_name = x;
        agent_info.priority = y;
        agent_resources.stackSize = z;
        ////////////////////
    }

    Agent(){ //Constructor
        ////////////////////
    }

    Agent_AID aid(){
        return agent_info.aid;
    }
};

// Agent Platform Class

class Agent_Platform{

    private:
    Agent agentAMS;
    Agent_AID Agent_Handle[AGENT_LIST_SIZE];
    Agent agents[AGENT_LIST_SIZE];
    UBaseType_t subbscribers;
    //USER_DEF_COND cond;
    //USER_DEF_COND* ptr_cond;
    
    public:
    Agent_Platform(char* name){
        agentAMS.agent_info.agent_name = name;
    }

    void agent_init(){}

    bool boot(){}

    bool agent_search(Agent_AID aid){
        int16_t i;
        for(i = 0; i < AGENT_LIST_SIZE;i++){
            if(Agent_Handle[i] == aid){
                return TRUE;
            }
        }
        return FALSE;
    }

    void agent_wait(TickType_t ticks){}

    void agent_yield(){}

    Agent_AID get_running_agent(){}

    BaseType_t get_state(Agent_AID aid){}

    Agent_info get_Agent_description(Agent_AID aid){
        int16_t i;
        for(i = 0; i < AGENT_LIST_SIZE;i++){
            if(Agent_Handle[i] == aid){
                return agents[i].agent_info;
            }
        }
    }

    Agent_info get_AP_description(){
        return agentAMS.agent_info;
    }

    private: //                         ??????
    ERROR_CODE register_agent(Agent_AID aid){}

    ERROR_CODE deregister_agent(Agent_AID aid){}

    ERROR_CODE kill_agent(Agent_AID aid){}

    ERROR_CODE suspend_agent(Agent_AID aid){}

    ERROR_CODE resume_agent(Agent_AID aid){}

    ERROR_CODE restart_agent(Agent_AID aid){}
};

// Agent Organization Class

class Agent_Organization
{
    //public:
};

// Message Class
class Agent_Message
{
    private:
    typedef struct MsgObj{
        Agent_AID sender_agent;
        Agent_AID target_agent;
        MSG_TYPE type;
        char* content; 
    }MsgObj;

    public:
    ;
};

