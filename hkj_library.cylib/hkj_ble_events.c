#include "hkj_library.h"
/** Host stack events.
   * Generic events: 0x01 to 0x1F
   * GAP events: 0x20 to 0x3F
   * GATT events: 0x40 to 0x6F
   * L2CAP events: 0x70 to 0x7F  
   * Future use: 0x80 to 0xFF
*/
typedef enum
{
    /** This event is triggered by BLE stack when stack is in a bad state, Restarting stack is the 
	only way to get out of the state */
    CYBLE_EVT_HOST_INVALID = 0x00u,

    /* Range for Generic events - 0x01 to 0x1F */

    /** This event is received when BLE stack is initialized and turned ON by invoking CyBle_StackInit () function.*/
    CYBLE_EVT_STACK_ON = 0x01u,

    /** This event is received when there is a timeout and application needs to handle the event. 
       Timeout reason is defined by CYBLE_TO_REASON_CODE_T. */
    CYBLE_EVT_TIMEOUT,

    /** This event indicates that some internal hardware error has occurred. Reset of the hardware may be required. */
    CYBLE_EVT_HARDWARE_ERROR,

    /** This event is triggered by 'Host Stack' if 'Controller' responds with an error code for any HCI command.
       Event parameter returned will be an HCI error code as defined in Bluetooth 4.1 core specification, Volume 2,
       Part D, section 1.3. This event will be received only if there is an error. */
    CYBLE_EVT_HCI_STATUS,

    /** This event is triggered by host stack if BLE stack is busy or not. 
       Event Parameter corresponding to this event will indicate the state of BLE stack's internal protocol buffers
       for the application to safely initiate data transactions (GATT, GAP Security, and L2CAP transactions)
       with the peer BLE device.
       Event parameter is of type uint8.

        * CYBLE_STACK_STATE_BUSY (0x01) = CYBLE_STACK_STATE_BUSY indicates application that BLE stack's internal buffers
        *                   are about to be filled, and the remaining buffers are required to respond peer BLE device
        *                   After this event, application shall not initiate (GATT, GAP Security and L2CAP data transactions). 
        *                   However application shall respond to peer initiated transactions to prevent BLE protocol timeouts
        *                   to occur.
        *                   Application initiated data transactions can be resumed after CYBLE_EVT_STACK_BUSY_STATUS
        *                   event with parameter 'CYBLE_STACK_STATE_FREE' is received.
        *
        * CYBLE_STACK_STATE_FREE (0x00) = CYBLE_STACK_STATE_FREE indicates application that pending transactions are completed
        *                   and sufficient buffers are available to process application initiated transactions.
        *                   The 'CYBLE_EVT_STACK_BUSY_STATUS' event with 'CYBLE_STACK_STATE_FREE' is indicated to 
        *                   application if BLE Stack's internal buffer state has transitioned from 'CYBLE_STACK_STATE_BUSY'
        *                   to 'CYBLE_STACK_STATE_FREE'.
        *
       To increase BLE stack's internal buffers count and achieve better throughput for attribute MTU greater then 32, 
       use MaxAttrNoOfBuffer parameter in the Expression view of the Advanced tab    
    */
    CYBLE_EVT_STACK_BUSY_STATUS,
	
	/** This event is received when stack wants application to provide memory to process remote request.
       Event parameter is of type CYBLE_MEMORY_REQUEST_T.
       This event is automatically handled by the component for the CYBLE_PREPARED_WRITE_REQUEST request. 
       The component allocates sufficient memory for the long write request with assumption that attribute MTU size 
       is negotiated to the minimum possible value. Application could use dynamic memory allocation to save static 
       RAM memory consumption. To enable this event for application level, set EnableExternalPrepWriteBuff parameter
       in the Expression view of the Advanced tab to the true.    
    */
	CYBLE_EVT_MEMORY_REQUEST,

    /* Range for GAP events - 0x20 to 0x3F */

    /** This event is triggered every time a device is discovered; pointer to structure of type CYBLE_GAPC_ADV_REPORT_T 
        is returned as the event parameter. */
    CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT = 0x20u,

    /** This event is received by Peripheral and Central devices. When it is received by Peripheral, peripheral 
        needs to Call CyBle_GappAuthReqReply() to reply to authentication request from Central.

        When this event is received by Central, that means the slave has requested Central to initiate authentication
        procedure. Central needs to call CyBle_GappAuthReq() to initiate authentication procedure.
        Pointer to structure of type CYBLE_GAP_AUTH_INFO_T is returned as the event parameter. */
    CYBLE_EVT_GAP_AUTH_REQ,

    /** This event indicates that the device has to send passkey to be used during the pairing procedure. 
       CyBle_GapAuthPassKeyReply() is required to be called with valid parameters on receiving this event.

       Refer to Bluetooth Core Spec. 4.1, Part H, Section 2.3.5.1 Selecting STK Generation Method.

       Nothing is returned as part of the event parameter. */
    CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST,

    /** This event indicates that the device needs to display passkey during the pairing procedure.

       Refer to Bluetooth Core Spec. 4.1, Part H, Section 2.3.5.1 Selecting STK Generation Method.

       Pointer to data of type 'uint32' is returned as part of the event parameter. Passkey can
       be any 6-decimal-digit value. */
    CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST,

     /** This event indicates that the authentication procedure has been completed.

     The event parameter contains the security information as defined by CYBLE_GAP_AUTH_INFO_T.
     This event is generated at the end of the following three operations:
    	* Authentication is initiated with a newly connected device
    	* Encryption is initiated with a connected device that is already bonded
    	* Re-Encryption is initiated with a connected device with link already encrypted
     During encryption/re-encryption, the Encryption Information exchanged during the pairing process
     is used to encrypt/re-encrypt the link. As this does not modify any of the authentication 
     parameters with which the devices were paired, this event is generated with NULL event data
     and the result of the encryption operation. */
    CYBLE_EVT_GAP_AUTH_COMPLETE,

    /** Authentication process failed between two devices. The return value of type 
       CYBLE_GAP_AUTH_FAILED_REASON_T indicates the reason for failure. */
    CYBLE_EVT_GAP_AUTH_FAILED,

    /** Peripheral device has started/stopped advertising. 
       This event is generated after making a call to CyBle_GappEnterDiscoveryMode and 
       CyBle_GappExitDiscoveryMode functions. The event parameter contains the status
       which is of type 'uint8'.
       
       If the data is '0x00', it indicates 'success'; Anything else indicates 'failure'. */
    CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP,
    
    /** This event is generated at the GAP Peripheral end after connection is completed with peer Central device.
    	For GAP Central device, this event is generated as in acknowledgment of receiving this event successfully
    	by BLE Controller. Once connection is done, no more event is required but if fails to establish connection,
    	'CYBLE_EVT_GAP_DEVICE_DISCONNECTED' is passed to application. ' CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE'
    	event is triggered instead of 'CYBLE_EVT_GAP_DEVICE_CONNECTED', if Link Layer Privacy is enabled in component customizer.   
	Event parameter is a pointer to a structure of type CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T. */
    CYBLE_EVT_GAP_DEVICE_CONNECTED,

	/** Disconnected from remote device or failed to establish connection. Parameter returned with the event 
	contains pointer to the reason for disconnection, which is of type uint8. For details refer
	core spec 4.2, vol2, part D */
    CYBLE_EVT_GAP_DEVICE_DISCONNECTED,

    /** Encryption change event for active connection. 'evParam' can be decoded as
         * evParam[0] = 0x00 -> Encryption OFF
         * evParam[0] = 0x01 -> Encryption ON
    	 * Any other value of evParam[0] -> Error
     
       This is an informative event for application when there is a change in encryption. 
       Application may choose to ignore it. */
    CYBLE_EVT_GAP_ENCRYPT_CHANGE,

    /** This event is generated at the GAP Central and the Peripheral end after connection parameter update
		is requested from the host to the controller. Event parameter is a pointer to a structure of type CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T. */
    CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE,

    /** Central device has started/stopped scanning. 
       This event is generated after making a call to CyBle_GapcStartDiscovery and 
       CyBle_GapcStopDiscovery APIs. The event parameter contains the status, which is of type 'uint8'.
       
       If the data is '0x00', it indicates 'success'; Anything else indicates 'failure'. */
    CYBLE_EVT_GAPC_SCAN_START_STOP,

    /** Indication that the SMP keys exchange with peer device is complete, the event handler
       is expected to store the peer device keys, especially IRK which is used to resolve the
       peer device after the connection establishment.
       
       Event parameter returns data of type CYBLE_GAP_SMP_KEY_DIST_T containing the peer device keys. */
    CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT,
    
    /** This event indicates that the device needs to display passkey during 
        secure connection pairing procedure. CyBle_GapAuthPassKeyReply() is
        required to be called with valid parameters on receiving this event.
        Since no key to be entered by the user for Numeric comparison, 
        parameter passkey for the function CyBle_GapAuthPassKeyReply will be 
        ignored.
        Event parameter is a pointer to a 6 digit Passkey value. */
    CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST,
    
    /** This event is generated when keypress (Secure connections) is received
       from peer device. */
    CYBLE_EVT_GAP_KEYPRESS_NOTIFICATION,

    /** This event is generated when OOB generation for Secure connections is complete.  
       Event parameter is of type 'CYBLE_GAP_OOB_DATA_T' */
    CYBLE_EVT_GAP_OOB_GENERATED_NOTIFICATION,
    
    /** The LE Data Length Change event notifies the Host of a change to either the maximum Payload length or 
       the maximum transmission time of Data Channel PDUs in either direction. The values reported are the maximum
       that will actually be used on the connection following the change. Event parameter is of type 
       'CYBLE_GAP_CONN_DATA_LENGTH_T' */
    CYBLE_EVT_GAP_DATA_LENGTH_CHANGE,

    /** The LE Enhanced Connection Complete event indicates application that a new connection has been created when 
        Link Layer Privacy is enabled in component customizer. 
       Event parameter is of type 'CYBLE_GAP_ENHANCE_CONN_COMPLETE_T' */
    CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE,
	
    /** The LE Direct Advertising Report event indicates that directed advertisements have been received where 
       the advertiser is using a resolvable private address for the InitA field in the ADV_DIRECT_IND PDU and the
       Scanning_Filter_Policy is equal to 0x02 or 0x03. Event parameter is of type 'CYBLE_GAPC_DIRECT_ADV_REPORT_T' */
    CYBLE_EVT_GAPC_DIRECT_ADV_REPORT,

    /** SMP negotiated auth info event is raised as soon as SMP has completed pairing properties (feature exchange)
     * negotiation. The event parameter is CYBLE_GAP_AUTH_INFO_T. CYBLE_GAP_AUTH_INFO_T will have the 
     * negotiated parameter, the pairing should either pass with these negotiated parameters or may fail.
     */
    CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO,

    /* Range for GATT events - 0x40 to 6F */

    /** The event is received by the Client when the Server cannot perform the requested 
       operation and sends out an error response. Event parameter is a pointer to a structure
       of type CYBLE_GATTC_ERR_RSP_PARAM_T. */
    CYBLE_EVT_GATTC_ERROR_RSP = 0x40u,

    /** This event is generated at the GAP Peripheral end after connection is completed with peer Central device.
    	For GAP Central device, this event is generated as in acknowledgment of receiving this event successfully
    	by BLE Controller. Once connection is done, no more event is required but if fails to establish connection,
    	'CYBLE_EVT_GATT_DISCONNECT_IND' is passed to application.      
	Event parameter is a pointer to a structure of type CYBLE_GAP_CONN_PARAM_UPDATED_IN_CONTROLLER_T. */
    CYBLE_EVT_GATT_CONNECT_IND,

    /** GATT is disconnected. Nothing is returned as part of the event parameter. */
    CYBLE_EVT_GATT_DISCONNECT_IND,

    /** 'GATT MTU Exchange Request' received from GATT client device. Event parameter 
       contains the MTU size of type CYBLE_GATT_XCHG_MTU_PARAM_T. */
    CYBLE_EVT_GATTS_XCNHG_MTU_REQ,

    /** 'GATT MTU Exchange Response' received from server device. Event parameter is a
       pointer to a structure of type CYBLE_GATT_XCHG_MTU_PARAM_T. */
    CYBLE_EVT_GATTC_XCHNG_MTU_RSP,

    /** 'Read by Group Type Response' received from server device. Event parameter
       is a pointer to a structure of type CYBLE_GATTC_READ_BY_GRP_RSP_PARAM_T. */
    CYBLE_EVT_GATTC_READ_BY_GROUP_TYPE_RSP,

    /** 'Read by Type Response' received from server device. Event parameter is a
       pointer to a structure of type CYBLE_GATTC_READ_BY_TYPE_RSP_PARAM_T. */
    CYBLE_EVT_GATTC_READ_BY_TYPE_RSP,

    /** 'Find Information Response' received from server device. Event parameter is
       a pointer to a structure of type 'CYBLE_GATTC_FIND_INFO_RSP_PARAM_T. */
    CYBLE_EVT_GATTC_FIND_INFO_RSP,

    /** 'Find by Type Value Response' received from server device. Event parameter is
       a pointer to a structure of type CYBLE_GATTC_FIND_BY_TYPE_RSP_PARAM_T. */
    CYBLE_EVT_GATTC_FIND_BY_TYPE_VALUE_RSP,

    /** 'Read Response' from server device. Event parameter is a pointer to a
       structure of type CYBLE_GATTC_READ_RSP_PARAM_T. */
    CYBLE_EVT_GATTC_READ_RSP,

    /** 'Read Blob Response' from server. Event parameter is a pointer to a
       structure of type CYBLE_GATTC_READ_RSP_PARAM_T. */
    CYBLE_EVT_GATTC_READ_BLOB_RSP,

    /** 'Read Multiple Responses' from server. Event parameter is a pointer
       to a structure of type CYBLE_GATTC_READ_RSP_PARAM_T. The 'actualLen' field
       should be ignored as it is unused in this event response. */
    CYBLE_EVT_GATTC_READ_MULTI_RSP,

    /** 'Write Request' from client device. Event parameter is a pointer to
       a structure of type CYBLE_GATTS_WRITE_REQ_PARAM_T . */
    CYBLE_EVT_GATTS_WRITE_REQ,

    /** 'Write Response' from server device. Event parameter is a pointer
       to a structure of type CYBLE_CONN_HANDLE_T. */
    CYBLE_EVT_GATTC_WRITE_RSP,

    /** 'Write Command' Request from client device. Event parameter is a 
       pointer to a structure of type CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T. */
    CYBLE_EVT_GATTS_WRITE_CMD_REQ,

    /** 'Prepare Write' Request from client device. Event parameter is a
       pointer to a structure of type CYBLE_GATTS_PREP_WRITE_REQ_PARAM_T. */
    CYBLE_EVT_GATTS_PREP_WRITE_REQ, 

    /** 'Execute Write' request from client device. Event parameter is a
       pointer to a structure of type 'CYBLE_GATTS_EXEC_WRITE_REQ_T'
       This event will be triggered before GATT DB is modified. GATT Db will be updated 
       only if there is no error condition provided by application. In case of error condition triggered
       during stack validation, partial write will occur. Write will be canceled from that handle where 
       error has occurred and error response corresponding to that handle will be sent to remote.
       If at any point of time 'CYBLE_GATT_EXECUTE_WRITE_CANCEL_FLAG' is received in 
       execWriteFlag fields of 'CYBLE_GATTS_EXEC_WRITE_REQ_T' structure, then all previous 
       writes are canceled. For execute cancel scenario, all elements of 
       'CYBLE_GATTS_EXEC_WRITE_REQ_T' should be ignored except execWriteFlag and connHandle.
     */
    CYBLE_EVT_GATTS_EXEC_WRITE_REQ,

    /** 'Execute Write' response from server device. Event parameter is a
       pointer to a structure of type CYBLE_GATTC_EXEC_WRITE_RSP_T. */
    CYBLE_EVT_GATTC_EXEC_WRITE_RSP,

    /** Notification data received from server device. Event parameter
       is a pointer to a structure of type CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T. */
    CYBLE_EVT_GATTC_HANDLE_VALUE_NTF,

    /** Indication data received from server device. Event parameter is
       a pointer to a structure of type CYBLE_GATTC_HANDLE_VALUE_IND_PARAM_T. */
    CYBLE_EVT_GATTC_HANDLE_VALUE_IND,

    /** Confirmation to indication response from client device. Event
       parameter is a pointer to a structure of type CYBLE_CONN_HANDLE_T. */
    CYBLE_EVT_GATTS_HANDLE_VALUE_CNF,

    /** Confirmation to indication response from client device. Event
       parameter is a pointer to a structure of type CYBLE_GATTS_SIGNED_WRITE_CMD_REQ_PARAM_T. 
       if value.val parameter is set to Zero, then signature is not matched and ignored by stack */
    CYBLE_EVT_GATTS_DATA_SIGNED_CMD_REQ,

    /** Event indicating that GATT group procedure has stopped or completed, this event occurs
       only if application has called CyBle_GattcStopCmd API. 
       Event parameters shall be ignored */
    CYBLE_EVT_GATTC_STOP_CMD_COMPLETE,

   /** Event parameter type is CYBLE_GATTS_CHAR_VAL_READ_REQ_T. It is triggered on server side 
       when client sends read request and when characteristic has CYBLE_GATT_DB_ATTR_CHAR_VAL_RD_EVENT 
       property set. This event could be ignored by application unless it need to response by error response which
       needs to be set in gattErrorCode field of event parameter. */
    CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ,

	/** Event indicates that GATT long procedure is end and stack will not send any further
    * requests to peer. Either this event or 'CYBLE_EVT_GATTC_ERROR_RSP' will be received
    * by application. This event may get triggered for below GATT long procedures:
    * 	1. CyBle_GattcDiscoverAllPrimaryServices
	* 	2. CyBle_GattcDiscoverPrimaryServiceByUuid
	* 	3. CyBle_GattcFindIncludedServices
	* 	4. CyBle_GattcDiscoverAllCharacteristics
	* 	5. CyBle_GattcDiscoverCharacteristicByUuid
	* 	6. CyBle_GattcDiscoverAllCharacteristicDescriptors
	* 	7. CyBle_GattcReadLongCharacteristicValues
	* 	8. CyBle_GattcReadLongCharacteristicDescriptors	\n
	* 	Event parameter is ATT opcode for the corresponding long GATT Procedure.
	*/
	CYBLE_EVT_GATTC_LONG_PROCEDURE_END,

    /* Range for L2CAP events - 0x70 to 0x7F */

    /** This event indicates the connection parameter update received
       from the remote device. The application is expected to reply to L2CAP using the
       CyBle_L2capLeConnectionParamUpdateResponse() function to respond to the remote
       device, whether parameters are accepted or rejected.
       
       Event Parameter pointer points to data of type 'CYBLE_GAP_CONN_UPDATE_PARAM_T' */
    CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ = 0x70u,

    /** This event indicates the connection parameter update response received
       from the master. Event Parameter pointer points to data with two possible values:
    	* Accepted = 0x0000
    	* Rejected  = 0x0001

       Data is of type unit16. */
    CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP,

    /** This event indicates that the request send over l2cap signaling has been
       rejected. Event parameter is a pointer to a structure of type
       CYBLE_L2CAP_COMMAND_REJ_REASON_T. */
    CYBLE_EVT_L2CAP_COMMAND_REJ,

    /** This event is used to inform application of the incoming L2CAP CBFC
       Connection Request. Event parameter is a pointer to a structure of type
       CYBLE_L2CAP_CBFC_CONN_IND_PARAM_T is returned. */
    CYBLE_EVT_L2CAP_CBFC_CONN_IND,

    /** This event is used to inform application of the L2CAP CBFC Connection
       Response/Confirmation. Event parameter is a pointer to a structure of
       type CYBLE_L2CAP_CBFC_CONN_CNF_PARAM_T is returned. */
    CYBLE_EVT_L2CAP_CBFC_CONN_CNF,

    /** This event is used to inform application of the L2CAP CBFC Disconnection
       Request received from the Peer device. Event parameter is a pointer to
       Local CID of type unit16. */
    CYBLE_EVT_L2CAP_CBFC_DISCONN_IND,

    /** This event is used to inform application of the L2CAP CBFC Disconnection
       confirmation/Response received from the Peer device. Event parameter is a
       pointer to a structure of type CYBLE_L2CAP_CBFC_DISCONN_CNF_PARAM_T. */
    CYBLE_EVT_L2CAP_CBFC_DISCONN_CNF,

    /** This event is used to inform application of data received over L2CAP
       CBFC channel. Event parameter is a pointer to a structure of type
       CYBLE_L2CAP_CBFC_RX_PARAM_T. */
    CYBLE_EVT_L2CAP_CBFC_DATA_READ,

    /** This event is used to inform the application of receive credits reached
       low mark. After receiving L2CAP data/payload from peer device for a
       specification Channel, the available credits are calculated.
       
       If the credit count goes below the low mark, this event is called to inform
       the application of the condition, so that if the application wants it can
       send more credits to the peer device.
       
       Event parameter is a pointer to a structure of type
       CYBLE_L2CAP_CBFC_LOW_RX_CREDIT_PARAM_T. */
    CYBLE_EVT_L2CAP_CBFC_RX_CREDIT_IND,

    /** This event is used to inform application of having received transmit
       credits. This event is called on receiving LE Flow Control Credit from peer
       device.
       
       Event parameter is a pointer to a structure of type
       CYBLE_L2CAP_CBFC_LOW_TX_CREDIT_PARAM_T.
       
       If the 'result' field of the received data is non-zero, this indicates an
       error. If the sum of 'credit' field value and the previously available credit
       at the peer device receiving credit information exceeds 65535, it indicates a
       'credit overflow' error.
       
       In case of error, the peer device receiving this event should initiate
       disconnection of the L2CAP channel by invoking CyBle_L2capDisconnectReq () 
       function. */
    CYBLE_EVT_L2CAP_CBFC_TX_CREDIT_IND,

    /** This event is used to inform application of data transmission completion over L2CAP CBFC
       channel. Event parameter is of type 'CYBLE_L2CAP_CBFC_DATA_WRITE_PARAM_T' 
       This event will be deprecated in future. It is only kept for backward compatibility.
       It is not recommended to be used by new design */
    CYBLE_EVT_L2CAP_CBFC_DATA_WRITE_IND,

#ifdef CYBLE_HOST_QUALIFICATION
	/** Tester to manipulate pairing request or response PDU. Event parameter is a pointer to 1 bytes data.
	    Tester can manipulate the bits of the byte */
	CYBLE_EVT_QUAL_SMP_PAIRING_REQ_RSP = 0x80u,

	/** Tester to manipulate local Public Key. Event parameter is a pointer to local public key of size 64 Bytes.
		Tester can manipulate the bits/bytes */
	CYBLE_EVT_QUAL_SMP_LOCAL_PUBLIC_KEY,

	/** Tester to assign pairing failed error code. Event parameter is a pointer to 16 bits value.
		Tester should assign error code to lower bits */
	CYBLE_EVT_QUAL_SMP_PAIRING_FAILED_CMD,

#endif /* CYBLE_HOST_QUALIFICATION */

    /*##Range for for future use - 0x90 to 0xFF*/

    /** This event is used to inform application that flash write is pending
        Stack internal data structures are modified and require backup. */
    CYBLE_EVT_PENDING_FLASH_WRITE = 0xFA,

    /** LE PING Authentication Timeout Event to indicate that peer device has not responded
        with the valid MIC packet within the application configured ping authentication time. */
    CYBLE_EVT_LE_PING_AUTH_TIMEOUT = 0xFB,

	/** Maximum value of CYBLE_EVENT_T type */
    CYBLE_EVT_MAX = 0xFF

}CYBLE_EVENT_T;

