#include <pspsdk.h>
#include <pspkernel.h>
#include <pspsysmem_kernel.h>
#include <pspthreadman_kernel.h>
#include <pspcrypt.h>
#include <pspdebug.h>
#include <pspinit.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "psputilsforkernel.h"
#include "systemctrl.h"
#include "utils.h"
#include "printk.h"
#include "main.h"

typedef struct {
	u32 tag;
	u8 *key;
	u32 code;
	u32 type;
} cipher;

static u8 g_key_00000000[0x90] = {
	0x6A, 0x19, 0x71, 0xF3, 0x18, 0xDE, 0xD3, 0xA2, 0x6D, 0x3B, 0xDE, 0xC7, 0xBE, 0x98, 0xE2, 0x4C,
	0xE3, 0xDC, 0xDF, 0x42, 0x7B, 0x5B, 0x12, 0x28, 0x7D, 0xC0, 0x7A, 0x59, 0x86, 0xF0, 0xF5, 0xB5,
	0x58, 0xD8, 0x64, 0x18, 0x84, 0x24, 0x7F, 0xE9, 0x57, 0xAB, 0x4F, 0xC6, 0x92, 0x6D, 0x70, 0x29,
	0xD3, 0x61, 0x87, 0x87, 0xD0, 0xAE, 0x2C, 0xE7, 0x37, 0x77, 0xC7, 0x3C, 0x96, 0x7E, 0x21, 0x1F,
	0x65, 0x95, 0xC0, 0x61, 0x57, 0xAC, 0x64, 0xD8, 0x5A, 0x6D, 0x14, 0xD2, 0x9C, 0x54, 0xC6, 0x68,
	0x5D, 0xF5, 0xC3, 0xF0, 0x50, 0xDA, 0xEA, 0x19, 0x43, 0xA7, 0xAD, 0xC3, 0x2A, 0x14, 0xCA, 0xC8,
	0x4C, 0x83, 0x86, 0x18, 0xAE, 0x86, 0x49, 0xFB, 0x4F, 0x45, 0x75, 0xD2, 0xC3, 0xD6, 0xE1, 0x13,
	0x69, 0x37, 0xC6, 0x90, 0xCF, 0xF9, 0x79, 0xA1, 0x77, 0x3A, 0x3E, 0xBB, 0xBB, 0xD5, 0x3B, 0x84,
	0x1B, 0x9A, 0xB8, 0x79, 0xF0, 0xD3, 0x5F, 0x6F, 0x4C, 0xC0, 0x28, 0x87, 0xBC, 0xAE, 0xDA, 0x00, 
};

static u8 g_key_02000000[0x90] = {
	0x72, 0x81, 0x2F, 0xB3, 0x39, 0x5A, 0x3D, 0xBD, 0x38, 0x8A, 0x10, 0x74, 0x96, 0x55, 0xB1, 0xDF,
	0x88, 0x9F, 0xEE, 0xA1, 0xB5, 0x71, 0x74, 0x89, 0x56, 0xE1, 0xA3, 0xBB, 0x7E, 0x9F, 0xC3, 0xC2,
	0x9E, 0xF8, 0x9B, 0xB9, 0x87, 0xBD, 0x22, 0x88, 0x57, 0xDE, 0x1B, 0x88, 0xC9, 0x9A, 0x3B, 0x1A,
	0xBA, 0xBD, 0xC7, 0xA6, 0x58, 0xCB, 0x8F, 0xA1, 0x0E, 0xDF, 0x64, 0x3B, 0x4A, 0x96, 0x96, 0xCB,
	0x36, 0xD0, 0x4F, 0x2D, 0x32, 0xDD, 0x19, 0xAB, 0xE1, 0xD6, 0x54, 0xFE, 0x97, 0x13, 0x57, 0x5C,
	0x7A, 0x68, 0x05, 0x71, 0x34, 0x7D, 0x31, 0x1E, 0x33, 0x66, 0xDD, 0x6D, 0x7B, 0x76, 0x17, 0x1B,
	0x25, 0x9B, 0xAF, 0x21, 0x79, 0x17, 0x72, 0x10, 0xFD, 0xB5, 0x55, 0x35, 0xA9, 0xBE, 0x55, 0xAE,
	0x72, 0x45, 0xCE, 0x55, 0xA2, 0x70, 0x80, 0xE5, 0xAD, 0xD0, 0xBE, 0xB9, 0xE4, 0x7E, 0x02, 0xA9,
	0x92, 0x46, 0xC3, 0x35, 0x05, 0xF1, 0x7A, 0x93, 0xC1, 0x3A, 0x1A, 0x48, 0x99, 0x3B, 0x3C, 0x1B,
};

