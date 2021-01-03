/***********************************************************
 *   MAES is a framework for Real Time Embedded Systems    *
 *   designed by C. Chan-Zheng at TU Delft.                *
 *   This is a library which implements the framework      *
 *   components to be compatible with FreeRTOS developed   *
 *   by D. Rojas Marin at ITCR.                            *
************************************************************/


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>

namespace MAES
{

#define Agent_AID TaskHandle_t		        // Agent ID
#define Mailbox_Handle QueueHandle_t        // Agent's Mailbox Handle
#define AGENT_LIST_SIZE 64					// Maximum Agents per platform
#define MAX_RECEIVERS AGENT_LIST_SIZE - 1	// Maximum receivers available for any agent
#define BEHAVIOUR_LIST_SIZE 8				//
#define ORGANIZATIONS_SIZE 16				// Maximum Members for org

	/*******************************************************
	 *                        ENUMS                        *
	 *******************************************************/

	enum MSG_TYPE
	{
		ACCEPT_PROPOSAL,
		AGREE,
		CANCEL,
		CFP,
		CONFIRM,
		DISCONFIRM,
		FAILURE,
		INFORM,
		INFORM_IF,
		INFORM_REF,
		NOT_UNDERSTOOD,
		PROPAGATE,
		PROPOSE,
		QUERY_IF,
		QUERY_REF,
		REFUSE,
		REJECT_PROPOSAL,
		REQUEST,
		REQUEST_WHEN,
		REQUEST_WHENEVER,
		SUBSCRIBE,
		NO_RESPONSE,
	};

	enum ERROR_CODE
	{
		NO_ERRORS,
		FOUND,
		HANDLE_NULL,
		LIST_FULL,
		DUPLICATED,
		NOT_FOUND,
		TIMEOUT,
		INVALID,
		NOT_REGISTERED,
	};

	enum AGENT_MODE
	{
		ACTIVE,
		SUSPENDED,
		WAITING,
		TERMINATED,
		NO_MODE,
	};

	enum ORG_AFFILIATION
	{
		OWNER,
		ADMIN,
		MEMBER,
		NON_MEMBER,
	};

	enum ORG_ROLE
	{
		MODERATOR,
		PARTICIPANT,
		VISITOR,
		NONE,
	};

	enum ORG_TYPE
	{
		HIERARCHY,
		TEAM,
	};

	/*******************************************************
	 *                     DEFINITIONS                     *
	 *******************************************************/

	 // Organization Information
	typedef struct
	{
		ORG_TYPE org_type;
		UBaseType_t members_num;
		UBaseType_t banned_num;
		Agent_AID members[AGENT_LIST_SIZE];
		Agent_AID banned[AGENT_LIST_SIZE];
		Agent_AID owner;
		Agent_AID admin;
		Agent_AID moderator;
	} org_info;

	// Agent Information
	typedef struct
	{
		Agent_AID aid;
		Mailbox_Handle mailbox_handle;
		char* agent_name;
		UBaseType_t priority;
		Agent_AID AP;
		org_info* org;
		ORG_AFFILIATION affiliation;
		ORG_ROLE role;
	} Agent_info;

	// Agent Resources
	typedef struct
	{
		uint16_t stackSize;
		TaskFunction_t function;
		void* taskParameters;
	} Agent_resources;

	// Message
	typedef struct
	{
		Agent_AID sender_agent;
		Agent_AID target_agent;
		MSG_TYPE type;
		char* content;
	} MsgObj;

	// AP Description
	typedef struct
	{
		Agent_AID AMS_AID;
		char* AP_name;
		UBaseType_t subscribers;
	} AP_Description;

	/*******************************************************
	 *                       CLASSES                       *
	 *******************************************************/

	 // User Conditions
	class USER_DEF_COND
	{
	public:
		virtual bool register_cond();
		virtual bool deregister_cond();
		virtual bool suspend_cond();
		virtual bool kill_cond();
		virtual bool resume_cond();
		virtual bool restart_cond();
	};

	// Agent Class
	class Agent
	{
	private:
		Agent_info agent;
		Agent_resources resources;
		Agent();

	public:
		friend class Agent_Platform;
		friend class Agent_Organization;
		friend class Agent_Msg;
		Agent(char* name, UBaseType_t pri, uint16_t sizeStack);
		Agent_AID AID();
	};
	
	//Environment Class

	typedef struct{
		Agent_AID first;
		Agent * second;
		}sysVar;

	class sysVars
	{
	public:
		Agent* get_TaskEnv(Agent_AID aid);
		void set_TaskEnv(Agent_AID aid, Agent* agent_ptr);
		void erase_TaskEnv(Agent_AID aid);
		sysVar * getEnv();

	private:
		sysVar environment[AGENT_LIST_SIZE];
	};

	extern sysVars env;

