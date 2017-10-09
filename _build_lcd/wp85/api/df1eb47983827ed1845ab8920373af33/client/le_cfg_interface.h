/*
 * ====================== WARNING ======================
 *
 * THE CONTENTS OF THIS FILE HAVE BEEN AUTO-GENERATED.
 * DO NOT MODIFY IN ANY WAY.
 *
 * ====================== WARNING ======================
 */

/**
 * @page c_config Config Tree API
 *
 * @ref le_cfg_interface.h "API Reference" <br>
 * @ref howToConfigTree
 *
 * <HR>
 *
 * The Config Tree API is used by apps to read and write their specific configurations.
 * Each app is given an isolated tree. The system utilities
 * store their configuration in the @c root tree.
 * Trees are created when they are first accessed by a user or component/process etc.
 * Apps automatically get read access to a tree with the same name as the app.
 * Apps can also be granted read or write access to any other tree with any other name (e.g., a tree
 * named @c foo can be shared with @e appA and @e appB).
 *
 * Paths in the tree look like traditional Unix style paths like this:
 *
 * @code /path/to/my/value @endcode
 *
 * The path root is the root of the tree where the app has been given access. If the
 * app has permission to access another tree, the path can also include the name
 * of the other tree, followed by a colon.
 *
 * @code secondTree:/path/to/my/value @endcode
 *
 * In this case, a value named @c value is read from the tree named @c secondTree
 *
 * The tree is broken down into stems and leaves.
 *
 * A stem is a node that has at least one child
 * node. A leaf has no children, but can hold a value.
 *
 * The config tree supports string, signed integer, boolean, floating point, and empty
 * values. It's recommended to store anything more complex using stems and leaves, which enhances
 * readablity and debugging. It also sidesteps nasty cross platform alignment issues.
 *
 * @todo Talk about the treeConfig, user limits, tree name, tree access. Global timeouts.
 *
 * @section cfg_transaction Read and Write Transactions
 *
 * The config tree uses simple transactions to work with its data. Both read
 * and write transactions are supported. Use read transactions to ensure you can
 * atomically read multiple values from your configuration while keeping consistency
 * with third parties trying to write data.
 *
 * To prevent a single client from locking out other clients, read and
 * write transactions have their own configurable timeout.
 *
 * During a write transaction, both reading and writing are allowed. If you
 * write a value during a transaction and read from that value again, you will get the same value
 * you wrote. Third party clients will continue to see the old value. It's not until you commit
 * your transaction that third parties will begin to see your updated value.
 *
 * During read transactions, writes are not permitted and are thrown away.
 *
 * Transactions are started by creating an iterator. Either a read or write iterator can be
 * created. To end the transaction, you can  delete the iterator, cancelling the
 * transaction. Or,for write transactions, you can commit the iterator.
 *
 * You can have multiple read transactions against the tree. They won't
 * block other transactions from being creating. A read transaction won't block creating a write
 * transaction either. A read transaction only blocks a write transaction from being
 * comitted to the tree.
 *
 * A write transaction in progress will also block creating another write transaction.
 * If a write transaction is in progress when the request for another write transaction comes in,
 * the secondary request will be blocked. This secondary request will remain blocked until the
 * first transaction has been comitted or has timed out. The transaction timeout default is 30
 * seconds. You can extend the timeout by setting a value (in seconds) in
 * @c configTree/transactionTimeout.
 *
 * @section cfg_iteration Iterating the Tree
 *
 * This code sample shows how to iterate a specified node and print its contents:
 *
 * @code
 * static void PrintNode(le_cfg_IteratorRef_t iteratorRef)
 * {
 *     do
 *     {
 *         char stringBuffer[MAX_CFG_STRING] = { 0 };
 *
 *         le_cfg_GetNodeName(iteratorRef, "", stringBuffer, sizeof(stringBuffer));
 *
 *         switch (le_cfg_GetNodeType(iteratorRef, ""))
 *         {
 *             case LE_CFG_TYPE_STEM:
 *                 {
 *                     printf("%s/\n", stringBuffer);
 *
 *                     if (le_cfg_GoToFirstChild(iteratorRef) == LE_OK)
 *                     {
 *                         PrintNode(iteratorRef);
 *                         le_cfg_GoToNode(iteratorRef, "..");
 *                     }
 *                 }
 *                 break;
 *
 *             case LE_CFG_TYPE_EMPTY:
 *                 printf("%s = *empty*\n", stringBuffer);
 *                 break;
 *
 *             case LE_CFG_TYPE_BOOL:
 *                 printf("%s = %s\n",
 *                        stringBuffer,
 *                        (le_cfg_GetBool(iteratorRef, "", false) ? "true" : "false"));
 *                 break;
 *
 *             case LE_CFG_TYPE_INT:
 *                 printf("%s = %d\n", stringBuffer, le_cfg_GetInt(iteratorRef, "", 0));
 *                 break;
 *
 *             case LE_CFG_TYPE_FLOAT:
 *                 printf("%s = %f\n", stringBuffer, le_cfg_GetFloat(iteratorRef, "", 0.0));
 *                 break;
 *
 *             case LE_CFG_TYPE_STRING:
 *                 printf("%s = ", stringBuffer);
 *                 LE_ASSERT(le_cfg_GetString(iteratorRef,
 *                                            "",
 *                                            stringBuffer,
 *                                            sizeof(stringBuffer),
 *                                            "") == LE_OK);
 *                 printf("%s\n", stringBuffer);
 *                 break;
 *
 *             case LE_CFG_TYPE_DOESNT_EXIST:
 *                 printf("%s = ** DENIED **\n", stringBuffer);
 *                 break;
 *         }
 *     }
 *     while (le_cfg_GoToNextSibling(iteratorRef) == LE_OK);
 * }
 *
 *
 * le_cfg_IteratorRef_t iteratorRef = le_cfg_CreateReadTxn("/path/to/my/location");
 *
 * PrintNode(iteratorRef);
 * le_cfg_CancelTxn(iteratorRef);
 *
 *
 * @endcode
 *
 *
 * @section cfg_transactWrite Writing Configuration Data
 *
 * This code sample uses a write transaction to update a target's IP address
 * so the data is written atomically.
 *
 * @code
 * void SetIp4Static
 * (
 *     le_cfg_IteratorRef_t iteratorRef,
 *     const char* interfaceNamePtr,
 *     const char* ipAddrPtr,
 *     const char* netMaskPtr
 * )
 * {
 *     // Change current tree position to the base ip4 node.
 *     char nameBuffer[MAX_CFG_STRING] = { 0 };
 *
 *     int r = snprintf(nameBuffer, sizeof(nameBuffer), "/system/%s/ip4", interfaceNamePtr);
 *     LE_ASSERT((r >= 0) && (r < sizeof(nameBuffer));
 *
 *     le_cfg_GoToNode(iteratorRef, nameBuffer);
 *
 *     le_cfg_SetString(iteratorRef, "addr", ipAddrPtr);
 *     le_cfg_SetString(iteratorRef, "mask", netMaskPtr);
 *
 *     le_cfg_CommitTxn(iteratorRef);
 * }
 * @endcode
 *
 *
 * @section cfg_transactRead Reading Configuration Data
 *
 * This is a code sample of a read transaction.
 *
 * @code
 * le_result_t GetIp4Static
 * (
 *     le_cfg_IteratorRef_t iteratorRef,
 *     const char* interfaceNamePtr,
 *     char* ipAddrPtr,
 *     size_t ipAddrSize,
 *     char* netMaskPtr,
 *     size_t netMaskSize
 * )
 * {
 *     // Change current tree position to the base ip4 node.
 *     char nameBuffer[MAX_CFG_STRING] = { 0 };
 *
 *     int r = snprintf(nameBuffer, sizeof(nameBuffer), "/system/%s/ip4", interfaceNamePtr);
 *     if (r < 0)
 *     {
 *         return LE_FAULT;
 *     }
 *     else if (r >= sizeof(nameBuffer))
 *     {
 *         return LE_OVERFLOW;
 *     }
 *
 *     if (le_cfg_NodeExists(iteratorRef, nameBuffer) == false)
 *     {
 *         LE_WARN("Configuration not found.");
 *         return LE_NOT_FOUND;
 *     }
 *
 *     le_cfg_GoToNode(iteratorRef, nameBuffer);
 *
 *     le_cfg_GetString(iteratorRef, "addr", ipAddrPtr, ipAddrSize, "");
 *     le_cfg_GetString(iteratorRef, "mask", netMaskPtr, netMaskSize, "");
 *
 *     return LE_OK;
 * }
 * @endcode
 *
 *
 * @section cfg_quick Working without Transactions
 *
 * It's possible to ignore iterators and transactions entirely (e.g., if all you need to do
 * is read or write some simple values in the tree).
 *
 * The non-transactional reads and writes work almost identically to the transactional versions.
 * They just don't explictly take an iterator object. The "quick" functions internally use an
 * implicit transaction. This implicit transaction wraps one get or set, and does not protect
 * your code from other activity in the system.
 *
 * Because these functions don't take an explicit transaction, they can't work with relative
 * paths. If a relative path is given, the path will be considered relative to the tree's root.
 *
 * Translating this to a "quick" (non-transactional) example looks like this:
 *
 * @code
 * void ClearIpInfo
 * (
 *     const char* interfaceNamePtr
 * )
 * {
 *     char pathBuffer[MAX_CFG_STRING] = { 0 };
 *
 *     snprintf(pathBuffer, sizeof(pathBuffer), "/system/%s/ip4/", interfaceNamePtr);
 *     le_cfg_QuickDeleteNode(pathBuffer);
 * }
 * @endcode
 *
 * @note Because each read is independant, there's no guarantee of
 * consistency between them. If another process changes one of the values while you
 * read/write the other, the two values could be read out of sync.
 *
 * You'll also need to set @ref howToConfigTree_nonTxn.
 *
 * <HR>
 *
 * Copyright (C) Sierra Wireless Inc. Use of this work is subject to license.
 */
