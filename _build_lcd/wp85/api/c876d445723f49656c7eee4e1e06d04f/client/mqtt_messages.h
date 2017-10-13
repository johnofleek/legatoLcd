/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef MQTT_MESSAGES_H_INCLUDE_GUARD
#define MQTT_MESSAGES_H_INCLUDE_GUARD


#include "legato.h"

#define PROTOCOL_ID_STR "383e7d8382b90bf52ac1b7fb3f953c59"

#ifdef MK_TOOLS_BUILD
    extern const char** mqtt_ServiceInstanceNamePtr;
    #define SERVICE_INSTANCE_NAME (*mqtt_ServiceInstanceNamePtr)
#else
    #define SERVICE_INSTANCE_NAME "mqtt"
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

#define _MSGID_mqtt_Config 0
#define _MSGID_mqtt_Connect 1
#define _MSGID_mqtt_Disconnect 2
#define _MSGID_mqtt_Send 3
#define _MSGID_mqtt_SendJson 4
#define _MSGID_mqtt_GetConnectionState 5
#define _MSGID_mqtt_AddSessionStateHandler 6
#define _MSGID_mqtt_RemoveSessionStateHandler 7
#define _MSGID_mqtt_AddIncomingMessageHandler 8
#define _MSGID_mqtt_RemoveIncomingMessageHandler 9


#endif // MQTT_MESSAGES_H_INCLUDE_GUARD

