// Author: Xiaoqi LIU, GTIIT
// Date: September 2024

// This program aims test the encryption & decryption of the eveloping method we metioned in paper (we use evp from OpenSSL).

// In order to compile and run this program, you need to install THREE libraries: GMP, MPFR and OpenSSL.
// Compile: g++ -o chebyshev_encryption_evp chebyshev_encryption_evp.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp -lssl -lcrypto
// Run: ./chebyshev_encryption_evp <precision l> <precision m> (l for assignment, m for compuation)
#include <iostream>
#include <string>
#include <fstream>
#include <bitset>

#include <gmpxx.h>
#include "chebyshev.hpp"

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <time.h>

using namespace std;


/*---------------------------------- envelope Seal and Open -----------------------------------*/
// I didn't put these function in the library chebyshev.hpp because only this program uses the envelope functions from OpenSSL.

void handleErrors(void) {
	ERR_print_errors_fp(stderr);
	abort();
}

int envelope_seal(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext) {
	EVP_CIPHER_CTX *ctx;
	int len;
	int ciphertext_len;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new()))
		handleErrors();

	/*
	 * Initialise the encryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits
	 */
	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
		handleErrors();

	/*
	 * Provide the plaintext to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
		handleErrors();
	ciphertext_len = len;

	/*
	 * Finalise the encryption. Further ciphertext bytes may be written at
	 * this stage.
	 */
	if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len))
		handleErrors();
	ciphertext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	return ciphertext_len;
}

int envelope_open(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {
	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new()))
		handleErrors();

	/*
	 * Initialise the decryption operation. IMPORTANT - ensure you use a key
	 * and IV size appropriate for your cipher
	 * In this example we are using 256 bit AES (i.e. a 256 bit key). The
	 * IV size for *most* modes is the same as the block size. For AES this
	 * is 128 bits
	 */
	if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
		handleErrors();

	/*
	 * Provide the plaintext to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary.
	 */
	if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
		handleErrors();
	plaintext_len = len;

	/*
	 * Finalise the decryption. Further plaintext bytes may be written at
	 * this stage.
	 */
	if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len))
		handleErrors();
	plaintext_len += len;

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);
	return plaintext_len;
}


/*----------------------------------- Main: Experiment here -----------------------------------*/


