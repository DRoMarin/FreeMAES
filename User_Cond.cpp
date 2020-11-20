#include "maes-rtos.h"

/*    Author: Daniel Rojas Marín    */

namespace MAES
{

    /****************************************************************************
    * Class: USER_DEF_COND                                                      *
    * Defined the default conditions                                            *
    ****************************************************************************/

    bool USER_DEF_COND::register_cond()
    {
        return true;
    }
    
    bool USER_DEF_COND::kill_cond()
    {
        return true;
    }
    
    bool USER_DEF_COND::deregister_cond()
    {
        return true;
    }
    
    bool USER_DEF_COND::suspend_cond()
    {
        return true;
    }
    
    bool USER_DEF_COND::resume_cond()
    {
        return true;
    }
    
    bool USER_DEF_COND::restart_cond()
    {
        return true;
    }
} // namespace MAES