/**
 * @file le_cfg_interface.h
 *
 * Legato @ref c_config include file.
 *
 * Copyright (C) Sierra Wireless Inc. Use of this work is subject to license.
 */

#ifndef LE_CFG_INTERFACE_H_INCLUDE_GUARD
#define LE_CFG_INTERFACE_H_INCLUDE_GUARD


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
void le_cfg_ConnectService
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
le_result_t le_cfg_TryConnectService
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
void le_cfg_DisconnectService
(
    void
);


//--------------------------------------------------------------------------------------------------
/**
 * Reference to a tree iterator object.
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_cfg_Iterator* le_cfg_IteratorRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Identifies the type of node.
 */
//--------------------------------------------------------------------------------------------------
typedef enum
{
    LE_CFG_TYPE_EMPTY = 0,
        ///< A node with no value.

    LE_CFG_TYPE_STRING = 1,
        ///< A string encoded as utf8.

    LE_CFG_TYPE_BOOL = 2,
        ///< Boolean value.

    LE_CFG_TYPE_INT = 3,
        ///< Signed 32-bit.

    LE_CFG_TYPE_FLOAT = 4,
        ///< 64-bit floating point value.

    LE_CFG_TYPE_STEM = 5,
        ///< Non-leaf node, this node is the parent of other nodes.

    LE_CFG_TYPE_DOESNT_EXIST = 6
        ///< Node doesn't exist.
}
le_cfg_nodeType_t;