	// Agent Platform Class
	class Agent_Platform
	{
	private:
		Agent agentAMS;
		Agent_AID Agent_Handle[AGENT_LIST_SIZE];
		AP_Description description;
		USER_DEF_COND cond;
		USER_DEF_COND* ptr_cond;

	public:
		Agent_Platform(char* name);
		Agent_Platform(char* name, USER_DEF_COND* user_cond);

		bool boot();
		void agent_init(Agent* agent, void behaviour(void* pvParameters));
		void agent_init(Agent* agent, void behaviour(void* pvParameters), void* pvParameters);
		bool agent_search(Agent_AID aid);
		void agent_wait(TickType_t ticks);
		void agent_yield();
		Agent_AID get_running_agent();
		AGENT_MODE get_state(Agent_AID aid);
		Agent_info get_Agent_description(Agent_AID aid);
		AP_Description get_AP_description();

		ERROR_CODE register_agent(Agent_AID aid);
		ERROR_CODE deregister_agent(Agent_AID aid);
		ERROR_CODE kill_agent(Agent_AID aid);
		ERROR_CODE suspend_agent(Agent_AID aid);
		ERROR_CODE resume_agent(Agent_AID aid);
		void restart(Agent_AID aid);
	};
	
	// AMS_task namespace
	namespace
	{
		typedef struct
		{
			Agent_Platform* services;
			USER_DEF_COND* cond;
		} AMSparameter;
		
		//AMSparameter parameters;
		void AMS_task(void* pvParameters);
	} // namespace

	// Message Class
	class Agent_Msg
	{
	private:
		MsgObj msg;
		Agent_AID receivers[MAX_RECEIVERS];
		UBaseType_t subscribers;
		Agent_AID caller;
		bool isRegistered(Agent_AID aid);
		Mailbox_Handle get_mailbox(Agent_AID aid);
		sysVars* ptr_env;
		// MAILBOX
	public:
		Agent_Msg();
		ERROR_CODE add_receiver(Agent_AID aid_receiver);
		ERROR_CODE remove_receiver(Agent_AID aid_receiver);
		void clear_all_receiver();
		void refresh_list();
		MSG_TYPE receive(TickType_t timeout);
		ERROR_CODE send(Agent_AID aid_receiver, TickType_t timeout);
		ERROR_CODE send();
		void set_msg_type(MSG_TYPE type);
		void set_msg_content(char* body);

		MsgObj* get_msg();
		MSG_TYPE get_msg_type();
		char* get_msg_content();
		Agent_AID get_sender();
		Agent_AID get_target_agent();

		ERROR_CODE registration(Agent_AID target_agent);
		ERROR_CODE deregistration(Agent_AID target_agent);
		ERROR_CODE suspend(Agent_AID target_agent);
		ERROR_CODE resume(Agent_AID target_agent);
		ERROR_CODE kill(Agent_AID target_agent);
		ERROR_CODE restart();
	};
	
		// Agent Organization Class
	class Agent_Organization
	{
	private:
		sysVars* ptr_env;
		org_info description;

	public:
		Agent_Organization(ORG_TYPE organization_type);
		ERROR_CODE create();
		ERROR_CODE destroy();
		ERROR_CODE isMember(Agent_AID aid);
		ERROR_CODE isBanned(Agent_AID aid);
		ERROR_CODE change_owner(Agent_AID aid);
		ERROR_CODE set_admin(Agent_AID aid);
		ERROR_CODE set_moderator(Agent_AID aid);
		ERROR_CODE add_agent(Agent_AID aid);
		ERROR_CODE kick_agent(Agent_AID aid);
		ERROR_CODE ban_agent(Agent_AID aid);
		ERROR_CODE remove_ban(Agent_AID aid);
		void clear_ban_list();
		ERROR_CODE set_participant(Agent_AID aid);
		ERROR_CODE set_visitor(Agent_AID aid);
		ORG_TYPE get_org_type();
		org_info get_info();
		UBaseType_t get_size();
		MSG_TYPE invite(Agent_Msg msg, UBaseType_t password, Agent_AID target_agent, UBaseType_t timeout);
	};


	// Behaviour Related Classes
	class Generic_Behaviour
	{
	public:
		void* taskParameters;
		Agent_Msg msg;
		Generic_Behaviour();
		virtual void action() = 0;
		virtual void setup();
		virtual bool done();
		virtual bool failure_detection();
		virtual void failure_identification();
		virtual void failure_recovery();
		void execute();
	};

	class OneShotBehaviour : public Generic_Behaviour
	{
	public:
		OneShotBehaviour();
		virtual void action() = 0;
		virtual bool done();
	};

	class CyclicBehaviour : public Generic_Behaviour
	{
	public:
		CyclicBehaviour();
		virtual void action() = 0;
		virtual bool done();
	};

} // namespace MAES