static u8 g_key_0B000000[0x90] = {
	0x0B, 0x01, 0x1C, 0xE7, 0x31, 0x15, 0x6B, 0x83, 0x3E, 0x26, 0x0D, 0xCC, 0x69, 0x36, 0x12, 0xCB,
	0xA7, 0xFD, 0x26, 0x66, 0x93, 0x2A, 0x6E, 0x1A, 0x91, 0x2E, 0xC6, 0xFC, 0xD8, 0x2F, 0x00, 0x13,
	0x5A, 0xE2, 0xDF, 0xB6, 0xA2, 0xE4, 0x27, 0xC8, 0x18, 0xC3, 0x50, 0x50, 0xB7, 0xE9, 0x4A, 0xED,
	0xCC, 0x3C, 0x30, 0xFD, 0x10, 0x6A, 0x2B, 0x0A, 0x22, 0xCB, 0xC6, 0xE0, 0x20, 0x65, 0x12, 0xEB,
	0x7D, 0x4E, 0x2A, 0x37, 0x0B, 0x0A, 0xEF, 0x88, 0xDA, 0x06, 0x54, 0xD4, 0x30, 0xAF, 0xCD, 0xCA,
	0x9A, 0xF9, 0xDA, 0x1A, 0xB0, 0x1B, 0xBB, 0x62, 0x0C, 0xDB, 0xF8, 0x44, 0x73, 0x56, 0x14, 0x8E,
	0x93, 0xB1, 0x2C, 0xFD, 0x67, 0xE2, 0x5D, 0xCB, 0x48, 0x5B, 0xD9, 0xB3, 0x54, 0x14, 0xD7, 0x9F,
	0x79, 0x9C, 0x24, 0xE9, 0xC2, 0x7A, 0x4E, 0x8C, 0x4D, 0x24, 0x19, 0x94, 0xFF, 0xC9, 0xC2, 0x2D,
	0x23, 0x63, 0x51, 0xB8, 0xFA, 0xD6, 0x7F, 0xE6, 0x5E, 0xBC, 0x32, 0xB2, 0x02, 0x13, 0xC4, 0x76,
};

static u8 g_key_4C74CBA9[0x90] = {
	0xD1, 0x47, 0x39, 0x3F, 0x75, 0x46, 0xEE, 0x35, 0x55, 0x1D, 0x49, 0x84, 0xBA, 0xE6, 0x9E, 0x80,
   	0x24, 0x95, 0x79, 0xDA, 0xF5, 0xD9, 0x42, 0x43, 0x7B, 0xBA, 0x46, 0xDE, 0xFB, 0xC6, 0x46, 0xB1,
   	0x00, 0xFD, 0x68, 0x8A, 0xE2, 0x96, 0x90, 0x65, 0x5A, 0xA5, 0x24, 0xB2, 0xAE, 0x84, 0x22, 0x85,
   	0x00, 0x2E, 0x44, 0x7B, 0x4D, 0xB6, 0xC9, 0x11, 0x80, 0x92, 0xF6, 0xE6, 0x18, 0xFB, 0x5A, 0x76,
   	0x99, 0x52, 0x99, 0x3E, 0x6D, 0x80, 0x31, 0x2F, 0x91, 0x73, 0x7A, 0xDF, 0xFE, 0x94, 0x6E, 0xD0,
   	0x65, 0xED, 0x6A, 0x6A, 0x6F, 0x00, 0x84, 0x5C, 0xFD, 0x46, 0x65, 0x95, 0x9C, 0xA7, 0x69, 0x63,
   	0xF3, 0xF1, 0x84, 0xF2, 0x97, 0x4A, 0x8D, 0x92, 0x47, 0xCE, 0xBC, 0xAE, 0xA8, 0xDC, 0xEA, 0xDF,
   	0xDB, 0xDF, 0xC7, 0x5F, 0x3A, 0x14, 0x8C, 0xCE, 0xE8, 0x88, 0xDA, 0xE8, 0x1A, 0x3E, 0xEF, 0x8E,
   	0x8C, 0xF1, 0x69, 0x5B, 0x8E, 0xCD, 0x37, 0x60, 0x74, 0xE0, 0xC8, 0x05, 0x38, 0xFE, 0xF7, 0x49, 
};

static u8 g_key_A6E308F0[0x10] = {
	0xF7, 0xD0, 0x7A, 0x51, 0x5B, 0xA4, 0x66, 0xD5, 0xF7, 0x6D, 0xF4, 0x01, 0xC6, 0x44, 0xB5, 0xE6,
};

