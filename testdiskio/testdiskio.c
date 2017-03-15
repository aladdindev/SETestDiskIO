// testdiskio.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <string.h>
#include "ff.h"
#include "diskio.h"
#include <windows.h>
#include "../SEfile/SEfile.h"

#define SECTOR_SIZE 512
#define SECTOR_SIZE_EXP 9

se3_session s;
uint16_t key_id;
TCHAR image_path[MAX_PATHNAME];
DWORD sector_count;

static uint8_t pin[32] = {
	't','e','s','t',  0,0,0,0, 0,0,0,0, 0,0,0,0,
	0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

#pragma warning(disable: 4996)
static
DWORD pn (
    DWORD pns
)
{
    static DWORD lfsr;
    UINT n;


    if (pns) {
        lfsr = pns;
        for (n = 0; n < 32; n++) pn(0);
    }
    if (lfsr & 1) {
        lfsr >>= 1;
        lfsr ^= 0x80200003;
    } else {
        lfsr >>= 1;
    }
    return lfsr;
}

int secube_init() {
	se3_disco_it it;
	uint16_t ret = SE3_OK;
	se3_key keyTable;
	se3_algo algTable[SE3_ALGO_MAX];
	bool found = false;
	int i = 0, count = 0;
	se3_device dev;

	L0_discover_init(&it);
	if (L0_discover_next(&it)) {
		if ((ret = L0_open(&dev, &(it.device_info), SE3_TIMEOUT)) != SE3_OK) {
			return 0;
		}
	}
	else {
		return 0;
	}

	printf("SEcube found!\nInfo:\n");
	printf("Path:\t %ls\n", it.device_info.path);
	printf("Serial Number: %u", it.device_info.serialno);
	printf("\n\n");

	if ((ret = L1_login(&s, &dev, pin, SE3_ACCESS_ADMIN)) != SE3_OK) {
		if (ret == SE3_ERR_PIN) {
			printf("Invalid password\n");
			L0_close(&dev);
			return 0;
		}
	}

	ret = L1_crypto_set_time(&s, (uint32_t)time(0));
	if (SE3_OK != ret) {
		printf("Failure to set time\n");
		L1_logout(&s);
		L0_close(&dev);
		return 0;
	}
	else {
		printf("Time set\n");
	}

	do {
		if (L1_key_list(&s, i, 1, NULL, &keyTable, &count)) {
			break;
		}

		if (count == 0) {
			break;
		}
		if (keyTable.validity>time(0)) {
			key_id = keyTable.id;
			found = true;
			break;
		}
		i++;
	} while (found == false);

	if (found == false) {
		L1_logout(&s);
		L0_close(&dev);
		printf("Device has no keys in keystore\n");
		return 0;
	}

	return 1;
}

int create_disk(TCHAR *path, LONGLONG size) {
	FILE* fp;
	uint8_t end = 0x00;

	_tfopen_s(&fp, path, _T("w"));

	if (!fp) {
		printf("Failed to open image file\n");
		return 0;
	}

	_fseeki64(fp, size - 1, SEEK_SET);

	fwrite(&end, 1, 1, fp);
	fclose(fp);

	return 1;
}

void show_usage() {
	printf("testdiskio.exe\n"
		"  -c Create a new image file.\n"
		"  -i Open existing image file.\n"
		"  -s Size in bytes of the image file. It should be at least the size to hold more than 63 sectors for FAT.\n"
		"  -d Destination directory for the image file.\n"
		"  -f Image file name.\n"
		"Examples:\n"
		"\tmirror.exe -c -s 1073741824 -p C:\\Users\\testdiskio\\ -f testdisk .\n"
		"\tmirror.exe -i C:\\Users\\testdiskio\\testdisk.ahd .\n");
}

int test_diskio (
    BYTE pdrv,      /* Physical drive number to be checked (all data on the drive will be lost) */
    UINT ncyc,      /* Number of test cycles */
    DWORD* buff,    /* Pointer to the working buffer */
    UINT sz_buff    /* Size of the working buffer in unit of byte */
)
{
    UINT n, cc, ns;
    DWORD sz_drv, lba, lba2, sz_eblk, pns = 1;
    WORD sz_sect;
    BYTE *pbuff = (BYTE*)buff;
    DSTATUS ds;
    DRESULT dr;

    printf("test_diskio(%u, %u, 0x%08X, 0x%08X)\n", pdrv, ncyc, (UINT)buff, sz_buff);

    if (sz_buff < _MAX_SS + 4) {
        printf("Insufficient work area to test.\n");
        return 1;
    }

    for (cc = 1; cc <= ncyc; cc++) {
        printf("**** Test cycle %u of %u start ****\n", cc, ncyc);

        /* Initialization */
        printf(" disk_initalize(%u)", pdrv);
        ds = disk_initialize(pdrv);
        if (ds & STA_NOINIT) {
            printf(" - failed.\n");
            return 2;
        } else {
            printf(" - ok.\n");
        }

        /* Get drive size */
        printf("**** Get drive size ****\n");
        printf(" disk_ioctl(%u, GET_SECTOR_COUNT, 0x%08X)", pdrv, (UINT)&sz_drv);
        sz_drv = 0;
        dr = disk_ioctl(pdrv, GET_SECTOR_COUNT, &sz_drv);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 3;
        }
        if (sz_drv < 128) {
            printf("Failed: Insufficient drive size to test.\n");
            return 4;
        }
        printf(" Number of sectors on the drive %u is %lu.\n", pdrv, sz_drv);

#if _MAX_SS != _MIN_SS
        /* Get sector size */
        printf("**** Get sector size ****\n");
        printf(" disk_ioctl(%u, GET_SECTOR_SIZE, 0x%X)", pdrv, (UINT)&sz_sect);
        sz_sect = 0;
        dr = disk_ioctl(pdrv, GET_SECTOR_SIZE, &sz_sect);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 5;
        }
        printf(" Size of sector is %u bytes.\n", sz_sect);
#else
        sz_sect = _MAX_SS;
#endif

        /* Get erase block size */
        printf("**** Get block size ****\n");
        printf(" disk_ioctl(%u, GET_BLOCK_SIZE, 0x%X)", pdrv, (UINT)&sz_eblk);
        sz_eblk = 0;
        dr = disk_ioctl(pdrv, GET_BLOCK_SIZE, &sz_eblk);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
        }
        if (dr == RES_OK || sz_eblk >= 2) {
            printf(" Size of the erase block is %lu sectors.\n", sz_eblk);
        } else {
            printf(" Size of the erase block is unknown.\n");
        }

        /* Single sector write test */
        printf("**** Single sector write test 1 ****\n");
        lba = 0;
        for (n = 0, pn(pns); n < sz_sect; n++) pbuff[n] = (BYTE)pn(0);
        printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
        dr = disk_write(pdrv, pbuff, lba, 1);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 6;
        }
        printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
        dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 7;
        }
        memset(pbuff, 0, sz_sect);
        printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
        dr = disk_read(pdrv, pbuff, lba, 1);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 8;
        }
        for (n = 0, pn(pns); n < sz_sect && pbuff[n] == (BYTE)pn(0); n++) ;
        if (n == sz_sect) {
            printf(" Data matched.\n");
        } else {
            printf("Failed: Read data differs from the data written.\n");
            return 10;
        }
        pns++;

        /* Multiple sector write test */
        printf("**** Multiple sector write test ****\n");
        lba = 1; ns = sz_buff / sz_sect;
        if (ns > 4) ns = 4;
        for (n = 0, pn(pns); n < (UINT)(sz_sect * ns); n++) pbuff[n] = (BYTE)pn(0);
        printf(" disk_write(%u, 0x%X, %lu, %u)", pdrv, (UINT)pbuff, lba, ns);
        dr = disk_write(pdrv, pbuff, lba, ns);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 11;
        }
        printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
        dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 12;
        }
        memset(pbuff, 0, sz_sect * ns);
        printf(" disk_read(%u, 0x%X, %lu, %u)", pdrv, (UINT)pbuff, lba, ns);
        dr = disk_read(pdrv, pbuff, lba, ns);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 13;
        }
        for (n = 0, pn(pns); n < (UINT)(sz_sect * ns) && pbuff[n] == (BYTE)pn(0); n++) ;
        if (n == (UINT)(sz_sect * ns)) {
            printf(" Data matched.\n");
        } else {
            printf("Failed: Read data differs from the data written.\n");
            return 14;
        }
        pns++;

        /* Single sector write test (misaligned memory address) */
        printf("**** Single sector write test (misaligned address) ****\n");
        lba = 5;
        for (n = 0, pn(pns); n < sz_sect; n++) pbuff[n+3] = (BYTE)pn(0);
        printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+3), lba);
        dr = disk_write(pdrv, pbuff+3, lba, 1);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 15;
        }
        printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
        dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 16;
        }
        memset(pbuff+5, 0, sz_sect);
        printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+5), lba);
        dr = disk_read(pdrv, pbuff+5, lba, 1);
        if (dr == RES_OK) {
            printf(" - ok.\n");
        } else {
            printf(" - failed.\n");
            return 17;
        }
        for (n = 0, pn(pns); n < sz_sect && pbuff[n+5] == (BYTE)pn(0); n++) ;
        if (n == sz_sect) {
            printf(" Data matched.\n");
        } else {
            printf("Failed: Read data differs from the data written.\n");
            return 18;
        }
        pns++;

		/* Sanity test */
		printf("**** Sanity test ****\n");
		memset(pbuff, 0, 2048);
		pbuff[1024] = 'a';
		pbuff[1025] = 'b';
		pbuff[2046] = 'b';
		pbuff[2047] = 'a';
		dr = disk_write(pdrv, pbuff, lba, 4);
		if (dr == RES_OK) {
			printf(" - ok.\n");
		}
		else {
			printf(" - failed.\n");
			return 17;
		}
		dr = disk_read(pdrv, pbuff, lba + 2, 2);
		if (dr == RES_OK) {
			printf(" - ok.\n");
		}
		else {
			printf(" - failed.\n");
			return 17;
		}
		if (pbuff[0] == 'a' && pbuff[1] == 'b' && pbuff[1022] == 'b' && pbuff[1023] == 'a') {
			printf(" Data matched.\n");
		}
		else {
			printf("Failed: Read data differs from the data written.\n");
			return 18;
		}
        /* 4GB barrier test */
        printf("**** 4GB barrier test ****\n");
        if (sz_drv >= 128 + 0x80000000 / (sz_sect / 2)) {
            lba = 6; lba2 = lba + 0x80000000 / (sz_sect / 2);
            for (n = 0, pn(pns); n < (UINT)(sz_sect * 2); n++) pbuff[n] = (BYTE)pn(0);
            printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
            dr = disk_write(pdrv, pbuff, lba, 1);
            if (dr == RES_OK) {
                printf(" - ok.\n");
            } else {
                printf(" - failed.\n");
                return 19;
            }
            printf(" disk_write(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+sz_sect), lba2);
            dr = disk_write(pdrv, pbuff+sz_sect, lba2, 1);
            if (dr == RES_OK) {
                printf(" - ok.\n");
            } else {
                printf(" - failed.\n");
                return 20;
            }
            printf(" disk_ioctl(%u, CTRL_SYNC, NULL)", pdrv);
            dr = disk_ioctl(pdrv, CTRL_SYNC, 0);
            if (dr == RES_OK) {
            printf(" - ok.\n");
            } else {
                printf(" - failed.\n");
                return 21;
            }
            memset(pbuff, 0, sz_sect * 2);
            printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)pbuff, lba);
            dr = disk_read(pdrv, pbuff, lba, 1);
            if (dr == RES_OK) {
                printf(" - ok.\n");
            } else {
                printf(" - failed.\n");
                return 22;
            }
            printf(" disk_read(%u, 0x%X, %lu, 1)", pdrv, (UINT)(pbuff+sz_sect), lba2);
            dr = disk_read(pdrv, pbuff+sz_sect, lba2, 1);
            if (dr == RES_OK) {
                printf(" - ok.\n");
            } else {
                printf(" - failed.\n");
                return 23;
            }
            for (n = 0, pn(pns); pbuff[n] == (BYTE)pn(0) && n < (UINT)(sz_sect * 2); n++) ;
            if (n == (UINT)(sz_sect * 2)) {
                printf(" Data matched.\n");
            } else {
                printf("Failed: Read data differs from the data written.\n");
                return 24;
            }
        } else {
            printf(" Test skipped.\n");
        }
        pns++;

        printf("**** Test cycle %u of %u completed ****\n\n", cc, ncyc);
    }

    return 0;
}

