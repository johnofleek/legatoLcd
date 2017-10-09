/*
 * AUTO-GENERATED _componentMain.c for the appComponent component.

 * Don't bother hand-editing this file.
 */

#include "legato.h"
#include "../src/eventLoop.h"
#include "../src/log.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char* _appComponent_le_data_ServiceInstanceName;
const char** le_data_ServiceInstanceNamePtr = &_appComponent_le_data_ServiceInstanceName;
void le_data_ConnectService(void);
extern const char* _appComponent_le_mdc_ServiceInstanceName;
const char** le_mdc_ServiceInstanceNamePtr = &_appComponent_le_mdc_ServiceInstanceName;
void le_mdc_ConnectService(void);
extern const char* _appComponent_le_mrc_ServiceInstanceName;
const char** le_mrc_ServiceInstanceNamePtr = &_appComponent_le_mrc_ServiceInstanceName;
void le_mrc_ConnectService(void);
extern const char* _appComponent_le_cfg_ServiceInstanceName;
const char** le_cfg_ServiceInstanceNamePtr = &_appComponent_le_cfg_ServiceInstanceName;
void le_cfg_ConnectService(void);
// Component log session variables.
le_log_SessionRef_t appComponent_LogSession;
le_log_Level_t* appComponent_LogLevelFilterPtr;

// Component initialization function (COMPONENT_INIT).
void _appComponent_COMPONENT_INIT(void);

// Library initialization function.
// Will be called by the dynamic linker loader when the library is loaded.
__attribute__((constructor)) void _appComponent_Init(void)
{
    LE_DEBUG("Initializing appComponent component library.");

    // Connect client-side IPC interfaces.
    // 'le_data' is [manual-start].
    le_mdc_ConnectService();
    le_mrc_ConnectService();
    le_cfg_ConnectService();

    // Register the component with the Log Daemon.
    appComponent_LogSession = log_RegComponent("appComponent", &appComponent_LogLevelFilterPtr);

    //Queue the COMPONENT_INIT function to be called by the event loop
    event_QueueComponentInit(_appComponent_COMPONENT_INIT);
}


#ifdef __cplusplus
}
#endif
