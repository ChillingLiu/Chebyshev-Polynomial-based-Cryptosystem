// Author: Xiaoqi LIU, GTIIT
// Date: September 2024

// This example consists of 2 parts, firstly it will produce a list with large numbers, here is "s_array[]", you can increase the number by increasing the number of iterations,
// and it will use the s_array to produce a "y_array[]" which is the result of Ts(x), namely, the image of x of chebyshev polynomial in s degree, both in expensive and economical algorithms. 
// Then it will use the last element of s_array as secret key s, and secondly last element as r, and perform the encryption & decryption procedures of Chebyshev based cryptosystem.
// Be careful to notice that the correctness of recovered plaintext u is related to the precision of MPF structure, and here I set it to be 320, which is 100 digits.

// In order to compile and run this program, you only need to install two library: GMP
// Compile: g++ -o example example.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp

#include <iostream>
#include <string>

#include <gmpxx.h>
#include "chebyshev.hpp"

using namespace std;


/*----------------------------------- Main: Experiment here -----------------------------------*/


int main() {

	cout << "Default Precision of GMP Floating Operations: " << mpf_get_default_prec() << endl;
	int prec = 320;
	mpf_set_default_prec(prec);
	std::cout.precision(100);
	cout << "Now, set it to: " << mpf_get_default_prec() << endl << endl;


	/*----------------------- Prepare arbirary large numbers for Testing -----------------------*/


	int iterations = 10;
	mpz_class* s_array = new mpz_class[iterations];
	mpz_class s = 1;
	mpz_class s_prev = 1;
	cout << "Prepare increasing extremely large numbers: " << endl;
	cout << "s = SUM(s_i * 2^i): s0 = 1, s1 = s0*1 = 1, s2 = s0*1 + s1*2 = 3, s3 = s0*1 + s1*2 + s2*4 = 15..." << endl;
	for (int i = 1; i < iterations; i++) {
		s_prev = s;
		int m = 1;
		for (int j = 0; j < i; j++) {
			m *= 2;
		}
		s = s_prev * (m + 1);
		s_array[i] = s;
	}

	mpf_class x;
	x.set_str("0.11111111111111111111111111111111111111111111111111", 10);

	mpf_class* y_array = new mpf_class[iterations];


	/*----------------------------------------- TEST -----------------------------------------*/


	cout << endl << "-------------------- Test: (n,x) -> y: Tn(x) = y --------------------" << endl;
	clock_t start, end;
	start = clock();
	// For larger number, it will need numerous time.
	for (int i = 1; i < 6; i++) {
		cout << "(" << x << ", " << s_array[i] << ") = " << chebyshev_expensive(x, s_array[i]) << endl;
	}
	end = clock();
	cout << "Total time of expensive iteration = " << double(end - start)/CLOCKS_PER_SEC << "s" << endl;
	cout << endl;

	start = clock();
	for (int i = 1; i < iterations; i++) {
		y_array[i] = chebyshev_economical(x, s_array[i]);
		cout << "(" << x << ", " << s_array[i] << ") = " << y_array[i] << endl;
	}
	end = clock();
	cout << "Total time of economical iteration = " << double(end - start)/CLOCKS_PER_SEC << "s" << endl;


	/*----------------------------------- Start Experiment -----------------------------------*/


	s = s_array[iterations-1];
	mpf_class y = chebyshev_economical(x, s);
	cout << endl << endl << "-------------------- Start experiment! --------------------" << endl;
	cout << endl << "---------------------- Key Generation ---------------------" << endl;
	cout << "Choose x = " << x << ", and s = " << s << endl;
	cout << "Then public key is (x, Ts(x)) = (" << x << ", " << y << ")." << endl;
	cout << "And private key is s = " << s << endl;

	mpf_class u;
	u.set_str("0.11111111112222222222333333333344444444445555555555", 10);

	mpz_class r = s_array[iterations-2];
	mpf_class z0 = chebyshev_economical(x, r);
	mpf_class z1 = chebyshev_economical(y, r);
	mpf_class z2 = u*z1;
	cout << endl << "------------------------ Encryption -----------------------" << endl;
	cout << "Choose plaintext u = " << u << ", choose r = " << r << endl;
	cout << "Then z0 = Tr(x) = " << z0 << ", z1 = Tr(y) = " << z1 << ", z2 = u*z1 = " << z2  << endl;
	cout << "And the ciphertext is c = (z0, z2) = (" << z0 << ", " << z2 << ")." << endl;

	mpf_class Ts_z0 = chebyshev_economical(z0, s);
	mpf_class u_recover = z2 / Ts_z0;
	cout << endl << "------------------------ Decryption -----------------------" << endl;
	cout << "We get the ciphertext (z0, z2), and we want to recover the plaintext u:" << endl;
	cout << "u = z2/Ts(z0) = " << u_recover << endl;
	

	delete[] y_array;
	delete[] s_array;
	return 0;
}
