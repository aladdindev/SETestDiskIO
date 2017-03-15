/**
 *  \file se3_common.h
 *  \brief This file contains defines and functions common for L0 and L1
 * 
 */

#pragma once

#include "se3c0def.h"
#include "aes256.h"
#include "sha256.h"
#include "pbkdf2.h"

extern const uint8_t se3_magic[SE3_MAGIC_SIZE];

/**
 *  \brief Compute length of data in a request in terms of SE3_COMM_BLOCK blocks
 *  
 *  \param [in] len_data_and_headers Data length
 *  \return Number of SE3_COMM_BLOCK blocks
 *  
 */
uint16_t se3_req_len_data(uint16_t len_data_and_headers);

/**
 *  \brief Compute length of data in a request accounting for headers
 *  
 *  \param [in] len_data Data length
 *  \return Number of Bytes
 *  
 */
uint16_t se3_req_len_data_and_headers(uint16_t len_data);

/**
 *  \brief Compute length of data in a request in terms of SE3_COMM_BLOCK blocks
 *  
 *  \param [in] len_data_and_headers Data length
 *  \return Number of SE3_COMM_BLOCK blocks
 *  
 */
uint16_t se3_resp_len_data(uint16_t len_data_and_headers);

/**
 *  \brief Compute length of data in a response accounting for headers
 *  
 *  \param [in] len_data Data Length
 *  \return Number of Bytes
 *  
 */
uint16_t se3_resp_len_data_and_headers(uint16_t len_data);

/**
 *  \brief Compute number of SE3_COMM_BLOCK blocks, given length in Bytes
 *  
 *  \param [in] len Length
 *  \return Number of Blocks
 *  
 */
uint16_t se3_nblocks(uint16_t len);

/*
enum {
	SE3_PAYL_CRYPTO = 1,
	SE3_PAYL_SIGN = 2,
	SE3_PAYL_CRYPTO_SIGN = 3
};*/

typedef struct se3_payload_cryptoctx_ {
	B5_tAesCtx aesenc;
    B5_tAesCtx aesdec;
	B5_tHmacSha256Ctx hmac;
	uint8_t hmac_key[B5_AES_256];
    uint8_t auth[B5_SHA256_DIGEST_SIZE];
} se3_payload_cryptoctx;
void se3_payload_cryptoinit(se3_payload_cryptoctx* ctx, const uint8_t* key);
void se3_payload_encrypt(se3_payload_cryptoctx* ctx, uint8_t* auth, uint8_t* iv, uint8_t* data, uint16_t nblocks, uint16_t flags);
bool se3_payload_decrypt(se3_payload_cryptoctx* ctx, const uint8_t* auth, const uint8_t* iv, uint8_t* data, uint16_t nblocks, uint16_t flags);