int _tmain (int argc, TCHAR* argv[])
{
    int rc;
    DWORD buff[512];  /* 2048 byte working buffer */
	int i;
	uint16_t create = 1;
	LONGLONG size;
	TCHAR file_name[MAX_PATHNAME];

	if (argc < 4) {
		show_usage();
		return 0;
	}

	for (i = 1; i < argc; i++) {
		switch (towlower(argv[i][1])) {
		case _T('c'):
			create = 1;
			break;
		case _T('i'):
			create = 0;
			break;
		case _T('d'):
			i++;
			_tcscpy_s(image_path, sizeof(image_path) / sizeof(TCHAR), argv[i]);
			break;
		case _T('f'):
			i++;
			_tcscpy_s(file_name, sizeof(file_name) / sizeof(TCHAR),argv[i]);
			break;
		case _T('s'):
			i++;
			size = _ttoll(argv[i]);
			size = size - size % SECTOR_SIZE;
			sector_count = (DWORD)(size >> SECTOR_SIZE_EXP);
			break;
		}
	}

	if (image_path[_tcslen(image_path) - 1] != '\\') {
		_tcscat_s(image_path, 1, _T("\\"));
	}

	_tcscat_s(image_path, sizeof(image_path) / sizeof(TCHAR) - _tcslen(image_path), file_name);
	_tcscat_s(image_path, sizeof(image_path) / sizeof(TCHAR) - _tcslen(image_path), _T(".ahd"));
	
	if (create) {
		printf("Creating disk, this may take a moment ...\n");
		create_disk(image_path, size);
	}
	else {
		printf("not yet implemented\n");
		return 0;
	}

	if(!secube_init()) {
		printf("Could not find secube device\n");
		return 0;
	}

    /* Check function/compatibility of the physical drive #0 */
    rc = test_diskio(0, 3, buff, sizeof buff);
    if (rc) {
        printf("Sorry the function/compatibility test failed. (rc=%d)\nFatFs will not work on this disk driver.\n", rc);
    } else {
        printf("Congratulations! The disk driver works well.\n");
    }

	getchar();

    return rc;
}