//--------------------------------------------------------------------------------------------------
/**
 * Length of the strings used by this API.
 */
//--------------------------------------------------------------------------------------------------
#define LE_CFG_STR_LEN 511


//--------------------------------------------------------------------------------------------------
/**
 * Length of the strings used by this API, including the trailing NULL.
 */
//--------------------------------------------------------------------------------------------------
#define LE_CFG_STR_LEN_BYTES 512


//--------------------------------------------------------------------------------------------------
/**
 * Allowed length of a node name.
 */
//--------------------------------------------------------------------------------------------------
#define LE_CFG_NAME_LEN 127


//--------------------------------------------------------------------------------------------------
/**
 * The node name length, including a trailing NULL.
 */
//--------------------------------------------------------------------------------------------------
#define LE_CFG_NAME_LEN_BYTES 128


//--------------------------------------------------------------------------------------------------
/**
 * Reference type used by Add/Remove functions for EVENT 'le_cfg_Change'
 */
//--------------------------------------------------------------------------------------------------
typedef struct le_cfg_ChangeHandler* le_cfg_ChangeHandlerRef_t;


//--------------------------------------------------------------------------------------------------
/**
 * Handler for node change notifications.
 *
 * @param contextPtr
 */
//--------------------------------------------------------------------------------------------------
typedef void (*le_cfg_ChangeHandlerFunc_t)
(
    void* contextPtr
);

