/**
 * @file nd06av1c_def.h
 * @author huiyang.jiang
 * @brief ND06AV1C's Macro definition and data structure
 * @version 1.x.x
 * @date 2023-5
 *
 * @copyright Copyright (c) 2023, Shenzhen Nephotonics Inc.
 *
 */
#ifndef __ND06AV1C__DEF__H__
#define __ND06AV1C__DEF__H__

#include <stdint.h>
#include <stdio.h>

#define DEBUG_INFO        0 /*!< µ÷ÊÔÐÅÏ¢´òÓ¡¿ª¹Ø */
#define NX_PRINTF(fmt, args...)   do{if(DEBUG_INFO) printf(fmt, ##args); }while(0) /*!< µ÷ÊÔ½Ó¿Ú£¬Ä¬ÈÏÊ¹ÓÃprintf */


/** @defgroup ND06AV1C_Global_Define_Group ND06AV1C Defines
 *	@brief	  ND06AV1C Defines
 *	@{
 */


/** @defgroup ND06AV1C_Reg_Group ND06AV1C Register Defines
 * @brief	  ND06AV1C Register Defines
 *  @{
 */

#define ND06AV1C_REG_BOOT1                    (0x55AA)  /*!< ND06AV1CÄ£×éµÚÒ»¶ÎÆô¶¯¼Ä´æÆ÷£¬ÐèÒª¶Ô¸Ã¼Ä´æÆ÷Ð´1byte£¬ÇÒÖµÎª0x38 */
#define ND06AV1C_REG_BOOT2                    (0x3BFC)  /*!< ND06AV1CÄ£×éµÚ¶þ¶ÎÆô¶¯¼Ä´æÆ÷£¬ÐèÒª¶Ô¸Ã¼Ä´æÆ÷Ð´4bytes£¬ÇÒÖµÎª0xFFFFFFF2 */
#define ND06AV1C_REG_BOOT3                    (0x0888)  /*!< ND06AV1CÄ£×éµÚÈý¶ÎÆô¶¯¼Ä´æÆ÷£¬ÐèÒª¶Ô¸Ã¼Ä´æÆ÷Ð´4bytes£¬ÇÒÖµÎª0x00000001 */
#define ND06AV1C_REG_BOOT4                    (0x3A1C)  /*!< ND06AV1CÄ£×éµÚËÄ¶ÎÆô¶¯¼Ä´æÆ÷£¬ÐèÒª¶Ô¸Ã¼Ä´æÆ÷Ð´4bytes£¬ÇÒÖµÎª0x00000030 */
#define ND06AV1C_REG_BOOT5                    (0x0808)  /*!< ND06AV1CÄ£×éµÚÎå¶ÎÆô¶¯¼Ä´æÆ÷£¬ÐèÒª¶Ô¸Ã¼Ä´æÆ÷Ð´4bytes£¬ÇÒÖµÎª0x00000001 */
#define ND06AV1C_REG_DEV_ID                   (0x3B80)  /*!< ND06AV1CÄ£×éÉè±¸ID */