static u8 g_key_A6E306F0[0x10] = {
	0x46, 0xB7, 0x96, 0xDC, 0x2B, 0x98, 0xAA, 0x84, 0xFC, 0x26, 0xE4, 0x43, 0xD6, 0xCC, 0xC0, 0xBE,
};

static u8 g_key_A6E305F0[0x10] = {
	0x13, 0x2B, 0x33, 0x61, 0x59, 0x81, 0x47, 0xCF, 0xD8, 0x0F, 0x1C, 0xE6, 0x0E, 0xD2, 0x43, 0x78,
};

static u8 g_key_6AB7CC03[0x10] = {
	0x4B, 0x2B, 0xCE, 0x96, 0xC9, 0x4A, 0x65, 0xB0, 0x0C, 0x32, 0x2A, 0xC6, 0xC3, 0x8C, 0x73, 0x2D,
};

static cipher g_cipher[] = {
	{ 0xA6E308F0, g_key_A6E308F0, 0x5F, 2},
	{ 0xA6E306F0, g_key_A6E306F0, 0x5F, 2},
	{ 0xA6E305F0, g_key_A6E305F0, 0x5F, 2},
	{ 0x6AB7CC03, g_key_6AB7CC03, 0x5F, 2},
	{ 0x4C74CBA9, g_key_4C74CBA9, 0x5F, 1},
	{ 0x0B000000, g_key_0B000000, 0x4E, 0},
	{ 0x02000000, g_key_02000000, 0x45, 0},
//	{ 0x10000000, g_key_10000000, 0x43, 0},
	{ 0x00000000, g_key_00000000, 0x42, 0},
};

static cipher *get_cipher_by_tag(u32 tag)
{
	int i;

	for(i=0; i<NELEMS(g_cipher); ++i) {
		if (g_cipher[i].tag == tag)
			return &g_cipher[i];
	}

	return NULL;
}

static int (*sub_000000e0)(u32 *tag, u8 *key, u32 code, u8 *prx, u32 size, u32 *newsize, u32 use_polling, u8 *blacklist, u32 blacklistsize, u32 type, u8 *xor_key1, u8 *xor_key2) = NULL;

static int myMesgLed_sub_000000e0_wrapper(u32 *tag, u8 *key, u32 code, u8 *prx, u32 size, u32 *newsize, u32 use_polling, u8 *blacklist, u32 blacklistsize, u32 type, u8 *xor_key1, u8 *xor_key2)
{
	int ret;
	u32 keytag;

//	hexdump(prx, 0x150);
	keytag = *(u32*)(prx+0xD0);
	ret = (*sub_000000e0)(tag, key, code, prx, size, newsize, use_polling, blacklist, blacklistsize, type, xor_key1, xor_key2);

	if (ret < 0) {
		cipher *c;

		printk("%s: first tag 0x%08X code %x type %d returns %d\n", __func__, *tag, code, type, ret);
		c = get_cipher_by_tag(keytag);

		if (c == NULL) {
			printk("%s: tag 0x%08X not found\n", __func__, keytag);
			c = get_cipher_by_tag(*tag);

			if (c == NULL) {
				goto exit;
			}
		}

		ret = (*sub_000000e0)(&c->tag, c->key, c->code, prx, size, newsize, use_polling, blacklist, blacklistsize, c->type, xor_key1, xor_key2);
		printk("%s: second tag 0x%08X code %x type %d returns %d\n", __func__, c->tag, c->code, c->type, ret);
	}

exit:
	printk("%s: tag 0x%08X code %x type %d returns 0x%08X\n", __func__, *tag, code, type, ret);

	if (ret < 0)
		ret = -301;

	return ret;
}

// sceMesgLed_driver_81F72B1F: the updater decryptor
// sceMesgLed_driver_CE51764C: the 0x02000000 decryptor?

#if 0
		pokew @sceLoaderCore@+0x5AC4 0
		pokew @sceLoaderCore@+0x5AC8 0

		two nop would disable apitype check
#endif

void patch_mesgled_for_updater(u32 mesgled_textaddr)
{
	sub_000000e0 = (void*)(mesgled_textaddr + 0xe0);

	switch(psp_model) {
		case PSP_1000:
			_sw(MAKE_CALL(myMesgLed_sub_000000e0_wrapper), mesgled_textaddr+0x1B68);
			_sw(MAKE_CALL(myMesgLed_sub_000000e0_wrapper), mesgled_textaddr+0x3884);
			break;
		default:
			printk("%s: unknown model %dg\n", __func__, psp_model+1);
			break;
	}
}