//--------------------------------------------------------------------------------------------------
/**
 * Create a read transaction and open a new iterator for traversing the config tree.
 *
 * @note This action creates a read lock on the given tree, which will start a read-timeout.
 *        Once the read timeout expires, all active read iterators on that tree will be
 *        expired and the clients will be killed.
 *
 * @note A tree transaction is global to that tree; a long-held read transaction will block other
 *        user's write transactions from being comitted.
 *
 * @return This will return a newly created iterator reference.
 */
//--------------------------------------------------------------------------------------------------
le_cfg_IteratorRef_t le_cfg_CreateReadTxn
(
    const char* basePath
        ///< [IN] Path to the location to create the new iterator.
);

//--------------------------------------------------------------------------------------------------
/**
 * Create a write transaction and open a new iterator for both reading and writing.
 *
 * @note This action creates a write transaction. If the app holds the iterator for
 *        longer than the configured write transaction timeout, the iterator will cancel the
 *        transaction. Other reads will fail to return data, and all writes will be thrown
 *        away.
 *
 * @note A tree transaction is global to that tree; a long-held write transaction will block
 *       other user's write transactions from being started. Other trees in the system
 *       won't be affected.
 *
 * @return This will return a newly created iterator reference.
 */
//--------------------------------------------------------------------------------------------------
le_cfg_IteratorRef_t le_cfg_CreateWriteTxn
(
    const char* basePath
        ///< [IN] Path to the location to create the new iterator.
);