/*<! RO */
#define ND06AV1C_REG_FW_VERSION                         (0xA700 + ND06AV1C_REG_OFS) /*!< ¹Ì¼þ°æ±¾¼Ä´æÆ÷ */
#define ND06AV1C_REG_STATE                              (0xA704 + ND06AV1C_REG_OFS) /*!< Ä£×é×´Ì¬¼Ä´æÆ÷ */
#define ND06AV1C_REG_ERROR                              (0xA708 + ND06AV1C_REG_OFS) /*!< ´íÎó¼Ä´æÆ÷ */
#define ND06AV1C_REG_ITOFDATA                           (0xA70C + ND06AV1C_REG_OFS) /*!< ÄÚ²¿itof½á¹¹ÌåµØÖ·¼Ä´æÆ÷ */
/*<! RW */
#define ND06AV1C_REG_EXP_THERM                          (0xA710 + ND06AV1C_REG_OFS) /*!< »ý·ÖÊ±¼äÎÂ¶È¼Ä´æÆ÷ */
#define ND06AV1C_REG_SLAVE_ADDR                         (0xA714 + ND06AV1C_REG_OFS) /*!< Ä£¿éi2cµØÖ·¼Ä´æÆ÷ */
#define ND06AV1C_REG_DATA_VAL_REQ                       (0xA718 + ND06AV1C_REG_OFS) /*!< Êý¾ÝÇëÇó¼Ä´æÆ÷ */
#define ND06AV1C_REG_UNIVERSAL_ADDR                     (0xA71C + ND06AV1C_REG_OFS) /*!< Í¨ÓÃ²Ù×÷µØÖ·¼Ä´æÆ÷ */
#define ND06AV1C_REG_UNIVERSAL_DATA                     (0xA720 + ND06AV1C_REG_OFS) /*!< Í¨ÓÃ²Ù×÷Êý¾Ý¼Ä´æÆ÷ */
#define ND06AV1C_REG_DRIVER_THERM                       (0xA72C + ND06AV1C_REG_OFS) /*!< Çý¶¯ÎÂ¶È¼Ä´æÆ÷ */
#define ND06AV1C_BOOT_FLAG_REG                          (0xA5FC + ND06AV1C_REG_OFS) /*!< bootÆô¶¯¼Ä´æÆ÷ */


/** @} ND06AV1C_Reg_Group */


/** @defgroup ND06AV1C_Boot_Group ND06AV1C Boot Value
 *	@brief	  ND06AV1C Boot  (ND06AV1C_REG_BOOT)
 *	@{
 */

#define ND06AV1C_REG_BOOT1_VALUE              0x38          /*!< ND06AV1CÄ£×éµÚÒ»¶ÎÆô¶¯Öµ£¬ÇÒÖµÎª0x38 */
#define ND06AV1C_REG_BOOT2_VALUE              0xFFFFFFF2    /*!< ND06AV1CÄ£×éµÚ¶þ¶ÎÆô¶¯Öµ£¬ÇÒÖµÎª0xFFFFFFF2 */
#define ND06AV1C_REG_BOOT3_VALUE              0x00000001    /*!< ND06AV1CÄ£×éµÚÈý¶ÎÆô¶¯Öµ£¬ÇÒÖµÎª0x00000001 */
#define ND06AV1C_REG_BOOT4_VALUE              0x00000030    /*!< ND06AV1CÄ£×éµÚËÄ¶ÎÆô¶¯Öµ£¬ÇÒÖµÎª0x00000030 */
#define ND06AV1C_REG_BOOT5_VALUE              0x00000001    /*!< ND06AV1CÄ£×éµÚÎå¶ÎÆô¶¯Öµ£¬ÇÒÖµÎª0x00000001 */
#define ND06AV1C_REG_DEV_ID_VALUE             0x002F0012    /*!< ND06AV1CÄ£×éÉè±¸IDÄ¬ÈÏÖµ */
#define ND06AV1C_BOOTFLAG_RUN                 0x5a004000    /*!< ÔËÐÐÄ£Ê½bootflag*/
#define ND06AV1C_BOOTFLAG_CONFIG              0x5a00c000    /*!< ÅäÖÃÄ£Ê½bootflag*/
#define ND06AV1C_BOOTFLAG_CALIB1              0x5a010000    /*!< ±ê¶¨Ä£Ê½1bootflag*/
#define ND06AV1C_BOOTFLAG_CALIB2              0x5a014000    /*!< ±ê¶¨Ä£Ê½2bootflag*/
/** @} ND06AV1C_Boot_Group */

/** @defgroup ND06AV1C_Calib Flag
 *	@{
 */

#define ND06AV1C_OFFSET_FLAG1                 0x55aa4000    /*!< offsetµÚÒ»Î»ÖÃ±ê¶¨flag*/
#define ND06AV1C_OFFSET_FLAG2                 0x55aa4001    /*!< offsetµÚ¶þÎ»ÖÃ±ê¶¨flag*/
#define ND06AV1C_OFFSET_NOLINE_FLAG           0x55aa2000    /*!< µ¥Î»ÖÃ±ê¶¨offset£¬·ÇÏßÐÔflag  */
#define ND06AV1C_OFFSET_ONLY_FLAG             0x55aa2001    /*!< Ö»±ê¶¨offset  */
#define ND06AV1C_OFFSET_NEAR_FLAG             0x55aa1000    /*!< ½ü¾àÀëoffset±ê¶¨flag  */
#define ND06AV1C_CALIB_NONE_FLAG              0
/** @} ND06AV1C_Calib Flag */


