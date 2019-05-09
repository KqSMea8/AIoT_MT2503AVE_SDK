#ifndef _AES_H_
#define _AES_H_
#include <stdint.h>
    


// #define the macros below to 1/0 to enable/disable the mode of operation.
//
// ECB enables the basic ECB 16-byte block algorithm. Both can be enabled simultaneously.

void iotlock_AES128_ECB_encrypt(uint8_t* input, const uint8_t* key, uint8_t *output);
void iotlock_AES128_ECB_decrypt(uint8_t* input, const uint8_t* key, uint8_t *output);

void AES_Test(void);

#define MODE_NOPADDING		0x00
#define MODE_PKCS7_PADDING	0x01

//return : bytes encrypted in out buff
//
//	puOutBufLen : 输入时，指明输出缓存最大长度；
//				  输出时，为实际返回数据的长度
uint32_t AES128_EncryptECB
(
uint8_t bMode,
uint8_t* pbInput,
uint32_t uInputLen,
uint8_t *pbOut,
uint32_t *puOutBufLen,	//in: out buff len,out: bytes return
const uint8_t* pbKey
);
//return : bytes of plain text with padding-removed 
//	puOutBufLen : 输入时，指明输出缓存最大长度；
//				  输出时，为实际返回数据的长度
uint32_t AES128_DecryptECB
(
uint8_t bMode,
uint8_t* pbInput,
uint32_t uInputLen,
uint8_t *pbOut,
uint32_t *puOutBufLen,	//in: out buff len,out: bytes return
const uint8_t* pbKey
);
#endif //_AES_H_