//--------------------------------------------------------------------------------------------------
/**
 * Close the write iterator and commit the write transaction. This updates the config tree
 * with all of the writes that occured using the iterator.
 *
 * @note This operation will also delete the iterator object.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_CommitTxn
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator object to commit.
);

//--------------------------------------------------------------------------------------------------
/**
 * Close and free the given iterator object. If the iterator is a write iterator, the transaction
 * will be canceled. If the iterator is a read iterator, the transaction will be closed.
 *
 * @note This operation will also delete the iterator object.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_CancelTxn
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator object to close.
);

//--------------------------------------------------------------------------------------------------
/**
 * Change the node where the iterator is pointing. The path passed can be an absolute or a
 * relative path from the iterators current location.
 *
 * The target node does not need to exist. Writing a value to a non-existant node will
 * automatically create that node and any ancestor nodes (parent, parent's parent, etc.) that
 * also don't exist.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_GoToNode
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to move.

    const char* newPath
        ///< [IN] Absolute or relative path from the current location.
);

//--------------------------------------------------------------------------------------------------
/**
 * Move the iterator to the parent of the node.
 *
 * @return Return code will be one of the following values:
 *
 *         - LE_OK        - Commit was completed successfully.
 *         - LE_NOT_FOUND - Current node is the root node: has no parent.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GoToParent
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator to move.
);

//--------------------------------------------------------------------------------------------------
/**
 * Move the iterator to the the first child of the node where the iterator is currently pointed.
 *
 * For read iterators without children, this function will fail. If the iterator is a write
 * iterator, then a new node is automatically created. If this node or newly created
 * children of this node are not written to, then this node will not persist even if the iterator is
 * comitted.
 *
 * @return Return code will be one of the following values:
 *
 *         - LE_OK        - Move was completed successfully.
 *         - LE_NOT_FOUND - The given node has no children.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GoToFirstChild
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator object to move.
);

//--------------------------------------------------------------------------------------------------
/**
 * Jump the iterator to the next child node of the current node. Assuming the following tree:
 *
 * @code
 * baseNode/
 *   childA/
 *     valueA
 *     valueB
 * @endcode
 *
 * If the iterator is moved to the path, "/baseNode/childA/valueA". After the first
 * GoToNextSibling the iterator will be pointing at valueB. A second call to GoToNextSibling
 * will cause the function to return LE_NOT_FOUND.
 *
 * @return Returns one of the following values:
 *
 *         - LE_OK            - Commit was completed successfully.
 *         - LE_NOT_FOUND     - Iterator has reached the end of the current list of siblings.
 *                              Also returned if the the current node has no siblings.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GoToNextSibling
(
    le_cfg_IteratorRef_t iteratorRef
        ///< [IN] Iterator to iterate.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get path to the node where the iterator is currently pointed.
 *
 * Assuming the following tree:
 *
 * @code
 * baseNode/
 *   childA/
 *     valueA
 *     valueB
 * @endcode
 *
 * If the iterator was currently pointing at valueA, GetPath would return the following path:
 *
 * @code
 * /baseNode/childA/valueA
 * @endcode
 *
 * Optionally, a path to another node can be supplied to this function. So, if the iterator is
 * again on valueA and the relative path ".." is supplied then this function will return the
 * following path:
 *
 * @code
 * /baseNode/childA/
 * @endcode
 *
 * @return - LE_OK            - The write was completed successfully.
 *         - LE_OVERFLOW      - The supplied string buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GetPath
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to move.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.

    char* pathBuffer,
        ///< [OUT] Absolute path to the iterator's current node.

    size_t pathBufferNumElements
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the type of node where the iterator is currently pointing.
 *
 * @return le_cfg_nodeType_t value indicating the stored value.
 */
//--------------------------------------------------------------------------------------------------
le_cfg_nodeType_t le_cfg_GetNodeType
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator object to use to read from the tree.

    const char* path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.
);

//--------------------------------------------------------------------------------------------------
/**
 * Get the name of the node where the iterator is currently pointing.
 *
 * @return - LE_OK       Read was completed successfully.
 *         - LE_OVERFLOW Supplied string buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GetNodeName
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator object to use to read from the tree.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.

    char* name,
        ///< [OUT] Read the name of the node object.

    size_t nameNumElements
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Add handler function for EVENT 'le_cfg_Change'
 *
 * This event provides information on changes to the given node object, or any of it's children,
 * where a change could be either a read, write, create or delete operation.
 */
//--------------------------------------------------------------------------------------------------
le_cfg_ChangeHandlerRef_t le_cfg_AddChangeHandler
(
    const char* newPath,
        ///< [IN] Path to the object to watch.

    le_cfg_ChangeHandlerFunc_t handlerPtr,
        ///< [IN]

    void* contextPtr
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Remove handler function for EVENT 'le_cfg_Change'
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_RemoveChangeHandler
(
    le_cfg_ChangeHandlerRef_t addHandlerRef
        ///< [IN]
);

//--------------------------------------------------------------------------------------------------
/**
 * Delete the node specified by the path. If the node doesn't exist, nothing happens. All child
 * nodes are also deleted.
 *
 * If the path is empty, the iterator's current node is deleted.
 *
 * Only valid during a write transaction.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_DeleteNode
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.
);

//--------------------------------------------------------------------------------------------------
/**
 * Check if the given node is empty. A node is also considered empty if it doesn't yet exist. A
 * node is also considered empty if it has no value or is a stem with no children.
 *
 * If the path is empty, the iterator's current node is queried for emptiness.
 *
 * Valid for both read and write transactions.
 *
 * @return A true if the node is considered empty, false if not.
 */
//--------------------------------------------------------------------------------------------------
bool le_cfg_IsEmpty
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.
);

