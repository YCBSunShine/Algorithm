//
//  aes.c
//  AES
//
//  Created by hc on 2017/12/1.
//  Copyright © 2017年 hc. All rights reserved.
//

#include "aes.h"

static uint8_tx gmult(uint8_tx a, uint8_tx b) {
    
    uint8_tx p = 0, i = 0, hbs = 0;
    
    for (i = 0; i < 8; i++) {
        if (b & 1) {
            p ^= a;
        }
        
        hbs = a & 0x80;
        a <<= 1;
        if (hbs) a ^= 0x1b;
        b >>= 1;
    }
    
    return (uint8_tx)p;
}

static void coef_add(uint8_tx a[], uint8_tx b[], uint8_tx d[]) {
    
    d[0] = a[0]^b[0];
    d[1] = a[1]^b[1];
    d[2] = a[2]^b[2];
    d[3] = a[3]^b[3];
}

static void coef_mult(uint8_tx *a, uint8_tx *b, uint8_tx *d) {
    
    d[0] = gmult(a[0],b[0])^gmult(a[3],b[1])^gmult(a[2],b[2])^gmult(a[1],b[3]);
    d[1] = gmult(a[1],b[0])^gmult(a[0],b[1])^gmult(a[3],b[2])^gmult(a[2],b[3]);
    d[2] = gmult(a[2],b[0])^gmult(a[1],b[1])^gmult(a[0],b[2])^gmult(a[3],b[3]);
    d[3] = gmult(a[3],b[0])^gmult(a[2],b[1])^gmult(a[1],b[2])^gmult(a[0],b[3]);
}

int Nb = 4;

int Nk;

int Nr;

static uint8_tx s_box[256] = {
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

static uint8_tx inv_s_box[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};


static uint8_tx R[] = {0x02, 0x00, 0x00, 0x00};

static uint8_tx * Rcon(uint8_tx i) {
    
    if (i == 1) {
        R[0] = 0x01;
    } else if (i > 1) {
        R[0] = 0x02;
        i--;
        while (i-1 > 0) {
            R[0] = gmult(R[0], 0x02);
            i--;
        }
    }
    
    return R;
}


static void add_round_key(uint8_tx *state, uint8_tx *w, uint8_tx r) {
    
    uint8_tx c;
    
    for (c = 0; c < Nb; c++) {
        
        state[Nb*0+c] = state[Nb*0+c]^w[4*Nb*r+4*c+0];
        state[Nb*1+c] = state[Nb*1+c]^w[4*Nb*r+4*c+1];
        state[Nb*2+c] = state[Nb*2+c]^w[4*Nb*r+4*c+2];
        state[Nb*3+c] = state[Nb*3+c]^w[4*Nb*r+4*c+3];
    }
}

static void mix_columns(uint8_tx *state) {
    
    uint8_tx a[] = {0x02, 0x01, 0x01, 0x03};
    uint8_tx i, j, col[4], res[4];
    
    for (j = 0; j < Nb; j++) {
        
        for (i = 0; i < 4; i++) {
            col[i] = state[Nb*i+j];
        }

        coef_mult(a, col, res);
        for (i = 0; i < 4; i++) {
            state[Nb*i+j] = res[i];
        }
    }
}

static void inv_mix_columns(uint8_tx *state) {
    
    uint8_tx a[] = {0x0e, 0x09, 0x0d, 0x0b};
    uint8_tx i, j, col[4], res[4];
    
    for (j = 0; j < Nb; j++) {
        for (i = 0; i < 4; i++) {
            col[i] = state[Nb*i+j];
        }
        
        coef_mult(a, col, res);
        
        for (i = 0; i < 4; i++) {
            state[Nb*i+j] = res[i];
        }
    }
}

static void shift_rows(uint8_tx *state) {
    
    uint8_tx i, k, s, tmp;
    
    for (i = 1; i < 4; i++) {
        s = 0;
        while (s < i) {
            tmp = state[Nb*i+0];
            
            for (k = 1; k < Nb; k++) {
                state[Nb*i+k-1] = state[Nb*i+k];
            }
            
            state[Nb*i+Nb-1] = tmp;
            s++;
        }
    }
}

static void inv_shift_rows(uint8_tx *state) {
    
    uint8_tx i, k, s, tmp;
    
    for (i = 1; i < 4; i++) {
        s = 0;
        while (s < i) {
            tmp = state[Nb*i+Nb-1];
            
            for (k = Nb-1; k > 0; k--) {
                state[Nb*i+k] = state[Nb*i+k-1];
            }
            
            state[Nb*i+0] = tmp;
            s++;
        }
    }
}

static void sub_bytes(uint8_tx *state) {
    
    uint8_tx i, j;
    uint8_tx row, col;
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            row = (state[Nb*i+j] & 0xf0) >> 4;
            col = state[Nb*i+j] & 0x0f;
            state[Nb*i+j] = s_box[16*row+col];
        }
    }
}

static void inv_sub_bytes(uint8_tx *state) {
    
    uint8_tx i, j;
    uint8_tx row, col;
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            row = (state[Nb*i+j] & 0xf0) >> 4;
            col = state[Nb*i+j] & 0x0f;
            state[Nb*i+j] = inv_s_box[16*row+col];
        }
    }
}