int main (int argc, char* argv[]) {
	int n_digits_l, n_digits_m;
	if (argc == 3) {
		n_digits_l = atoi(argv[1]);
		n_digits_m = atoi(argv[2]);		
	}
	else {
		cout << "Notice: If you want to run this program, you need 2 inputs, such that: " << endl;
		cout << "Example: ./chebyshev_encryption_evp <precision l> <precision m> (l for assignment, m for compuation) " << endl;
		cout << "For example: ./chebyshev_encryption_evp 100 120" << endl;
		return 0;
	}
	
	// The cout precision is the precision of the printing out data,
	// to make the output more accurate, be careful to change it before you print out.
	cout.precision(50);
	mpf_set_default_prec(digits2bit_mpf(n_digits_l));
	cout << "Precision of mpf is: " << mpf_get_default_prec() << ", where 3.2 bit represents one decimal digit in average. ";
	cout << "Which is " << int(mpf_get_default_prec() / 3.2) << " digits." << endl;


	/*----------------------------------- Put plaintext into an envelope -----------------------------------*/


	/* Read message from message.txt */
	ifstream file ("encryption_message.txt");
	string readfile = "";
	string tmp;
	while (getline(file, tmp)) {
		readfile += tmp;
		readfile += "\n";		
	}
	file.close();
	/* plaintext to be encrypted */
	cout << endl << "Original plaintext: " << endl;
	unsigned char* plaintext = (unsigned char *)readfile.c_str();

	cout << plaintext << endl;


	/*----------------------------------- Choose envelope Key -----------------------------------*/


	// Choose envelope_key as 0.0101... with precision l number of digits.
	string tmp_string = "01";
	string envelope_key_string = "0.";
	while (envelope_key_string.length() - 2 < n_digits_l) 
		envelope_key_string += tmp_string;

	/* A 256 bit key */
	// We must change the type of data before we use the key to manipulate the plaintext.
	//unsigned char* envelope_key = envelope_key_string;
	unsigned char* envelope_key;
	strcpy((char*)envelope_key, envelope_key_string.c_str());

	/* A 128 bit IV, Initialisation Vector */
	unsigned char* iv = (unsigned char *)"01234567890123456789";

	/*
	 * Buffer for ciphertext. Ensure the buffer is long enough for the
	 * ciphertext which may be longer than the plaintext, depending on the
	 * algorithm and mode.
	 */
	unsigned char ciphertext[128];

	/* Buffer for the decrypted text */
	unsigned char decryptedtext[128];

	int decryptedtext_len, ciphertext_len;
	
	/* Put plaintext into an envelope, seal it with key */
	ciphertext_len = envelope_seal(plaintext, strlen((char *)plaintext), envelope_key, iv, ciphertext);

	cout << "Put plaintext into an envelope: " << endl;
	BIO_dump_fp(stdout, (const char *)ciphertext, ciphertext_len);
	cout << "with an envelope_key = " << envelope_key << endl;


	/*----------------------------------- Manipulate the envelope key -----------------------------------*/

	/*----------------------------------- Preparation/Assignment -----------------------------------*/

	// Choose s as 10^8 for m = 120, by the experiment we did in "chehyshev_gmp_relation.cpp" 
	mpz_class s = 1;
	for (int i = 0; i < 8; i++) {
		s *= 10;
	}
	mpz_class r = s / 2;

	// Change the precision of MPF from l to m, which is the precision of computation.
	mpf_set_default_prec(digits2bit_mpf(n_digits_m));
	cout << endl << "Precision m of mpf for computation is: " << mpf_get_default_prec() << ", where 3.2 bit represents one decimal digit in average. ";
	cout << "Which is " << int(mpf_get_default_prec() / 3.2) << " digits." << endl;

	// Choose x as 0.1111... with precision l number of digits.
	mpf_class x;
	tmp_string = "1";
	string x_string = "0.1";
	while (x_string.length() - 2 < n_digits_m) 
		x_string += tmp_string;
	x.set_str(x_string.c_str(), 10);

	cout.precision(n_digits_m);
	cout << "We choose x: " << endl << x << endl;

	mpf_class y = chebyshev_economical(x, s);
	mpf_class z0 = chebyshev_economical(x, r);
	mpf_class z1 = chebyshev_economical(y, r);

	cout.precision(count_digits_mpf_decimal(y));
	cout << endl << "Decrytion of the envelope_key using Chebyshev Public-Key Scheme: " << endl;
	cout << "Public key (x, y) = (x, Ts(x)) = (" << x << ", " << y << ")." << endl;
	cout << "Private key s = " << s << endl;
	
	mpf_class envelope_key_float;
	envelope_key_float.set_str(envelope_key_string.c_str(), 10);
	mpf_class z2 = envelope_key_float * z1;
	cout << endl << "Ciphertext = (z0, z2), where z0 = Tr(x) = " << z0 << ", and z2 = envelope_key*Tr(y) = " << z2 << endl;
	cout << "with r = " << r << endl;

	mpf_class Ts_z0 = chebyshev_economical(z0, s);

	envelope_key_float = z2 / Ts_z0;
	cout << endl << "Recovered envelope_key = z2/Ts(z0) = " << envelope_key_float << endl;


	/*--------------------------- Open the envelope key with recovered envelope key ---------------------------*/


	/* Convert float number back to string */
	//get_str() returns an integer form string with an variable represent power.
	//Example: "3.1415" -> "31415" with power = 1.
	mp_exp_t power;
	// strlen to make sure they have exactly the same length.
	string envelope_key_recover = envelope_key_float.get_str(power, 10, envelope_key_string.length());

	//Add missing 0's at the front.
	for (int j = power; j <= 0; j++) {
		if (j == 0) {
			envelope_key_recover = "0." + envelope_key_recover;
		}
		else {
			envelope_key_recover = "0" + envelope_key_recover;
		}
	}
	envelope_key = (unsigned char*)envelope_key_recover.c_str();
	cout << endl << "Take neccessary digits by precision l: " << endl << "Recovered envelope Key = " << envelope_key << endl;

	/* Decrypt the ciphertext */
	decryptedtext_len = envelope_open(ciphertext, ciphertext_len, envelope_key, iv, decryptedtext);

	/* Add a NULL terminator. We are expecting printable text */
	decryptedtext[decryptedtext_len] = '\0';

	/* Show the decrypted text */
	cout << endl << "Open the envelope with recovered envelope_key: " << endl;
	cout << decryptedtext << endl;

	return 0;
}