typedef struct
{
    const CYBLE_EVENT_T event;
    const char *event_name;
} event_pair;

static const event_pair lookup_table[] = 
{		
    { CYBLE_EVT_HOST_INVALID, "CYBLE_EVT_HOST_INVALID" },
    { CYBLE_EVT_STACK_ON, "CYBLE_EVT_STACK_ON" },
    { CYBLE_EVT_TIMEOUT, "CYBLE_EVT_TIMEOUT" },
    { CYBLE_EVT_HARDWARE_ERROR, "CYBLE_EVT_HARDWARE_ERROR" },
    { CYBLE_EVT_HCI_STATUS, "CYBLE_EVT_HCI_STATUS" },
    { CYBLE_EVT_STACK_BUSY_STATUS, "CYBLE_EVT_STACK_BUSY_STATUS" },
	{ CYBLE_EVT_MEMORY_REQUEST, "CYBLE_EVT_MEMORY_REQUEST" },
    { CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT, "CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT" },
    { CYBLE_EVT_GAP_AUTH_REQ, "CYBLE_EVT_GAP_AUTH_REQ" },
    { CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST, "CYBLE_EVT_GAP_PASSKEY_ENTRY_REQUEST" },
    { CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST, "CYBLE_EVT_GAP_PASSKEY_DISPLAY_REQUEST" },
    { CYBLE_EVT_GAP_AUTH_COMPLETE, "CYBLE_EVT_GAP_AUTH_COMPLETE" },
    { CYBLE_EVT_GAP_AUTH_FAILED, "CYBLE_EVT_GAP_AUTH_FAILED" },
    { CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP, "CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP" },
    { CYBLE_EVT_GAP_DEVICE_CONNECTED, "CYBLE_EVT_GAP_DEVICE_CONNECTED" },
    { CYBLE_EVT_GAP_DEVICE_DISCONNECTED, "CYBLE_EVT_GAP_DEVICE_DISCONNECTED" },
    { CYBLE_EVT_GAP_ENCRYPT_CHANGE, "CYBLE_EVT_GAP_ENCRYPT_CHANGE" },
    { CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE, "CYBLE_EVT_GAP_CONNECTION_UPDATE_COMPLETE" },
    { CYBLE_EVT_GAPC_SCAN_START_STOP, "CYBLE_EVT_GAPC_SCAN_START_STOP" },
    { CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT, "CYBLE_EVT_GAP_KEYINFO_EXCHNGE_CMPLT" },
    { CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST, "CYBLE_EVT_GAP_NUMERIC_COMPARISON_REQUEST" },
    { CYBLE_EVT_GAP_KEYPRESS_NOTIFICATION, "CYBLE_EVT_GAP_KEYPRESS_NOTIFICATION" },
    { CYBLE_EVT_GAP_OOB_GENERATED_NOTIFICATION, "CYBLE_EVT_GAP_OOB_GENERATED_NOTIFICATION" },
    { CYBLE_EVT_GAP_DATA_LENGTH_CHANGE, "CYBLE_EVT_GAP_DATA_LENGTH_CHANGE" },
    { CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE, "CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE" },
    { CYBLE_EVT_GAPC_DIRECT_ADV_REPORT, "CYBLE_EVT_GAPC_DIRECT_ADV_REPORT" },
    { CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO, "CYBLE_EVT_GAP_SMP_NEGOTIATED_AUTH_INFO" },
    { CYBLE_EVT_GATTC_ERROR_RSP, "CYBLE_EVT_GATTC_ERROR_RSP" },
    { CYBLE_EVT_GATT_CONNECT_IND, "CYBLE_EVT_GATT_CONNECT_IND" },
    { CYBLE_EVT_GATT_DISCONNECT_IND, "CYBLE_EVT_GATT_DISCONNECT_IND" },
    { CYBLE_EVT_GATTS_XCNHG_MTU_REQ, "CYBLE_EVT_GATTS_XCNHG_MTU_REQ" },
    { CYBLE_EVT_GATTC_XCHNG_MTU_RSP, "CYBLE_EVT_GATTC_XCHNG_MTU_RSP" },
    { CYBLE_EVT_GATTC_READ_BY_GROUP_TYPE_RSP, "CYBLE_EVT_GATTC_READ_BY_GROUP_TYPE_RSP" },
    { CYBLE_EVT_GATTC_READ_BY_TYPE_RSP, "CYBLE_EVT_GATTC_READ_BY_TYPE_RSP" },
    { CYBLE_EVT_GATTC_FIND_INFO_RSP, "CYBLE_EVT_GATTC_FIND_INFO_RSP" },
    { CYBLE_EVT_GATTC_FIND_BY_TYPE_VALUE_RSP, "CYBLE_EVT_GATTC_FIND_BY_TYPE_VALUE_RSP" },
    { CYBLE_EVT_GATTC_READ_RSP, "CYBLE_EVT_GATTC_READ_RSP" },
    { CYBLE_EVT_GATTC_READ_BLOB_RSP, "CYBLE_EVT_GATTC_READ_BLOB_RSP" },
    { CYBLE_EVT_GATTC_READ_MULTI_RSP, "CYBLE_EVT_GATTC_READ_MULTI_RSP" },
    { CYBLE_EVT_GATTS_WRITE_REQ, "CYBLE_EVT_GATTS_WRITE_REQ" },
    { CYBLE_EVT_GATTC_WRITE_RSP, "CYBLE_EVT_GATTC_WRITE_RSP" },
    { CYBLE_EVT_GATTS_WRITE_CMD_REQ, "CYBLE_EVT_GATTS_WRITE_CMD_REQ" },
    { CYBLE_EVT_GATTS_PREP_WRITE_REQ, "CYBLE_EVT_GATTS_PREP_WRITE_REQ" },
    { CYBLE_EVT_GATTS_EXEC_WRITE_REQ, "CYBLE_EVT_GATTS_EXEC_WRITE_REQ" },
    { CYBLE_EVT_GATTC_EXEC_WRITE_RSP, "CYBLE_EVT_GATTC_EXEC_WRITE_RSP" },
    { CYBLE_EVT_GATTC_HANDLE_VALUE_NTF, "CYBLE_EVT_GATTC_HANDLE_VALUE_NTF" },
    { CYBLE_EVT_GATTC_HANDLE_VALUE_IND, "CYBLE_EVT_GATTC_HANDLE_VALUE_IND" },
    { CYBLE_EVT_GATTS_HANDLE_VALUE_CNF, "CYBLE_EVT_GATTS_HANDLE_VALUE_CNF" },
    { CYBLE_EVT_GATTS_DATA_SIGNED_CMD_REQ, "CYBLE_EVT_GATTS_DATA_SIGNED_CMD_REQ" },
    { CYBLE_EVT_GATTC_STOP_CMD_COMPLETE, "CYBLE_EVT_GATTC_STOP_CMD_COMPLETE" },
    { CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ, "CYBLE_EVT_GATTS_READ_CHAR_VAL_ACCESS_REQ" },
	{ CYBLE_EVT_GATTC_LONG_PROCEDURE_END, "CYBLE_EVT_GATTC_LONG_PROCEDURE_END" },
    { CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ, "CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_REQ" },
    { CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP, "CYBLE_EVT_L2CAP_CONN_PARAM_UPDATE_RSP" },
    { CYBLE_EVT_L2CAP_COMMAND_REJ, "CYBLE_EVT_L2CAP_COMMAND_REJ" },
    { CYBLE_EVT_L2CAP_CBFC_CONN_IND, "CYBLE_EVT_L2CAP_CBFC_CONN_IND" },
    { CYBLE_EVT_L2CAP_CBFC_CONN_CNF, "CYBLE_EVT_L2CAP_CBFC_CONN_CNF" },
    { CYBLE_EVT_L2CAP_CBFC_DISCONN_IND, "CYBLE_EVT_L2CAP_CBFC_DISCONN_IND" },
    { CYBLE_EVT_L2CAP_CBFC_DISCONN_CNF, "CYBLE_EVT_L2CAP_CBFC_DISCONN_CNF" },
    { CYBLE_EVT_L2CAP_CBFC_DATA_READ, "CYBLE_EVT_L2CAP_CBFC_DATA_READ" },
    { CYBLE_EVT_L2CAP_CBFC_RX_CREDIT_IND, "CYBLE_EVT_L2CAP_CBFC_RX_CREDIT_IND" },
    { CYBLE_EVT_L2CAP_CBFC_TX_CREDIT_IND, "CYBLE_EVT_L2CAP_CBFC_TX_CREDIT_IND" },
    { CYBLE_EVT_L2CAP_CBFC_DATA_WRITE_IND, "CYBLE_EVT_L2CAP_CBFC_DATA_WRITE_IND" },
#ifdef CYBLE_HOST_QUALIFICATION
	CYBLE_EVT_QUAL_SMP_PAIRING_REQ_RSP = 0x80u,
	CYBLE_EVT_QUAL_SMP_LOCAL_PUBLIC_KEY,
	CYBLE_EVT_QUAL_SMP_PAIRING_FAILED_CMD,
#endif
    { CYBLE_EVT_PENDING_FLASH_WRITE, "CYBLE_EVT_PENDING_FLASH_WRITE" },
    { CYBLE_EVT_LE_PING_AUTH_TIMEOUT, "CYBLE_EVT_LE_PING_AUTH_TIMEOUT" },
    { CYBLE_EVT_MAX, "CYBLE_EVT_MAX" }
};