/** @defgroup ND06AV1C_State_Group ND06AV1C Data Request Index
 *	@brief	  ND06AV1C State  (ND06AV1C_REG_STATE)
 *	@{
 */
#define ND06AV1C_STATE_IDLE                     0x00000000
#define ND06AV1C_STATE_LOAD_CODE                0x000000A4
#define ND06AV1C_STATE_SOFTWARE_READY           0x000000A5
#define ND06AV1C_STATE_CALIB                    0x000000A6

/** @} ND06AV1C_State_Group */



/** @defgroup ND06AV1C_Data_Val_Req_Idx_Group ND06AV1C Data Request Index
 *	@brief	  ND06AV1C Data Request Mask (ND06AV1C_REG_DATA_VAL_REQ)
 *	@{
 */
// REG_DATA_REQ MASK
#define ND06AV1C_DEPTH_DATA_REQ_MASK             0x00000001  /*!< Éî¶ÈÊý¾Ý¶ÁÈ¡ÇëÇóÑÚÂë */
#define ND06AV1C_FW_SW_REQ_MASK                  0x00000002  /*!< ND06¹Ì¼þÇÐ»»ÇëÇóÑÚÂë */
#define ND06AV1C_OSC_DATA_WR_REQ_MASK            0x00000003  /*!< ÇëÇóÐ´Èëosc trim ÇëÇóÑÚÂë*/
#define ND06AV1C_FW_UP_REQ_MASK                  0x00000004  /*!< ¹Ì¼þ¸üÐÂÇëÇóÑÚÂë */
#define ND06AV1C_CALIB_REQ_MASK                  0x00000005  /*!< ±ê¶¨ÇëÇóÑÚÂë */
#define ND06AV1C_GET_CONFIG_ARRD_REQ_MASK        0x00000006  /*!< »ñÈ¡ËùÓÐÅäÖÃ²ÎÊýµØÖ·ÇëÇóÑÚÂë*/
#define ND06AV1C_SAVE_CONFIG_ARRD_REQ_MASK       0x00000007  /*!< ±£´æitofÊý¾Ý - ÇëÇóÑÚÂë*/
#define ND06AV1C_CALIB_RESET_REQ_MASK            0x0000000C  /*!< »Ö¸´³ö³§ÉèÖÃ-ÇëÇóÑÚÂë */

// REG_DATA_VAL MASK
#define ND06AV1C_DEPTH_DATA_VAL_MASK             0x00010000  /*!< Éî¶ÈÊý¾ÝÓÐÐ§Î»ÑÚÂë */
#define ND06AV1C_FW_SW_VAL_MASK                  0x00020000  /*!< ND06¹Ì¼þÇÐ»»-ÇëÇóÓÐÐ§ */
#define ND06AV1C_OSC_DATA_WR_VAL_MASK            0x00030000  /*!< Ð´Èëosc trim ÇëÇóÓÐÐ§*/
#define ND06AV1C_FW_UP_VAL_MASK                  0x00040000  /*!< ¹Ì¼þ¸üÐÂ-ÇëÇóÓÐÐ§ */
#define ND06AV1C_CALIB_VAL_MASK                  0x00050000  /*!< ±ê¶¨ÇëÇóÑÚÂë -ÇëÇóÓÐÐ§*/
#define ND06AV1C_GET_CONFIG_ARRD_VAL_MASK        0x00060000  /*!< »ñÈ¡ËùÓÐÅäÖÃ²ÎÊýµØÖ· -ÇëÇóÓÐÐ§*/
#define ND06AV1C_SAVE_CONFIG_ARRD_VAL_MASK       0x00070000  /*!< ±£´æitofÊý¾Ý - ÇëÇóÓÐÐ§*/
#define ND06AV1C_CALIB_RESET_VAL_MASK            0x000C0000  /*!< »Ö¸´³ö³§ÉèÖÃ-ÇëÇóÑÚÂë */



