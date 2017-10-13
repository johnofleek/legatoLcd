/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */


#ifndef MQTT_INTERFACE_H_INCLUDE_GUARD
#define MQTT_INTERFACE_H_INCLUDE_GUARD


#include "legato.h"

//--------------------------------------------------------------------------------------------------
/**
 *
 * Connect the current client thread to the service providing this API. Block until the service is
 * available.
 *
 * For each thread that wants to use this API, either ConnectService or TryConnectService must be
 * called before any other functions in this API.  Normally, ConnectService is automatically called
 * for the main thread, but not for any other thread. For details, see @ref apiFilesC_client.
 *
 * This function is created automatically.
 */
//--------------------------------------------------------------------------------------------------
void mqtt_ConnectService
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 *
 * Try to connect the current client thread to the service providing this API. Return with an error
 * if the service is not available.
 *
 * For each thread that wants to use this API, either ConnectService or TryConnectService must be
 * called before any other functions in this API.  Normally, ConnectService is automatically called
 * for the main thread, but not for any other thread. For details, see @ref apiFilesC_client.
 *
 * This function is created automatically.
 *
 * @return
 *  - LE_OK if the client connected successfully to the service.
 *  - LE_UNAVAILABLE if the server is not currently offering the service to which the client is bound.
 *  - LE_NOT_PERMITTED if the client interface is not bound to any service (doesn't have a binding).
 *  - LE_COMM_ERROR if the Service Directory cannot be reached.
 */
//--------------------------------------------------------------------------------------------------
le_result_t mqtt_TryConnectService
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 *
 * Disconnect the current client thread from the service providing this API.
 *
 * Normally, this function doesn't need to be called. After this function is called, there's no
 * longer a connection to the service, and the functions in this API can't be used. For details, see
 * @ref apiFilesC_client.
 *
 * This function is created automatically.
 */
//--------------------------------------------------------------------------------------------------
void mqtt_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'mqtt_SessionState'
 */
//--------------------------------------------------------------------------------------------------
typedef struct mqtt_SessionStateHandler* mqtt_SessionStateHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'mqtt_IncomingMessage'
 */
//--------------------------------------------------------------------------------------------------
typedef struct mqtt_IncomingMessageHandler* mqtt_IncomingMessageHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for session state changes
 *
 * @param isConnected
 *        Session State: connected or disconnected
 * @param connectErrorCode
 *        connection returned code
 * @param subErrorCode
 *        subscribe returned code
 * @param contextPtr
 */
//--------------------------------------------------------------------------------------------------
typedef void (*mqtt_SessionStateHandlerFunc_t)
(
    bool isConnected,
    int32_t connectErrorCode,
    int32_t subErrorCode,
    void* contextPtr
);


//--------------------------------------------------------------------------------------------------
/**
 * Handler for Incoming message
 *
 * @param topicName
 *        Name of the subscribed topic
 * @param key
 *        Key Name of the data
 * @param value
 *        Value of the data
 * @param timestamp
 *        Timestamp of the data
 * @param contextPtr
 */
//--------------------------------------------------------------------------------------------------
typedef void (*mqtt_IncomingMessageHandlerFunc_t)
(
    const char* topicName,
    const char* key,
    const char* value,
    const char* timestamp,
    void* contextPtr
);

//--------------------------------------------------------------------------------------------------
/**
 * Configure the session
 */
//--------------------------------------------------------------------------------------------------
void mqtt_Config
(
    const char* brokerUrl,
        ///< [IN]

    int32_t portNumber,
        ///< [IN]

    int32_t keepAlive,
        ///< [IN]

    int32_t QoS
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Open a MQTT session
 */
//--------------------------------------------------------------------------------------------------
void mqtt_Connect
(
    const char* password
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Close MQTT session
 */
//--------------------------------------------------------------------------------------------------
void mqtt_Disconnect
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Send data (key, value) to MQTT broker
 */
//--------------------------------------------------------------------------------------------------
void mqtt_Send
(
    const char* key,
        ///< [IN]

    const char* value,
        ///< [IN]

    int32_t* errCodePtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * Send JSON data (JSON) to MQTT broker
 */
//--------------------------------------------------------------------------------------------------
void mqtt_SendJson
(
    const char* json,
        ///< [IN]

    int32_t* errCodePtr
        ///< [OUT]
);

//--------------------------------------------------------------------------------------------------
/**
 * get mqtt connection state - useful for manual query
 */
//--------------------------------------------------------------------------------------------------
uint8_t mqtt_GetConnectionState
(
    void
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'mqtt_SessionState'
 *
 * This event provides information on session state changes
 */
//--------------------------------------------------------------------------------------------------
mqtt_SessionStateHandlerRef_t mqtt_AddSessionStateHandler
(
    mqtt_SessionStateHandlerFunc_t handlerPtr,
        ///< [IN]

    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'mqtt_SessionState'
 */
//--------------------------------------------------------------------------------------------------
void mqtt_RemoveSessionStateHandler
(
    mqtt_SessionStateHandlerRef_t addHandlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'mqtt_IncomingMessage'
 *
 * This event provides information on the incoming MQTT message
 */
//--------------------------------------------------------------------------------------------------
mqtt_IncomingMessageHandlerRef_t mqtt_AddIncomingMessageHandler
(
    mqtt_IncomingMessageHandlerFunc_t handlerPtr,
        ///< [IN]

    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'mqtt_IncomingMessage'
 */
//--------------------------------------------------------------------------------------------------
void mqtt_RemoveIncomingMessageHandler
(
    mqtt_IncomingMessageHandlerRef_t addHandlerRef
        ///< [IN]
);


#endif // MQTT_INTERFACE_H_INCLUDE_GUARD

