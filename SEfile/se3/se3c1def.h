/**
 *  \file se3c1def.h
 *  \brief This file contains defines to be used both for L1 and L0 
 *  	   functions
 */
#pragma once

#include "se3c0def.h"

/** L1 errors */
enum {
    SE3_ERR_ACCESS = 100,  ///< insufficient privileges
    SE3_ERR_PIN = 101,  ///< pin rejected
    SE3_ERR_RESOURCE = 200,  ///< resource not found
    SE3_ERR_EXPIRED = 201,  ///< resource expired
    SE3_ERR_MEMORY = 400,  ///< no more space to allocate resource
	SE3_ERR_AUTH =	401	   ///< SHA256HMAC Authentication failed
};
/**
 *   @defgroup AccessLogin
 *   @{
 *  	@brief Use this values as access parameter
 *  		   when using L1_login
 */
enum {
    SE3_ACCESS_USER = 100,	
    SE3_ACCESS_ADMIN = 1000,
    SE3_ACCESS_MAX = 0xFFFF
};
/**
 *  @}
 */


/** Configuration records definitions */
enum {
	SE3_RECORD_SIZE = 32,
	SE3_RECORD_MAX = 2
};

/** Default configuration record types */
enum {
    SE3_RECORD_TYPE_ADMINPIN = 0,
    SE3_RECORD_TYPE_USERPIN = 1
};

/** L1 field size definitions */
enum {
    SE3_L1_PIN_SIZE = 32,
    SE3_L1_KEY_SIZE = 32,
    SE3_L1_AUTH_SIZE = 16,
    SE3_L1_CRYPTOBLOCK_SIZE = 16,
    SE3_L1_CHALLENGE_SIZE = 32,
	SE3_L1_CHALLENGE_ITERATIONS = 32,
    SE3_L1_IV_SIZE = 16,
    SE3_L1_TOKEN_SIZE = 16
};

/** L1 request fields definitions */
enum {
    SE3_REQ1_OFFSET_AUTH = 0,
    SE3_REQ1_OFFSET_IV = 16,
    SE3_REQ1_OFFSET_TOKEN = 32,
    SE3_REQ1_OFFSET_LEN = 48,
    SE3_REQ1_OFFSET_CMD = 50,
    SE3_REQ1_OFFSET_DATA = 64,

    SE3_REQ1_MAX_DATA = (SE3_REQ_MAX_DATA - SE3_REQ1_OFFSET_DATA)
};


/** L1 response fields definitions */
enum {
    SE3_RESP1_OFFSET_AUTH = 0,
    SE3_RESP1_OFFSET_IV = 16,
    SE3_RESP1_OFFSET_TOKEN = 32,
    SE3_RESP1_OFFSET_LEN = 48,
    SE3_RESP1_OFFSET_STATUS = 50,
    SE3_RESP1_OFFSET_DATA = 64,

    SE3_RESP1_MAX_DATA = (SE3_RESP_MAX_DATA - SE3_RESP1_OFFSET_DATA)
};

/** L1 command codes */
enum {
    SE3_CMD1_CHALLENGE = 1,
    SE3_CMD1_LOGIN = 2,
    SE3_CMD1_LOGOUT = 3,
    SE3_CMD1_CONFIG = 4,
	SE3_CMD1_KEY_EDIT = 5,
	SE3_CMD1_KEY_LIST = 6,
	SE3_CMD1_CRYPTO_INIT = 7,
	SE3_CMD1_CRYPTO_UPDATE = 8,
    SE3_CMD1_CRYPTO_LIST = 9,
    SE3_CMD1_CRYPTO_SET_TIME = 10

};

/** L1_config operations */
enum {
    SE3_CONFIG_OP_GET = 1,
    SE3_CONFIG_OP_SET = 2
};

/** L1_config fields */
enum {
    SE3_CMD1_CONFIG_REQ_OFF_ID = 0,
    SE3_CMD1_CONFIG_REQ_OFF_OP = 2,
    SE3_CMD1_CONFIG_REQ_OFF_VALUE = 4,
    SE3_CMD1_CONFIG_RESP_OFF_VALUE = 0
};

/** L1_challenge fields */
enum {
    SE3_CMD1_CHALLENGE_REQ_OFF_CC1 = 0,
    SE3_CMD1_CHALLENGE_REQ_OFF_CC2 = 32,
    SE3_CMD1_CHALLENGE_REQ_OFF_ACCESS = 64,
    SE3_CMD1_CHALLENGE_REQ_SIZE = 66,
    SE3_CMD1_CHALLENGE_RESP_OFF_SC = 0,
    SE3_CMD1_CHALLENGE_RESP_OFF_SRESP = 32,
    SE3_CMD1_CHALLENGE_RESP_SIZE = 64
};