//--------------------------------------------------------------------------------------------------
/**
 * Clear out the nodes's value. If it doesn't exist it will be created, but have no value.
 *
 * If the path is empty, the iterator's current node will be cleared. If the node is a stem
 * then all children will be removed from the tree.
 *
 * Only valid during a write transaction.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetEmpty
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.
);

//--------------------------------------------------------------------------------------------------
/**
 * Check to see if a given node in the config tree exists.
 *
 * @return True if the specified node exists in the tree. False if not.
 */
//--------------------------------------------------------------------------------------------------
bool le_cfg_NodeExists
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read a string value from the config tree. If the value isn't a string, or if the node is
 * empty or doesn't exist, the default value will be returned.
 *
 * Valid for both read and write transactions.
 *
 * If the path is empty, the iterator's current node will be read.
 *
 * @return - LE_OK       - Read was completed successfully.
 *         - LE_OVERFLOW - Supplied string buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_GetString
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path,
        ///<      or a path relative from the iterator's current
        ///<      position.

    char* value,
        ///< [OUT] Buffer to write the value into.

    size_t valueNumElements,
        ///< [IN]

    const char* defaultValue
        ///< [IN] Default value to use if the original can't be
        ///<        read.
);

//--------------------------------------------------------------------------------------------------
/**
 * Write a string value to the config tree. Only valid during a write
 * transaction.
 *
 * If the path is empty, the iterator's current node will be set.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetString
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.

    const char* value
        ///< [IN] Value to write.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read a signed integer value from the config tree.
 *
 * If the underlying value is not an integer, the default value will be returned instead. The
 * default value is also returned if the node does not exist or if it's empty.
 *
 * If the value is a floating point value, then it will be rounded and returned as an integer.
 *
 * Valid for both read and write transactions.
 *
 * If the path is empty, the iterator's current node will be read.
 */
//--------------------------------------------------------------------------------------------------
int32_t le_cfg_GetInt
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.

    int32_t defaultValue
        ///< [IN] Default value to use if the original can't be
        ///<        read.
);

//--------------------------------------------------------------------------------------------------
/**
 * Write a signed integer value to the config tree. Only valid during a
 * write transaction.
 *
 * If the path is empty, the iterator's current node will be set.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetInt
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.

    int32_t value
        ///< [IN] Value to write.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read a 64-bit floating point value from the config tree.
 *
 * If the value is an integer then the value will be promoted to a float. Otherwise, if the
 * underlying value is not a float or integer, the default value will be returned.
 *
 * If the path is empty, the iterator's current node will be read.
 *
 * @note Floating point values will only be stored up to 6 digits of precision.
 */
//--------------------------------------------------------------------------------------------------
double le_cfg_GetFloat
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.

    double defaultValue
        ///< [IN] Default value to use if the original can't be
        ///<        read.
);

//--------------------------------------------------------------------------------------------------
/**
 * Write a 64-bit floating point value to the config tree. Only valid
 * during a write transaction.
 *
 * If the path is empty, the iterator's current node will be set.
 *
 * @note Floating point values will only be stored up to 6 digits of precision.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetFloat
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.

    double value
        ///< [IN] Value to write.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read a value from the tree as a boolean. If the node is empty or doesn't exist, the default
 * value is returned. Default value is also returned if the node is a different type than
 * expected.
 *
 * Valid for both read and write transactions.
 *
 * If the path is empty, the iterator's current node will be read.
 */
//--------------------------------------------------------------------------------------------------
bool le_cfg_GetBool
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.

    bool defaultValue
        ///< [IN] Default value to use if the original can't be
        ///<        read.
);

