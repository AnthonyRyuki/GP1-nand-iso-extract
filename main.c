/*
 * Copyright (C) 2017 FIX94
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */
#include <stdio.h>
#include <stdbool.h>
#include <inttypes.h>
#include <malloc.h>
#include <string.h>
#include "des.h"

#define KEYS_AVAIL 3

/* Triforce games are encrypted a little weird */
static inline void do64BitSwap(void *in, void *out)
{
	*(unsigned long long*)out = __builtin_bswap64(*(unsigned long long*)in);
}

static struct _des_ctx DESctx;
/* Just a helper function for me to not get confused */
static inline void des_ecb_decrypt_swapped(struct _des_ctx *ctx, void *in, void *out)
{
	do64BitSwap(in,out); //out is now the swapped input
	des_ecb_decrypt(ctx,out,out); //out is now swapped decrypted
	do64BitSwap(out,out); //out is now decrypted
}

static inline void interleave(uint8_t *in1, uint8_t *in2, uint32_t inlen, uint8_t *out)
{
	uint32_t i;
	for(i = 0; i < inlen; i++)
	{
		out[(i<<1)|0]=in1[i];
		out[(i<<1)|1]=in2[i];
	}
}

static inline void des_decrypt_block(uint8_t *buf)
{
	uint32_t i;
	for(i = 0; i < 0x400; i+=8)
		des_ecb_decrypt_swapped(&DESctx, buf+i, buf+i);
}

static void combine_dec(char *in1, char *in2, uint32_t inlen, FILE *out)
{
	FILE *f1 = fopen(in1,"rb");
	FILE *f2 = fopen(in2,"rb");
	uint32_t i;
	uint8_t buf1[0x200], buf2[0x200], outbuf[0x400];
	for(i = 0; i < inlen; i+=0x200)
	{
		fread(buf1,1,0x200,f1);
		fread(buf2,1,0x200,f2);
		interleave(buf1,buf2,0x200,outbuf);
		des_decrypt_block(outbuf);
		fwrite(outbuf,1,0x400,out);
		//skip over verification? block
		fseek(f1,0x10,SEEK_CUR);
		fseek(f2,0x10,SEEK_CUR);
		i+=0x10;
	}
	fclose(f1);
	fclose(f2);
}

static bool verifyFiles(unsigned int game)
{
    FILE *f;
	int i;
	char name[32];
	if(game == 1)
    {
        for(i = 1; i <= 6; i++)
        {
            if(i == 3 || i == 4)
                sprintf(name,"ic%i5_k9f1208u0b",i);
            else
                sprintf(name,"ic%i_k9f1208u0b",i);
            f = fopen(name,"rb");
            if(!f)
            {
                printf("%s missing!\n", name);
                return false;
            }
            fseek(f,0,SEEK_END);
            if(ftell(f) != 0x4200000)
            {
                printf("%s has the wrong length!\n", name);
                fclose(f);
                return false;
            }
            fclose(f);
        }
    }
    else if(game == 2)
	{
	    for(i = 1; i <= 8; i++)
        {
            sprintf(name,"ic%i_k9f1208u0b.bin",i);
            f = fopen(name,"rb");
            if(!f)
            {
                printf("%s missing!\n", name);
                return false;
            }
            fseek(f,0,SEEK_END);
            if(ftell(f) != 0x4200000)
            {
                printf("%s has the wrong length!\n", name);
                fclose(f);
                return false;
            }
            fclose(f);
        }
	}
	else if(game == 3)
	{
	    for(i = 1; i <= 4; i++)
        {
            if(i == 3 || i == 4)
                sprintf(name, "ic%is.bin",i);
            else
                sprintf(name,"ic%i.bin",i);
            f = fopen(name,"rb");
            if(!f)
            {
                printf("%s missing!\n", name);
                return false;
            }
            fseek(f,0,SEEK_END);
            if(ftell(f) != 0x8400000)
            {
                printf("%s has the wrong length!\n", name);
                fclose(f);
                return false;
            }
            fclose(f);
        }
	}
	return true;
}

static const unsigned long long trikeys[KEYS_AVAIL] = {
    0xF767A7B0019E6751, //Mario Kart Arcade GP
    0xCFA3131991992F2B, //Mario Kart Arcade GP 2
    0x6DBAD0D96758FE7F, //F-Zero AX Monster Ride
    };

int main()
{
    unsigned int game;
	printf("Triforce NAND ISO Extract v2.0 by FIX94 and Anthony Ryuki\n");
	printf("Games:\n1: Mario Kart Arcade GP\n2: Mario Kart Arcade GP 2\n3: F-Zero AX Monster Ride\nPlease enter game number... ");
	scanf("%d", &game);
	if (game == 0 || game > 3)
    {
        printf("Not a valid choice\n");
        return 0;
    }
	printf("Checking files...\n");
	if(!verifyFiles(game))
		return 0;
	FILE *out = fopen("OUT.BIN","wb+");
	if(!out)
	{
		printf("OUT.BIN not writable!\n");
		return 0;
	}
	if(game == 1)
    {
        des_setkey(&DESctx, (unsigned char*)(trikeys));
        printf("Combining and decrypting");
        combine_dec("ic1_k9f1208u0b","ic2_k9f1208u0b",0x4200000,out);
        printf(".");
        combine_dec("ic35_k9f1208u0b","ic45_k9f1208u0b",0x4200000,out);
        printf(".");
        combine_dec("ic5_k9f1208u0b","ic6_k9f1208u0b",0x293FFF0,out);
        printf(".\n");
        fclose(out);
        printf("Done!\n");
    }
	else if(game == 2)
    {
        des_setkey(&DESctx, (unsigned char*)(trikeys+1));
        printf("Combining and decrypting");
        combine_dec("ic1_k9f1208u0b.bin","ic2_k9f1208u0b.bin",0x4200000,out);
        printf(".");
        combine_dec("ic3_k9f1208u0b.bin","ic4_k9f1208u0b.bin",0x4200000,out);
        printf(".");
        combine_dec("ic5_k9f1208u0b.bin","ic6_k9f1208u0b.bin",0x4200000,out);
        printf(".");
        combine_dec("ic7_k9f1208u0b.bin","ic8_k9f1208u0b.bin",0x317FFF0,out);
        printf(".\n");
        fclose(out);
        printf("Done!\n");
    }
    else if(game == 3)
    {
        des_setkey(&DESctx, (unsigned char*)(trikeys+2));
        printf("Combining and decrypting");
        combine_dec("ic1.bin","ic2.bin",0x8400000,out);
        printf(".");
        combine_dec("ic3s.bin","ic4s.bin",0x5296F10,out);
        printf(".");
        fclose(out);
        printf("Done!\n");
    }
	return 0;
}