static void sub_word(uint8_tx *w) {
    
    uint8_tx i;
    
    for (i = 0; i < 4; i++) {
        w[i] = s_box[16*((w[i] & 0xf0) >> 4) + (w[i] & 0x0f)];
    }
}

static void rot_word(uint8_tx *w) {
    
    uint8_tx tmp;
    uint8_tx i;
    
    tmp = w[0];
    
    for (i = 0; i < 3; i++) {
        w[i] = w[i+1];
    }
    
    w[3] = tmp;
}

static void key_expansion(uint8_tx *key, uint8_tx *w) {
    
    uint8_tx tmp[4];
    uint8_tx i;
    uint8_tx len = Nb*(Nr+1);
    
    for (i = 0; i < Nk; i++) {
        w[4*i+0] = key[4*i+0];
        w[4*i+1] = key[4*i+1];
        w[4*i+2] = key[4*i+2];
        w[4*i+3] = key[4*i+3];
    }
    
    for (i = Nk; i < len; i++) {
        tmp[0] = w[4*(i-1)+0];
        tmp[1] = w[4*(i-1)+1];
        tmp[2] = w[4*(i-1)+2];
        tmp[3] = w[4*(i-1)+3];
        
        if (i%Nk == 0) {
            rot_word(tmp);
            sub_word(tmp);
            coef_add(tmp, Rcon(i/Nk), tmp);
            
        } else if (Nk > 6 && i%Nk == 4) {
            
            sub_word(tmp);
            
        }
        
        w[4*i+0] = w[4*(i-Nk)+0]^tmp[0];
        w[4*i+1] = w[4*(i-Nk)+1]^tmp[1];
        w[4*i+2] = w[4*(i-Nk)+2]^tmp[2];
        w[4*i+3] = w[4*(i-Nk)+3]^tmp[3];
    }
}

static void cipher(uint8_tx *in, uint8_tx *out, uint8_tx *w) {
    
    uint8_tx state[4*Nb];
    uint8_tx r, i, j;
    
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < Nb; j++)
        {
            state[Nb*i+j] = in[i+4*j];
        }
    }
    
    add_round_key(state, w, 0);
    for (r = 1; r < Nr; r++)
    {
        sub_bytes(state);
        shift_rows(state);
        mix_columns(state);
        add_round_key(state, w, r);
    }
    
    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, w, Nr);
    
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < Nb; j++)
        {
            out[i+4*j] = state[Nb*i+j];
        }
    }
}

static void inv_cipher(uint8_tx *in, uint8_tx *out, uint8_tx *w) {
    
    uint8_tx state[4*Nb];
    uint8_tx r, i, j;
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            state[Nb*i+j] = in[i+4*j];
        }
    }
    
    add_round_key(state, w, Nr);
    
    for (r = Nr-1; r >= 1; r--) {
        inv_shift_rows(state);
        inv_sub_bytes(state);
        add_round_key(state, w, r);
        inv_mix_columns(state);
    }
    
    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, w, 0);
    
    for (i = 0; i < 4; i++) {
        for (j = 0; j < Nb; j++) {
            out[i+4*j] = state[Nb*i+j];
        }
    }
}

void AES_ECB_Encrypt(uint8_tx *buffer, uint32_tx bufSize, uint8_tx *key, uint8_tx **outBuf, uint32_tx *outSize)
{

    uint8_tx *w = NULL;
    uint32_tx i, size = 0;

    if (buffer == NULL || bufSize == 0)
    {
        return;
    }
    
    switch (sizeof(key))
    {
        default:
        case 16: Nk = 4; Nr = 10; break;
        case 24: Nk = 6; Nr = 12; break;
        case 32: Nk = 8; Nr = 14; break;
    }
    
    w = (uint8_tx *)malloc(Nb*(Nr+1)*4);
    key_expansion(key, w);
    size = (bufSize / 16 + (bufSize % 16 ? 1 : 0)) * 16;
    memset(buffer + bufSize, 0xff, size - bufSize);
    
    *outBuf = (uint8_tx *)malloc(size);
    
    for (i = 0; i < size / 16; i++)
    {
        cipher(buffer + i * 16, *outBuf + i * 16, w);
    }
    
    *outSize = size;
}

void AES_ECB_Decrypt(uint8_tx *buffer, uint32_tx bufSize, uint8_tx *key, uint8_tx **outBuf, uint32_tx *outSize)
{
    uint8_tx *tmp, *w = NULL;
    uint32_tx i, count = 0;
    
    
    if (buffer == NULL || bufSize == 0)
    {
        return;
    }
    
    switch (sizeof(key))
    {
        default:
        case 16: Nk = 4; Nr = 10; break;
        case 24: Nk = 6; Nr = 12; break;
        case 32: Nk = 8; Nr = 14; break;
    }
    
    w = (uint8_tx *)malloc(Nb*(Nr+1)*4);
    key_expansion(key, w);
    
    tmp = (uint8_tx *)malloc(bufSize);
    
    for (i = 0; i < bufSize / 16; i++)
    {
        inv_cipher(buffer + i * 16, tmp + i * 16, w);
    }
    
    for (i = bufSize - 1; i > 0; i--)
    {
        if (tmp[i] == 0xff)
        {
            count++;
        }
        else
        {
            break;
        }
    }
    
    *outSize = bufSize - count;
    *outBuf = (uint8_tx *)malloc(*outSize);
    memcpy(*outBuf, tmp, *outSize);
    free(tmp);
}