/** @defgroup ND06AV1C_Error_Group ND06AV1C Error Group
 *  @brief	  ND06AV1C Error Group (ND06AV1C_REG_ERROR_FLAG)
 *	@{
 */

#define ND06AV1C_DATA_READY				   				2	/*Ä£¿éÒÑ¾­×¼±¸ºÃÊý¾Ý£*/
#define ND06AV1C_GET_DATA_SUCCESS				   		1	/*»ñÈ¡Êý¾Ý³É¹¦*/
#define ND06AV1C_ERROR_NONE                            0    /*!< ³É¹¦ */
#define ND06AV1C_ERROR_TIME_OUT                       -2    /*!< ³¬Ê±´íÎó */
#define ND06AV1C_ERROR_I2C			                      -3    /*!< IICÍ¨Ñ¶´íÎó */
#define ND06AV1C_ERROR_FW                             -5    /*!< ¹Ì¼þ°æ±¾¼æÈÝ´íÎó */
#define ND06AV1C_ERROR_BOOT                           -6    /*!< Ä£×éÆô¶¯´íÎó */
#define ND06AV1C_ERROR_PARAMETER                      -8    /*!< ²ÎÊý´íÎó */
#define ND06AV1C_ERROR_EXP                            -11   /*!< ×Ô¶¯ÆØ¹âµ÷½ÚÊ§°Ü */
#define ND06AV1C_ERROR_CALIB_DATA                     -12   /*!< Ã»ÓÐ±ê¶¨ÐÅÏ¢ */
#define ND06AV1C_ERROR_DATAERR                        -13   /*!< Ô­Ê¼Êý¾ÝÒì³£ */
#define ND06AV1C_ERROR_OVEREXP                        65500 /*!< ¹ýÆØ¹â */
#define ND06AV1C_ERROR_UNDEREXP                       65300 /*!< Ç·ÆØ */

/** @} ND06AV1C_Error_Group */


/** @defgroup ND06AV1C_Slave_Address_Group ND06AV1C Device Mode
 *  @brief  ND06AV1C Slave Address (ND06AV1C_REG_Slave_ADDR)
 *	@{
 */

#define ND06AV1C_DEFAULT_SLAVE_ADDR             0x5B  /*!< ND06AV1CÄ¬ÈÏIICµØÖ·(7bit) */

/** @} ND06AV1C Slave Address */

/** @} ND06AV1C_Global_Define_Group */

/** @enum  ND06AV1C_Status_e
 *  @brief ¶¨ÒåND06AV1C×´Ì¬ºê
 */
typedef enum{
    ND06AV1C_DISABLE = 0,   ///< ¹Ø±Õ×´Ì¬
    ND06AV1C_ENABLE  = 1    ///< Ê¹ÄÜ×´Ì¬
  } ND06AV1C_Status_e;


/**
  * @struct ND06AV1C_ChipInfo_t
  *
  * @brief ND06AV1CÄ£×éÉú²úÐÅÏ¢\n
  */
 typedef struct {
    uint32_t  nd06av1c_fw_version;        ///< ND06AV1C¹Ì¼þ°æ±¾
 } ND06AV1C_ChipInfo_t;


/**
  * @struct ND06AV1C_Dev_t
  *
  * @brief Éè±¸ÀàÐÍ½á¹¹Ìå\n
  */
typedef struct {
    void(*SetXShutPinLevelFunc)(uint32_t );   ///< Èç¹ûÉèÖÃXShutµçÆ½º¯ÊýÖ¸Õë£¬²ÎÊýÖµÎª0£¬ÔòÊä³öµÍµçÆ½£¬·ñÔòÊä³ö¸ßµçÆ½
    ND06AV1C_ChipInfo_t   chip_info;             ///< Ä£×éÉú²úÐÅÏ¢
    uint8_t           i2c_dev_addr;           ///< i2cÉè±¸µØÖ·
    uint32_t          dev_pwr_state;          ///< Éè±¸µÄµ±Ç°×´Ì¬, ¾ÍÐ÷Ä£Ê½»òÕßË¯ÃßÄ£Ê½
} ND06AV1C_Dev_t;


