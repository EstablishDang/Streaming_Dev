/* FILE   : encrypt.h
 * DATE   : Jun 20, 2019
 * DESC   :
 */
#ifndef ENCRYPT_H_
#define ENCRYPT_H_

/*
 * PURPOSE : Init key and iv vector use for encypt
 * INPUT   : [model] - Model name of product
 * OUTPUT  : None
 * RETURN  : 0 if success
 * DESCRIPT: None
 */
int
alawEncryptKeyInit();

/*
 * PURPOSE : Set encrypt key and iv vector use for encypt
 * INPUT   : [model] - Model name of product
 * OUTPUT  : None
 * RETURN  : 0 if success
 * DESCRIPT: None
 */
int
alawSetEncryptKey();

/*
 * PURPOSE : Encrypt data
 * INPUT   : [model] - Model name of product
 * OUTPUT  : None
 * RETURN  : 0 if success
 * DESCRIPT: None
 */
int
alawEncrypt(const unsigned char *in, unsigned char *out, const unsigned long length);

#endif /* ENCRYPT_H_ */