#define NUM_PAIRS (sizeof(lookup_table) / sizeof((lookup_table)[0]))

const char *hkj_ble_events_get_name(int event)
{    
    unsigned int i;
    for (i=0; i < NUM_PAIRS; i++)
        if (lookup_table[i].event == event)
            return lookup_table[i].event_name;
            
    return 0;
}	

#ifndef MAX_LOGGED_EVENTS
#define MAX_LOGGED_EVENTS 15
#endif

typedef struct
{
    uint32 time;
    uint32 event;
} time_event_pair;

time_event_pair eventlog[MAX_LOGGED_EVENTS];
uint eventlog_index = 0;

static hkj_timer t;

void hkj_ble_events_log_clear(void)
{
    eventlog_index = 0;
    hkj_timer_ms_reset_delta(&t); 
}

void hkj_ble_events_log_init(void)
{
    hkj_ble_events_log_clear();  
    hkj_timer_ms_init(&t);
}

void hkj_ble_events_log_add(int event, void *eventparam)
{
    (void)eventparam; /* Reserved for future use */
    if (eventlog_index < MAX_LOGGED_EVENTS)
    {
        eventlog[eventlog_index].event = event;
        eventlog[eventlog_index].time = hkj_timer_ms_get_delta(&t); 
        eventlog_index++;
    }
}

void hkj_ble_events_log_debug_print(void)
{
    uint i;
    for ( i = 0; i < eventlog_index; i++ )
    {
        const char *event_name = hkj_ble_events_get_name(eventlog[i].event);
        if (event_name)
            printf("EVENT @ %lu ms: %s\r\n", eventlog[i].time, event_name);
        else
            printf("EVENT @ %lu ms: UNKNOWN (%lu)\r\n", eventlog[i].time, eventlog[i].event);
    }
    if (eventlog_index == MAX_LOGGED_EVENTS)
        printf("EVENT @ ?? ms: EVENT LOG FULL!\r\n");
}