/**
  * @struct ND06AV1C_Fw_t
  *
  * @brief Éè±¸¹Ì¼þ½á¹¹Ìå\n
  */
typedef struct {
    uint8_t *fw_user;
    uint8_t *fw_config;
    uint8_t *fw_calib;
    uint8_t *fw_calib2;
    uint8_t *fw_init;
    uint32_t fw_len[5];
} ND06AV1C_Fw_t;

/**
 * @struct ND06AV1C_Func_Ptr_t
 *
 * @brief Á´½ÓÆ½Ì¨µÄAPI\n
 */
typedef struct{
	int32_t(*I2C_WriteNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t); ///< i2cÐ´º¯Êý
	int32_t(*I2C_ReadNBytesFunc)(uint8_t, uint16_t, uint8_t *, uint8_t); ///< i2c¶Áº¯Êý
	void(*Delay1msFunc)(uint32_t );   ///< ÑÓÊ±1msº¯Êý
	void(*Delay10usFunc)(uint32_t );  ///< ÑÓÊ±10usº¯Êý
  uint64_t(*GetNowTimeUsFunc)(void );   ///< »ñÈ¡µ±Ç°ÏµÍ³Ê±¼äº¯Êý
}ND06AV1C_Func_Ptr_t;


typedef enum
{
  //±ØÐë±£´æµ½flash £¬ÖØÆôÄ£×éÖ®ºóÉúÐ§
  ND06AV1C_CONFIG_VCSEL_CURRENT               = 0,

  //Á¢¼´¿ÉÒÔÉúÐ§£¬Èç¹û²»½øÐÐ±£´æÔò½öÕë¶Ô´Ë´Î³õÊ¼»¯£¬ÖØÐÂ³õÊ¼»¯Ä£×éÖ®ºó»á»Ö¸´ÎªflashÖµ
  ND06AV1C_CONFIG_CALIB_DIS                   = 1,  //offset±ê¶¨¾àÀë
  ND06AV1C_CONFIG_AUTO_EXP                    = 2,  //×Ô¶¯ÆØ¹â±êÖ¾Î»
  ND06AV1C_CONFIG_MAX_EXP                     = 3,  //×î´ó»ý·ÖÊ±¼ä
  ND06AV1C_CONFIG_PHS_TAG                     = 4,  //×Ô¶¯ÆØ¹âÄ¿±êÖµ
  ND06AV1C_CONFIG_SAMPLING                    = 5,  //ÆØ¹âÀÛ¼Ó´ÎÊý
  ND06AV1C_CONFIG_FRAME_DELAY                 = 6,  //Ö¡ÑÓÊ±
  ND06AV1C_CONFIG_DIS_UNCY                    = 8,  //¾àÀëÄ£ºý´¦Àí±êÖ¾Î»
  ND06AV1C_CONFIG_HDR_AMP                     = 12, //hdrÄ¿±ê·ù¶È
  ND06AV1C_CONFIG_PC_FLAG                     = 13, //µãÔÆÊä³ö±êÖ¾Î»
  ND06AV1C_CONFIG_LIGHT                       = 10, //»·¾³¹âÇ¿¶È / ÅäÖÃ
  ND06AV1C_CONFIG_FILTER_TIME                 = 15, //Ä£×éÄÚ²¿ÂË²¨Æ÷ÅäÖÃ
  ND06AV1C_CONFIG_FILTER_Q                    = 16, //Ä£×éÄÚ²¿ÂË²¨Æ÷ÅäÖÃ
  ND06AV1C_CONFIG_FILTER_R                    = 17, //Ä£×éÄÚ²¿ÂË²¨Æ÷ÅäÖÃ
  ND06AV1C_CONFIG_FILTER_RB                   = 18, //Ä£×éÄÚ²¿ÂË²¨Æ÷ÅäÖÃ
  ND06AV1C_CONFIG_LIGHT_EXP_PRO               = 19, //»·¾³¹â³¡¾°»ý·ÖÊ±¼ä±¶ÂÊ
}ND06AV1C_Config_Options;

#endif

