/*******************************************************************************
*
* FILE: 
* 		command_handler.h
*
* DESCRIPTION: 
* 	    Processes commands sent to the flight computer to test hardware and 
*       software or exercise low level control over computer hardware 
*
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef COMMAND_HANDLER_H 
#define COMMAND_HANDLER_H 

#ifdef __cplusplus
extern "C" {
#endif


/*------------------------------------------------------------------------------
 Macros/Typedefs
------------------------------------------------------------------------------*/

/* Command codes */
typedef enum _COMMAND_CODE
    {
    COMMAND_PING_CODE = 0x01, 
    COMMAND_CONNECT_CODE    ,
    COMMAND_IGNITE_CODE     ,
    COMMAND_FLASH_CODE      ,
    COMMAND_SENSOR_CODE     
    } COMMAND_CODE;

/* Commands with subcommand boolean indicators */
#define SUBCOMMAND_REQUIRED        true
#define SUBCOMMAND_NOT_REQUIRED    false

/* Ignition system command interface subcommand codes */
typedef enum _IGN_SUBCOMMAND
	{
	IGN_MAIN_DEPLOY_SUBCOMMAND = 0x01,
	IGN_DROGUE_DEPLOY_SUBCOMMAND     ,
	IGN_CONT_SUBCOMMAND              ,
	IGN_NONE_SUBCOMMAND     
	} IGN_SUBCOMMAND;

/* Flash command interface subcommand codes */
typedef enum _FLASH_SUBCOMMAND
    {
	FLASH_READ_SUBCOMMAND = 0,
	FLASH_ENABLE_SUBCOMMAND  ,
	FLASH_DISABLE_SUBCOMMAND ,
	FLASH_WRITE_SUBCOMMAND   ,
	FLASH_ERASE_SUBCOMMAND   ,
	FLASH_STATUS_SUBCOMMAND  ,
	FLASH_EXTRACT_SUBCOMMAND 
    } FLASH_SUBCOMMAND;

/* Sensor command interface subcommand codes */
typedef enum _SENSOR_SUBCOMMAND 
    {
    SENSOR_DUMP_SUBCOMMAND = 0x01,
    SENSOR_POLL_SUBCOMMAND       ,
    SENSOR_NONE_SUBCOMMAND
    } SENSOR_SUBCOMMAND;


/*------------------------------------------------------------------------------
 Function Prototypes 
------------------------------------------------------------------------------*/

/* Parses and executes board-level commands */
COMMAND_STATUS command_handler 
    (
    COMMAND_CODE command
    );

/* Parses and executes ignition module subcommands */
COMMAND_STATUS ign_cmd_handler
	(
    IGN_SUBCOMMAND subcommand 
    );

/* Parses and executes flash module subcommands */
COMMAND_STATUS flash_cmd_handler
	(
    FLASH_SUBCOMMAND subcommand   
    );

/* Parses and executes sensor module subcommands */
COMMAND_STATUS sensor_cmd_handler 
	(
    SENSOR_SUBCOMMAND subcommand 
    );


#ifdef __cplusplus
}
#endif

#endif /* COMMAND_HANDLER_H */

/*******************************************************************************
* END OF FILE                                                                  *
*******************************************************************************/