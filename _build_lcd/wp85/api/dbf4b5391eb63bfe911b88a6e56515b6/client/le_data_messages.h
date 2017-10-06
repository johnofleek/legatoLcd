/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef LE_DATA_MESSAGES_H_INCLUDE_GUARD
#define LE_DATA_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "8d7a449d97fb39a21739605a943cac04"

#ifdef MK_TOOLS_BUILD
    extern const char** le_data_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*le_data_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "le_data"
#endif


// todo: This will need to depend on the particular protocol, but the exact size is not easy to
//       calculate right now, so in the meantime, pick a reasonably large size.  Once interface
//       type support has been added, this will be replaced by a more appropriate size.
#define _MAX_MSG_SIZE 1100

// Define the message type for communicating between client and server
typedef struct
{
    uint32_t id;
    uint8_t buffer[_MAX_MSG_SIZE];
}
_Message_t;

#define _MSGID_le_data_AddConnectionStateHandler 0
#define _MSGID_le_data_RemoveConnectionStateHandler 1
#define _MSGID_le_data_Request 2
#define _MSGID_le_data_Release 3
#define _MSGID_le_data_SetTechnologyRank 4
#define _MSGID_le_data_GetFirstUsedTechnology 5
#define _MSGID_le_data_GetNextUsedTechnology 6
#define _MSGID_le_data_GetTechnology 7


#endif // LE_DATA_MESSAGES_H_INCLUDE_GUARD

