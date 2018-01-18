//
//  aes.h
//  AES
//
//  Created by hc on 2017/12/1.
//  Copyright © 2017年 hc. All rights reserved.
//

#ifndef aes_h
#define aes_h

#include <stdio.h>
#include <stdlib.h>

/** 保证各平台一致 */
typedef unsigned char uint8_tx;
typedef unsigned int uint32_tx;

void AES_ECB_Encrypt(uint8_tx *buffer, uint32_tx bufSize, uint8_tx *key, uint8_tx **outBuf, uint32_tx *outSize);
void AES_ECB_Decrypt(uint8_tx *buffer, uint32_tx bufSize, uint8_tx *key, uint8_tx **outBuf, uint32_tx *outSize);

#endif /* aes_h */
