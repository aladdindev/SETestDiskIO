/*! \file  SEfile.h
 *  \brief This file includes constants, return values and public
 *         functions used for implementing a secure file system.
 *   \authors Francesco Giavatto, Nicolò Maunero, Giulio Scalia
 *   \date 17/09/2016
 *
 * In this library you will find wrappers to common OS system calls,
 * in order to manage encrypted files using the SECube Board.
 * In this files are also reported the constant used as parameter
 * and all the possible return values.
 *
 */

#ifndef __SEFILE_SYSCALL__
#define __SEFILE_SYSCALL__

#ifdef __cplusplus
extern "C" {
#endif

#include "se3/L0.h"
#include "se3/L1.h"
#include "se3/se3c1def.h"
#include <string.h>
#include <ctype.h>

typedef struct SEFILE_HANDLE * SEFILE_FHANDLE;  /**< Data struct used to access encrypted files @hideinitializer */

#ifdef __linux__
///    @cond linuxDef
#include <sys/types.h>	/* open, seek */
#include <sys/stat.h>	/* open, create */
#include <fcntl.h>		/* open, create */
#include <unistd.h>		/* write, read, seek, close */
#include <libgen.h>
#include <dirent.h>
///     @endcond
/**
 * \defgroup Mode_Defines mode parameter for secure_open
 * @{
 */
/** \name Use this values as mode parameter for
 * secure_open().
 */
///@{
#define SEFILE_READ		O_RDONLY    /**< Open as Read only @hideinitializer */
#define SEFILE_WRITE	O_RDWR      /**< Open for Read/Write @hideinitializer */
///@}
/** @}*/
/** \defgroup Access_Defines access parameter for secure_open
 * @{
 */
/** \name Use this values as access parameter for
 * secure_open().
 */
///@{
#define SEFILE_NEWFILE	O_CREAT | O_TRUNC /**< Create new file and delete if existing @hideinitializer */
#define SEFILE_OPEN		0                 /**< Open an existing file, create it if not existing @hideinitializer */
///@}
/** @}*/
/** \defgroup Seek_Defines whence parameter for secure_seek
 * @{
 */
/** \name Use this values as whence parameter for
 * secure_seek().
 */
///@{
#define SEFILE_BEGIN 	SEEK_SET    /**< Seek from file beginning @hideinitializer */
#define SEFILE_CURRENT 	SEEK_CUR    /**< Seek from current position @hideinitializer */
#define SEFILE_END 		SEEK_END    /**< Seek from file ending @hideinitializer */
///@}
/** @}*/
#elif _WIN32
#undef UNICODE
#undef _UNICODE
#ifdef __MINGW32__
#include <windows.h>
#include <fileapi.h>
#else
#include <Windows.h>
#include <FileApi.h>
#endif

/**
 * \defgroup mode mode parameter for secure_open
 * @{
 */
/** \name Use this values as mode parameter for
 * secure_open().
 */
///@{
#define SEFILE_READ		GENERIC_READ
#define SEFILE_WRITE	GENERIC_READ | GENERIC_WRITE
///@}
/** @}*/

/** \defgroup access access parameter for secure_open
 * @{
 */
/** \name Use this values as access parameter for
 * secure_open().
 */
///@{
#define SEFILE_NEWFILE	CREATE_ALWAYS
#define SEFILE_OPEN		OPEN_EXISTING
///@}
/** @}*/

/** \defgroup whence whence parameter for secure_open
 * @{
 */
/** \name Use this values as whence parameter for
 * secure_seek().
 */
///@{
#define SEFILE_BEGIN 	FILE_BEGIN
#define SEFILE_CURRENT 	FILE_CURRENT
#define SEFILE_END 		FILE_END
///@}
/** @}*/

#elif __APPLE__

#include <fcntl.h>		/* open, create */
#include <sys/stat.h>	/* open */
#include <unistd.h>		/* write, read, seek, close */
#include <sys/types.h>	/* open, read */
#include <sys/uio.h>	/* read */
#include <dirent.h>
//typedef SEFILE_INT32 SEFILE_FHANDLE;

/**
 * \defgroup mode mode parameter for secure_open
 * @{
 */
/**
 * \name Use this values as mode parameter for
 * secure_open().
 */
///@{
#define SEFILE_READ		O_RDONLY
#define SEFILE_WRITE	O_RDWR
///@}
/** @}*/

/** \defgroup access access parameter for secure_open
 * @{
 */
/**
 * \name Use this values as mode parameter for
 * secure_open().
 */
///@{
#define SEFILE_NEWFILE	O_CREAT | O_TRUNC
#define SEFILE_OPEN		0
///@}
/** @}*/

/** \defgroup whence whence parameter for secure_open
 * @{
 */
/**
 * \name Use this values as mode parameter for
 * secure_open().
 */
///@{
#define SEFILE_BEGIN 	SEEK_SET
#define SEFILE_CURRENT 	SEEK_CUR
#define SEFILE_END 		SEEK_END
///@}
/** @}*/
#endif

#define MAX_PATHNAME 256 /**< Maximum length for pathname string */


/** \defgroup errorValues error values
 * @{
 */
/** \name  Returned error values
 * If something goes wrong, one of this values
 * will be returned.
 */
///@{
#define SEFILE_ENV_ALREADY_SET 		15
#define SEFILE_ENV_WRONG_PARAMETER 	16
#define SEFILE_ENV_MALLOC_ERROR		17
#define SEFILE_ENV_NOT_SET			18
#define SEFILE_SECTOR_MALLOC_ERR	19
#define SEFILE_GET_FILEPOINTER_ERR	20
#define SEFILE_HANDLE_MALLOC_ERR	21
#define SEFILE_CLOSE_HANDLE_ERR		22
#define SEFILE_CREATE_ERROR			23
#define SEFILE_OPEN_ERROR 			24
#define SEFILE_WRITE_ERROR			25
#define SEFILE_SEEK_ERROR			26
#define SEFILE_READ_ERROR			27
#define SEFILE_ILLEGAL_SEEK			28
#define SEFILE_FILESIZE_ERROR		29
#define SEFILE_BUFFER_MALLOC_ERR	30
#define SEFILE_FILENAME_DEC_ERROR	31
#define SEFILE_FILENAME_ENC_ERROR	32
#define SEFILE_DIRNAME_ENC_ERROR	33
#define SEFILE_DIRNAME_DEC_ERROR	34
#define SEFILE_DIRNAME_TOO_LONG		35
#define SEFILE_MKDIR_ERROR			36
#define SEFILE_LS_ERROR				37
#define SEFILE_USER_NOT_ALLOWED		38
#define SEFILE_ENV_INIT_ERROR		39
#define SEFILE_ENV_UPDATE_ERROR		40
#define SEFILE_INTEGRITY_ERROR		41
#define SEFILE_NAME_NOT_VALID		42
#define SEFILE_TRUNCATE_ERROR		43
#define SEFILE_DEVICE_SN_MISMATCH   44
#define SEFILE_KEYID_NOT_PRESENT    45
#define SEFILE_ALGID_NOT_PRESENT    46
#define SEFILE_PATH_TOO_LONG        47
#define SEFILE_SYNC_ERR             48
#define SEFILE_SIGNATURE_MISMATCH   49

///@}
/** @}*/

/**
 * @defgroup Sector_Defines
 *  @{
 */
/**
 * \name Constant used to define sector structure.
 * Do not change this unless you know what you are doing.
 */
///@{
#ifndef SEFILE_SECTOR_SIZE
#define SEFILE_SECTOR_SIZE 			512				    /**< Actual sector size. Use only power of 2*/
#endif
#define SEFILE_SECTOR_DATA_SIZE		(SEFILE_SECTOR_SIZE-B5_SHA256_DIGEST_SIZE)  /**< The actual valid data may be as much as this, since the signature is coded on 32 bytes*/
//#define SEFILE_SECTOR_DATA_SIZE		(SEFILE_SECTOR_SIZE-4)  /**< The actual valid data may be as much as this, since the signature is coded on 4 bytes*/
#define SEFILE_BLOCK_SIZE			B5_AES_BLK_SIZE				/**< Cipher block algorithm requires to encrypt data whose size is a multiple of this block size*/
/** The largest multiple of \ref SEFILE_BLOCK_SIZE that can fit in \ref SEFILE_SECTOR_DATA_SIZE */
#define SEFILE_LOGIC_DATA			(SEFILE_SECTOR_DATA_SIZE-2)
/** The amount of Overhead created by \ref SEFILE_SECTOR::len and \ref SEFILE_SECTOR::signature */
#define SEFILE_SECTOR_OVERHEAD 		(SEFILE_SECTOR_SIZE-SEFILE_LOGIC_DATA)									
///@}
/** @}*/

 /**
 *  \brief This function creates a new secure environment,
 *        by allocating statically the parameters needed
 *        by the following functions.
 *  
 *  \param [in] s Contains the pointer to the se3_session
 *        structure that must be used during the session.
 *  \param [in] keyID Contains the ID number of the key that
 *        must be used during the session.
 *  \param [in] crypto Contains the id to specify which algorithm to use.
 *				See \ref AlgorithmAvail, it can be \ref SE3_ALGO_MAX + 1 if 
 *				you don't know which algorithm to choose.
 *         See \ref errorValues for error list.
 *  
 *  \details All the data passed to this function must be allocated
 * and filled with valid data. Once secure_init succeed it is
 * possible to destroy these data, since a copy has been made.
 * N.B. Remember to call the \ref secure_finit function to
 * deallocate these data once you have finished.
 */
uint16_t secure_init(se3_session *s, uint32_t keyID, uint16_t crypto);

 /**
 *  \brief This function can be called only after the secure_init()
 *        function and give to the user the possibility to
 *        overwrite the Environment variables with new ones.
 *  
 *  \param [in] s Contains the pointer to the se3_session
 *        structure that must be used during the session. Can be NULL.
 *  \param [in] keyID keyID Contains the ID number of the key that
 *        must be used during the session. Can be -1.
 *  \param [in] crypto Contains the id to specify which algorithm to use.
 *  \return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 *  
 *  \details Parameters from 1 to 3 can be a NULL pointer or '-1' value,
 * if all parameters are NULL (or '-1' in case of keyID) the function is
 * a No-Operation one.
 */
uint16_t secure_update(se3_session *s, int32_t keyID, uint16_t crypto);
/**
 * @brief This function deallocate the structures defined by the
 *        secure_init(). Should be called at the end of a session.
 * 		  No parameters are needed;
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_finit();
/**
 * @brief This function computes the encrypted name of the file
 *        specified at position path and its length.
 * @param [in] path It can be absolute or relative but it can
 *        not be a directory. No encrypted directory are allowed
 *        inside the path.
 * @param [out] enc_name Already allocate string where the encrypted
 *        filename should be stored.
 * @param [out] encoded_length Pointer to an allocated uint16_t where
 *        the length of the encrypted filename is stored.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t crypto_filename(char *path, char *enc_name, uint16_t *encoded_length);
/**
 * @brief This function opens a secure file and create a SEFILE_FHANDLE that can
 *        be used in future.
 * @param [in] path Specify the absolute/relative path where to retrieve the
 *        file to open. No encrypted directory are allowed inside the path.
 * @param [out] hFile The pointer in which the file handle to the opened file is placed
 *        after a success, NULL in case of failure.
 * @param [in] mode The mode in which the file should be opened. See \ref Mode_Defines.
 * @param [in] access Define if the file should be created or it should already exist.
 *        See \ref Access_Defines.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_open(char *path, SEFILE_FHANDLE *hFile, int32_t mode, int32_t access);
/**
 * @brief This function creates a new secure file and creates a SEFILE_FHANDLE that
 *        can be used in future. If the file already exists, it is overwritten
 *        with an empty one, all previous data are lost.
 * @param [in] path Specify the absolute/relative path where to create the file.
 *        No encrypted directory are allowed inside the path.
 * @param [out] hFile The pointer in which the file handle to the new opened file is
 *        placed after a success, NULL in case of failure.
 * @param [in] mode The mode in which the file should be created. See \ref Mode_Defines.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_create(char *path, SEFILE_FHANDLE *hFile, int mode);
/**
 * @brief This function writes the characters given by dataIn to the encrypted
 *        file hFile. Before writing them, dataIn is encrypted according to the
 *        environmental parameters.
 * @param [in] hFile The handle to an already opened file to be written.
 * @param [in] dataIn The string of characters that have to be written.
 * @param [in] dataIn_len The length, in bytes, of the data that have to be written.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_write(SEFILE_FHANDLE *hFile, uint8_t * dataIn, uint32_t dataIn_len);
/**
 * @brief This function reads from hFile bytesRead characters out of dataOut_len
 *        correctly decrypted ones and stores them in dataOut string.
 * @param [in] hFile The handle to an already opened file to be read.
 * @param [out] dataOut An already allocated array of characters where to store data read.
 * @param [in] dataOut_len Number of characters we want to read.
 * @param [out] bytesRead Number of effective characters read, MUST NOT BE NULL.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_read(SEFILE_FHANDLE *hFile,  uint8_t * dataOut, uint32_t dataOut_len, uint32_t * bytesRead);
/**
 * @brief This function is used to move correctly the file pointer.
 * @param [in] hFile The handle to the file to manipulate.
 * @param [in] offset Amount of character we want to move.
 * @param [out] position Pointer to a int32_t variable where the final position is
 *        stored, MUST NOT BE NULL.
 * @param [in] whence According to this parameter we can choose if we want to move
 *        from the file beginning, file ending or current file pointer position.
 *        See \ref Seek_Defines.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_seek(SEFILE_FHANDLE *hFile, int32_t offset, int32_t *position ,uint8_t whence);
/**
 * @brief This function resizes the file pointed by hFile to size. If size is
 *        bigger than its current size the gap is filled with 0s.
 * @param [in] hFile The handle to the file to manipulate.
 * @param [in] size  New size of the file.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_truncate(SEFILE_FHANDLE *hFile, uint32_t size);
/**
 * @brief This function releases resources related to hFile.
 * @param [in] hFile The handle to the file we do not want to manipulate
 *        no more.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_close(SEFILE_FHANDLE *hFile);
/**
 * @brief This function identifies which encrypted files and encrypted
 *        directories are present in the directory pointed by path
 *        and writes them in list. It only recognizes the ones encrypted
 *        with the current environmental parameters.
 * @param [in] path Absolute or relative path to the directory to browse.
 *        No encrypted directory are allowed inside the path.
 * @param [out] list Already allocated array where to store filenames and directory
 *        names. Each entry is separated by '\0'.
 * @param [out] list_length Pointer to a uint32_t used to stored total number of
 *        characters written in list.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_ls(char *path, char *list, uint32_t * list_length);
/**
 * @brief This function is used to get the total logic size of an encrypted
 *        file pointed by path. Logic size will always be smaller than
 *        physical size.
 * @param [in] path Absolute or relative path the file.
 *        No encrypted directory are allowed inside the path.
 * @param [out] position Pointer to an allocated uint32_t variable where will
 *        be stored the file size.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_getfilesize(char *path, uint32_t * position);
/**
 * @brief This function is used to compute the ciphertext of a directory
 *        name stored in dirname.
 * @param [in] dirpath Path to the directory whose name has to be encrypted.
 *        No encrypted directory are allowed inside the path.
 * @param [out] encDirname A preallocated string where to store the encrypted
 *        directory name
 * @param [out] enc_len A preallocated variable where to store how many bytes
 *        the encrypted directory name is long. Can be NULL.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t crypt_dirname(char *dirpath, char *encDirname, uint32_t* enc_len);
/**
 * @brief This function creates a directory with an encrypted name.
 * @param [in] path Absolute or relative path of the new directory.
 *        No encrypted directory are allowed inside the path.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_mkdir(char *path);
/**
 * @brief This function is used in case we want to be sure that
 *        the physical file is synced with the OS buffers.
 * @param hFile Handle to the secure file to be synced.
 * @return The function returns a (uint16_t) '0' in case of success.
 *         See \ref errorValues for error list.
 */
uint16_t secure_sync(SEFILE_FHANDLE *hFile);
#ifdef __cplusplus
}
#endif

#endif //__SEFILE_SYSCALL__