/** L1_login fields */
enum {
    SE3_CMD1_LOGIN_REQ_OFF_CRESP = 0,
    SE3_CMD1_LOGIN_REQ_SIZE = 32,
    SE3_CMD1_LOGIN_RESP_OFF_TOKEN = 0,
    SE3_CMD1_LOGIN_RESP_SIZE = 16
};

/** Keys: maximum sizes for variable fields */
enum {
	SE3_KEY_DATA_MAX = 2048,
	SE3_KEY_NAME_MAX = 32,
	SE3_KEY_FINGERPRINT_SIZE = 32,
	SE3_KEY_SALT_SIZE = 32
};

/**
 *  @defgroup KeyOpEdit
 *  @{
 *  @brief Use these values when using \ref L1_key_edit
 */
enum {
    SE3_KEY_OP_INSERT = 1,		/**< Use this value to insert a new key*/
    SE3_KEY_OP_DELETE = 2,		/**< Use this value to delete a new key*/
    SE3_KEY_OP_UPSERT = 3		/**< Use this value to update/insert a key*/
};
/**
 *  @}
 */

/** L1_key_edit fields */
enum {
    SE3_CMD1_KEY_EDIT_REQ_OFF_OP = 0,
    SE3_CMD1_KEY_EDIT_REQ_OFF_ID = 2,
    SE3_CMD1_KEY_EDIT_REQ_OFF_VALIDITY = 6,
    SE3_CMD1_KEY_EDIT_REQ_OFF_DATA_LEN = 10,
    SE3_CMD1_KEY_EDIT_REQ_OFF_NAME_LEN = 12,
    SE3_CMD1_KEY_EDIT_REQ_OFF_DATA_AND_NAME = 14
};


/** L1_key_list fields */
enum {
    SE3_CMD1_KEY_LIST_REQ_SIZE = 36,
    SE3_CMD1_KEY_LIST_REQ_OFF_SKIP = 0,
    SE3_CMD1_KEY_LIST_REQ_OFF_NMAX = 2,
	SE3_CMD1_KEY_LIST_REQ_OFF_SALT = 4,
    SE3_CMD1_KEY_LIST_RESP_OFF_COUNT = 0,
    SE3_CMD1_KEY_LIST_RESP_OFF_KEYINFO = 2,

    SE3_CMD1_KEY_LIST_KEYINFO_OFF_ID = 0,
    SE3_CMD1_KEY_LIST_KEYINFO_OFF_VALIDITY = 4,
    SE3_CMD1_KEY_LIST_KEYINFO_OFF_DATA_LEN = 8,
    SE3_CMD1_KEY_LIST_KEYINFO_OFF_NAME_LEN = 10,
	SE3_CMD1_KEY_LIST_KEYINFO_OFF_FINGERPRINT = 12,
    SE3_CMD1_KEY_LIST_KEYINFO_OFF_NAME = 44
};

/** Invalid handle values */
enum {
	SE3_ALGO_INVALID = 0xFFFF,
	SE3_SESSION_INVALID = 0xFFFFFFFF,
	SE3_KEY_INVALID = 0xFFFFFFFF
};


/**
 *  \defgroup AlgorithmAvail
 *  @{
 */
enum {
    SE3_ALGO_AES = 0,  ///< AES
	SE3_ALGO_SHA256 = 1,  ///< SHA256
	SE3_ALGO_HMACSHA256 = 2,  ///< HMAC-SHA256
	SE3_ALGO_AES_HMACSHA256 = 3,  ///< AES + HMAC-SHA256
	SE3_ALGO_AES_HMAC = 4,		///< AES 256 + HMAC Auth TODO remove

    SE3_ALGO_MAX = 8
};
/**
 *  @}
 */

/** L1_crypto_init fields */
enum {
    SE3_CMD1_CRYPTO_INIT_REQ_SIZE = 8,
    SE3_CMD1_CRYPTO_INIT_REQ_OFF_ALGO = 0,
    SE3_CMD1_CRYPTO_INIT_REQ_OFF_MODE = 2,
    SE3_CMD1_CRYPTO_INIT_REQ_OFF_KEY_ID = 4,
    SE3_CMD1_CRYPTO_INIT_RESP_SIZE = 4,
    SE3_CMD1_CRYPTO_INIT_RESP_OFF_SID = 0
};