//--------------------------------------------------------------------------------------------------
/**
 * Write a boolean value to the config tree. Only valid during a write
 * transaction.
 *
 * If the path is empty, the iterator's current node will be set.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_SetBool
(
    le_cfg_IteratorRef_t iteratorRef,
        ///< [IN] Iterator to use as a basis for the transaction.

    const char* path,
        ///< [IN] Path to the target node. Can be an absolute path, or
        ///<      a path relative from the iterator's current position.

    bool value
        ///< [IN] Value to write.
);

//--------------------------------------------------------------------------------------------------
/**
 * Delete the node specified by the path. If the node doesn't exist, nothing happens. All child
 * nodes are also deleted.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickDeleteNode
(
    const char* path
        ///< [IN] Path to the node to delete.
);

//--------------------------------------------------------------------------------------------------
/**
 * Make a given node empty. If the node doesn't currently exist then it is created as a new empty
 * node.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetEmpty
(
    const char* path
        ///< [IN] Absolute or relative path to read from.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read a string value from the config tree. If the value isn't a string, or if the node is
 * empty or doesn't exist, the default value will be returned.
 *
 * @return - LE_OK       - Commit was completed successfully.
 *         - LE_OVERFLOW - Supplied string buffer was not large enough to hold the value.
 */
//--------------------------------------------------------------------------------------------------
le_result_t le_cfg_QuickGetString
(
    const char* path,
        ///< [IN] Path to read from.

    char* value,
        ///< [OUT] Value read from the requested node.

    size_t valueNumElements,
        ///< [IN]

    const char* defaultValue
        ///< [IN] Default value to use if the original can't be read.
);

//--------------------------------------------------------------------------------------------------
/**
 * Write a string value to the config tree.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetString
(
    const char* path,
        ///< [IN] Path to the value to write.

    const char* value
        ///< [IN] Value to write.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read a signed integer value from the config tree. If the value is a floating point
 * value, then it will be rounded and returned as an integer. Otherwise If the underlying value is
 * not an integer or a float, the default value will be returned instead.
 *
 * If the value is empty or the node doesn't exist, the default value is returned instead.
 */
//--------------------------------------------------------------------------------------------------
int32_t le_cfg_QuickGetInt
(
    const char* path,
        ///< [IN] Path to the value to write.

    int32_t defaultValue
        ///< [IN] Default value to use if the original can't be read.
);

//--------------------------------------------------------------------------------------------------
/**
 * Write a signed integer value to the config tree.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetInt
(
    const char* path,
        ///< [IN] Path to the value to write.

    int32_t value
        ///< [IN] Value to write.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read a 64-bit floating point value from the config tree. If the value is an integer,
 * then it is promoted to a float. Otherwise, if the underlying value is not a float, or an
 * integer the default value will be returned.
 *
 * If the value is empty or the node doesn't exist, the default value is returned.
 *
 * @note Floating point values will only be stored up to 6 digits of precision.
 */
//--------------------------------------------------------------------------------------------------
double le_cfg_QuickGetFloat
(
    const char* path,
        ///< [IN] Path to the value to write.

    double defaultValue
        ///< [IN] Default value to use if the original can't be read.
);

//--------------------------------------------------------------------------------------------------
/**
 * Write a 64-bit floating point value to the config tree.
 *
 * @note Floating point values will only be stored up to 6 digits of precision.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetFloat
(
    const char* path,
        ///< [IN] Path to the value to write.

    double value
        ///< [IN] Value to write.
);

//--------------------------------------------------------------------------------------------------
/**
 * Read a value from the tree as a boolean. If the node is empty or doesn't exist, the default
 * value is returned. This is also true if the node is a different type than expected.
 *
 * If the value is empty or the node doesn't exist, the default value is returned instead.
 */
//--------------------------------------------------------------------------------------------------
bool le_cfg_QuickGetBool
(
    const char* path,
        ///< [IN] Path to the value to write.

    bool defaultValue
        ///< [IN] Default value to use if the original can't be read.
);

//--------------------------------------------------------------------------------------------------
/**
 * Write a boolean value to the config tree.
 */
//--------------------------------------------------------------------------------------------------
void le_cfg_QuickSetBool
(
    const char* path,
        ///< [IN] Path to the value to write.

    bool value
        ///< [IN] Value to write.
);


#endif // LE_CFG_INTERFACE_H_INCLUDE_GUARD

