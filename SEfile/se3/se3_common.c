#include "se3_common.h"


const uint8_t se3_magic[SE3_MAGIC_SIZE] = {
    0x3c, 0xab, 0x78, 0xb6, 0x2, 0x64, 0x47, 0xe9, 0x30, 0x26, 0xd4, 0x1f, 0xad, 0x68, 0x22, 0x27,
    0x41, 0xa4, 0x32, 0xba, 0xbe, 0x54, 0x83, 0xee, 0xab, 0x6b, 0x62, 0xce, 0xf0, 0x5c, 0x7, 0x91
};


uint16_t se3_req_len_data(uint16_t len_data_and_headers)
{
    uint16_t nblocks;
    if (len_data_and_headers < SE3_REQ_SIZE_HEADER) {
        return 0;
    }
    nblocks = len_data_and_headers/SE3_COMM_BLOCK;
    if (len_data_and_headers % SE3_COMM_BLOCK != 0) {
        nblocks++;
    }
    if (nblocks == 0)return 0;
    return len_data_and_headers - SE3_REQ_SIZE_HEADER - (nblocks - 1)*SE3_REQDATA_SIZE_HEADER;
}

uint16_t se3_req_len_data_and_headers(uint16_t len_data)
{
    uint16_t ndatablocks;
    if (len_data <= SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER) {
        return len_data + SE3_REQ_SIZE_HEADER;
    }
    len_data -= (SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER);
    ndatablocks = len_data / (SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER);
    if (len_data % (SE3_COMM_BLOCK - SE3_REQDATA_SIZE_HEADER) != 0) {
        ndatablocks++;
    }
    return SE3_COMM_BLOCK + len_data + ndatablocks*SE3_REQDATA_SIZE_HEADER;
}

uint16_t se3_resp_len_data(uint16_t len_data_and_headers)
{
    uint16_t nblocks;
    if (len_data_and_headers < SE3_RESP_SIZE_HEADER) {
        return 0;
    }
    nblocks = len_data_and_headers / SE3_COMM_BLOCK;
    if (len_data_and_headers % SE3_COMM_BLOCK != 0) {
        nblocks++;
    }
    if (nblocks == 0)return 0;
    return len_data_and_headers - SE3_RESP_SIZE_HEADER - (nblocks - 1)*SE3_RESPDATA_SIZE_HEADER;
}

uint16_t se3_resp_len_data_and_headers(uint16_t len_data)
{
    uint16_t ndatablocks;
    if (len_data <= SE3_COMM_BLOCK - SE3_RESP_SIZE_HEADER) {
        return len_data + SE3_RESP_SIZE_HEADER;
    }
    len_data -= (SE3_COMM_BLOCK - SE3_REQ_SIZE_HEADER);
    ndatablocks = len_data / (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER);
    if (len_data % (SE3_COMM_BLOCK - SE3_RESPDATA_SIZE_HEADER) != 0) {
        ndatablocks++;
    }
    return SE3_COMM_BLOCK + len_data + ndatablocks*SE3_RESPDATA_SIZE_HEADER;
}

uint16_t se3_nblocks(uint16_t len)
{
    uint16_t nblocks = len / SE3_COMM_BLOCK;
    if (len%SE3_COMM_BLOCK != 0) {
        nblocks++;
    }
    return nblocks;
}

void se3_payload_cryptoinit(se3_payload_cryptoctx* ctx, const uint8_t* key)
{
	uint8_t keys[2 * B5_AES_256];
	PBKDF2HmacSha256(key, B5_AES_256, NULL, 0, 1, keys, 2 * B5_AES_256);
    B5_Aes256_Init(&(ctx->aesenc), keys, B5_AES_256, B5_AES256_CBC_ENC);
    B5_Aes256_Init(&(ctx->aesdec), keys, B5_AES_256, B5_AES256_CBC_DEC);
	memcpy(ctx->hmac_key, keys + B5_AES_256, B5_AES_256);
	memset(keys, 0, 2 * B5_AES_256);
}
void se3_payload_encrypt(se3_payload_cryptoctx* ctx, uint8_t* auth, uint8_t* iv, uint8_t* data, uint16_t nblocks, uint16_t flags)
{
    if (flags & SE3_CMDFLAG_ENCRYPT) {
        B5_Aes256_SetIV(&(ctx->aesenc), iv);
        B5_Aes256_Update(&(ctx->aesenc), data, data, nblocks);
    }

    if (flags & SE3_CMDFLAG_SIGN) {
        B5_HmacSha256_Init(&(ctx->hmac), ctx->hmac_key, B5_AES_256);
        B5_HmacSha256_Update(&(ctx->hmac), iv, B5_AES_IV_SIZE);
        B5_HmacSha256_Update(&(ctx->hmac), data, nblocks*B5_AES_BLK_SIZE);
        B5_HmacSha256_Finit(&(ctx->hmac), ctx->auth);
        memcpy(auth, ctx->auth, 16);
    }
    else {
        memset(auth, 0, 16);
    }
}

bool se3_payload_decrypt(se3_payload_cryptoctx* ctx, const uint8_t* auth, const uint8_t* iv, uint8_t* data, uint16_t nblocks, uint16_t flags)
{
    if (flags & SE3_CMDFLAG_SIGN) {
        B5_HmacSha256_Init(&(ctx->hmac), ctx->hmac_key, B5_AES_256);
        B5_HmacSha256_Update(&(ctx->hmac), iv, B5_AES_IV_SIZE);
        B5_HmacSha256_Update(&(ctx->hmac), data, nblocks*B5_AES_BLK_SIZE);
        B5_HmacSha256_Finit(&(ctx->hmac), ctx->auth);
        if (memcmp(auth, ctx->auth, 16)) {
            return false;
        }
    }

    if (flags & SE3_CMDFLAG_ENCRYPT) {
        B5_Aes256_SetIV(&(ctx->aesdec), iv);
        B5_Aes256_Update(&(ctx->aesdec), data, data, nblocks);
    }

    return true;
}


