/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2016        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */

SYSTEMTIME SysTime;

extern se3_session s;
extern uint16_t key_id;
extern TCHAR image_path[MAX_PATHNAME];
extern DWORD sector_count;

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber to identify the drive */
)
{
	return 0;
}



/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber to identify the drive */
)
{
	GetLocalTime(&SysTime);

	return 0;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
	BYTE pdrv,		/* Physical drive nmuber to identify the drive */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
	HANDLE fileHandle;
	LARGE_INTEGER offset;
	DWORD size = count;

	if (pdrv || !count)
		return RES_PARERR;

	size = size << SECTOR_SIZE_EXP;

	offset.QuadPart = sector;
	offset.QuadPart = offset.QuadPart << SECTOR_SIZE_EXP;

	fileHandle = CreateFile(image_path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE)
		return RES_NOTRDY;

	if (SetFilePointerEx(fileHandle, offset, NULL, FILE_BEGIN) == 0) {
		CloseHandle(fileHandle);
		return RES_ERROR;
	}

	if (ReadFile(fileHandle, (LPCVOID)buff, size, NULL, NULL) == FALSE) {
		CloseHandle(fileHandle);
		return RES_ERROR;
	}

	CloseHandle(fileHandle);

	return disk_decrypt_sectors(buff, sector, count);
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber to identify the drive */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Start sector in LBA */
	UINT count			/* Number of sectors to write */
)
{
	HANDLE fileHandle;
	LARGE_INTEGER offset;
	DWORD size = count;
	DRESULT result;
	
	if (pdrv || !count)
		return RES_PARERR;

	size = size << SECTOR_SIZE_EXP;

	offset.QuadPart = sector;
	offset.QuadPart = offset.QuadPart << SECTOR_SIZE_EXP;

	if ((result = disk_crypt_sectors(buff, sector, count)) != RES_OK) {
		return result;
	}

	fileHandle = CreateFile(image_path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (fileHandle == INVALID_HANDLE_VALUE) {
		return RES_NOTRDY;
	}

	if (SetFilePointerEx(fileHandle, offset, NULL, FILE_BEGIN) == 0) {
		CloseHandle(fileHandle);
		return RES_ERROR;
	}

	if (WriteFile(fileHandle, (LPCVOID)buff, size, NULL, NULL) == FALSE) {
		CloseHandle(fileHandle);
		return RES_ERROR;
	}

	CloseHandle(fileHandle);

	return RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	DRESULT res;

	if (pdrv)
		return RES_PARERR;

	switch (cmd) {
	case GET_SECTOR_COUNT:
		*(DWORD*)buff = 1048576;
		res = RES_OK;
		break;

	case GET_SECTOR_SIZE:
		*(DWORD*)buff = SECTOR_SIZE;
		res = RES_OK;
		break;

	case GET_BLOCK_SIZE:
		*(DWORD*)buff = 1;
		res = RES_OK;
		break;

	case CTRL_SYNC:
		res = RES_OK;
		break;
	default:
		res = RES_PARERR;
		break;
	}

	return res;
}

DWORD get_fattime(void)
{
	return 	  (DWORD)(SysTime.wYear - 1980) << 25
		| (DWORD)SysTime.wMonth << 21
		| (DWORD)SysTime.wDay << 16
		| (DWORD)SysTime.wHour << 11
		| (DWORD)SysTime.wMinute << 5
		| (DWORD)SysTime.wSecond >> 1;
}

DRESULT disk_crypt_sectors(BYTE* buff, DWORD sector, UINT count) {
	uint32_t enc_sess_id = 0;
	uint16_t error = SE3_OK;
	uint8_t nonce_local[16] = { 0x00 };
	uint16_t curr_len = 0;
	void *sp = buff;
	void *rp = buff;
	UINT i;

	for (i = 0; i < count; i++) {
		if ((error = L1_crypto_init(&s, SE3_ALGO_AES, SE3_FEEDBACK_CTR | SE3_DIR_ENCRYPT, key_id, &enc_sess_id)) != SE3_OK) {
			return RES_NOTRDY;
		}

		error = L1_crypto_update(&s, enc_sess_id, SE3_DIR_ENCRYPT | SE3_FEEDBACK_CTR | SE3_CRYPTO_FLAG_FINIT, (uint16_t)512, nonce_local, (uint16_t)512, sp, &curr_len, rp);

		if (error) {
			return RES_NOTRDY;
		}
		sp = (UINT)sp + 512;
		rp = (UINT)rp + 512;
	}

	return RES_OK;
}

DRESULT disk_decrypt_sectors(BYTE* buff, DWORD sector, UINT count) {
	uint32_t enc_sess_id = 0;
	uint16_t error = SE3_OK;
	uint8_t nonce_local[16] = { 0x00 };
	uint16_t curr_len = 0;
	void *sp = buff;
	void *rp = buff;
	UINT i;

	for (i = 0; i < count; i++) {
		if (L1_crypto_init(&s, SE3_ALGO_AES, SE3_FEEDBACK_CTR | SE3_DIR_DECRYPT, key_id, &enc_sess_id) != SE3_OK) {
			return RES_NOTRDY;
		}

		error = L1_crypto_update(&s, enc_sess_id, SE3_DIR_DECRYPT | SE3_FEEDBACK_CTR | SE3_CRYPTO_FLAG_FINIT, (uint16_t)512, nonce_local, (uint16_t)512, sp, &curr_len, rp);

		if (error) {
			return RES_NOTRDY;
		}
		sp = (UINT)sp + 512;
		rp = (UINT)rp + 512;
	}

	return RES_OK;
}