/** L1_crypto_update fields */
enum {
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_SID = 0,
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_FLAGS = 4,
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN1_LEN = 6,
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATAIN2_LEN = 8,
    SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA = 16,

    SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATAOUT_LEN = 0,
    SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATA = 16
};

/** L1_crypto_update default flags */
enum {
	SE3_CRYPTO_FLAG_FINIT = (1 << 15),
	SE3_CRYPTO_FLAG_RESET = (1 << 14),
	SE3_CRYPTO_FLAG_SETIV = SE3_CRYPTO_FLAG_RESET,
	SE3_CRYPTO_FLAG_SETNONCE = (1 << 13),
	SE3_CRYPTO_FLAG_AUTH = (1 << 12)
};

/** L1_crypto_update maximum buffer sizes */
enum {
    SE3_CRYPTO_MAX_DATAIN = (SE3_REQ1_MAX_DATA - SE3_CMD1_CRYPTO_UPDATE_REQ_OFF_DATA),
    SE3_CRYPTO_MAX_DATAOUT = (SE3_RESP1_MAX_DATA - SE3_CMD1_CRYPTO_UPDATE_RESP_OFF_DATA)
};

/** L1_crypto_set_time fields */
enum {
    SE3_CMD1_CRYPTO_SET_TIME_REQ_SIZE = 4,
    SE3_CMD1_CRYPTO_SET_TIME_REQ_OFF_DEVTIME = 0
};

/** L1_crypto_list fields */
enum {
    SE3_CMD1_CRYPTO_LIST_REQ_SIZE = 0,
    SE3_CMD1_CRYPTO_LIST_RESP_OFF_COUNT = 0,
    SE3_CMD1_CRYPTO_LIST_RESP_OFF_ALGOINFO = 2,

    SE3_CMD1_CRYPTO_ALGOINFO_SIZE = 22,
    SE3_CMD1_CRYPTO_ALGOINFO_OFF_NAME = 0,
    SE3_CMD1_CRYPTO_ALGOINFO_OFF_TYPE = 16,
    SE3_CMD1_CRYPTO_ALGOINFO_OFF_BLOCK_SIZE = 18,
    SE3_CMD1_CRYPTO_ALGOINFO_OFF_KEY_SIZE = 20,

    SE3_CMD1_CRYPTO_ALGOINFO_NAME_SIZE = 16
};

/** L1_crypto_list default cipher types */
enum {
	SE3_CRYPTO_TYPE_BLOCKCIPHER = 0,
	SE3_CRYPTO_TYPE_STREAMCIPHER = 1,
	SE3_CRYPTO_TYPE_DIGEST = 2,
	SE3_CRYPTO_TYPE_BLOCKCIPHER_AUTH = 3,
	SE3_CRYPTO_TYPE_OTHER = 0xFFFF
};

#define SE3_DIR_SHIFT (8)
/**  \brief L1_crypto_init default modes
  *  
  *  One FEEDBACK and one DIR may be combined to specify the desired mode
  *  Example:
  *     Encrypt in CBC mode
  *     (SE3_FEEDBACK_CBC | SE3_DIR_ENCRYPT) 
  */
enum {
	SE3_FEEDBACK_ECB = 1,
	SE3_FEEDBACK_CBC = 2,
	SE3_FEEDBACK_OFB = 3,
	SE3_FEEDBACK_CTR = 4,
	SE3_FEEDBACK_CFB = 5,

	SE3_DIR_ENCRYPT = (1 << SE3_DIR_SHIFT),
	SE3_DIR_DECRYPT = (2 << SE3_DIR_SHIFT)
};


// required for in-place encryption with AES
#if (SE3_CRYPTO_MAX_DATAIN % 16 != 0)
#error "SE3_CRYPTO_MAX_DATAIN is not a multiple of 16"
#endif
#if (SE3_CRYPTO_MAX_DATAOUT % 16 != 0)
#error "SE3_CRYPTO_MAX_DATAIN is not a multiple of 16"
#endif
#if (SE3_REQ1_MAX_DATA % 16 != 0)
#error "SE3_REQ1_MAX_DATA is not a multiple of 16"
#endif
#if (SE3_RESP1_MAX_DATA % 16 != 0)
#error "SE3_RESP1_MAX_DATA is not a multiple of 16"
#endif
#if (SE3_REQ1_OFFSET_DATA % 16 != 0)
#error "SE3_REQ1_OFFSET_DATA is not a multiple of 16"
#endif
#if (SE3_RESP1_OFFSET_DATA % 16 != 0)
#error "SE3_RESP1_OFFSET_DATA is not a multiple of 16"
#endif
