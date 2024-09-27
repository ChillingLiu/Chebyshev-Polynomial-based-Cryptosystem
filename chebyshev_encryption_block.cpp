// Author: Xiaoqi LIU, GTIIT
// Date: September 2024

// This program aims test the encryption & decryption of the sysmetric blocking method we metioned in paper.

// In order to compile and run this program, you need to install two libraries: GMP and MPFR.
// Compile: g++ -o chebyshev_encryption_block chebyshev_encryption_block.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp
// Run: ./chebyshev_encryption_block <precision l> <precision m> (l for assignment, m for compuation)
#include <iostream>
#include <string>
#include <fstream>
#include <bitset>

#include <gmpxx.h>
#include <mpreal.h>
#include "chebyshev.hpp"

using namespace std;

int main (int argc, char* argv[]) {
	int n_digits_l, n_digits_m;
	if (argc == 3) {
		n_digits_l = atoi(argv[1]);
		n_digits_m = atoi(argv[2]);		
	}
	else {
		cout << "Notice: If you want to run this program, you need 2 inputs, such that: " << endl;
		cout << "Example: ./chebyshev_encryption_block <precision l> <precision m> (l for assignment, m for compuation) " << endl;
		cout << "For example: ./chebyshev_encryption_block 100 120" << endl;
		return 0;
	}

	/*----------------------------------- Preparation/Assignment -----------------------------------*/
	
	// The cout precision is the precision of the printing out data,
	// to make the output more accurate, be careful to change it before you print out.
	cout.precision(50);
	mpf_set_default_prec(digits2bit_mpf(n_digits_l));
	cout << "Precision of mpf is: " << mpf_get_default_prec() << ", where 3.2 bit represents one decimal digit in average. ";
	cout << "Which is " << int(mpf_get_default_prec() / 3.2) << " digits." << endl;

	/*----------------------------------- Manipulation of Plaintext from ASCII to Floating Numbers with Binary Form -----------------------------------*/

	// Choose u by reading message from encryption_message.txt with precision l number of digits.
	ifstream file ("encryption_message.txt");
	string message = "";
	string line;
	while (getline(file, line)) {
		message += line;
		message += "\n";		
	}
	file.close();
	cout << endl << "Original Message: " << endl;
	cout << message << endl;

	// length of binary_list, 8 binary digits represent 1 ASCII char, so we will have int(l/8) chars in a group.
	// precision l of MPF structure consists of l-2 digits in deciaml part.
	int char_n_per_group = int((n_digits_l) / 8);
	int binary_list_n = int(message.length() / char_n_per_group) + 1;
	string* binary_list = message_to_binary_list(message, binary_list_n, char_n_per_group);

	cout << "Convert Message to Plaintxt with " << char_n_per_group << " chars for ASCII per group in [0,1], where 8 digits represent 1 char:" << endl;
	// Convert string to floating numbers.
	mpf_class* u_list = new mpf_class[binary_list_n];
	for (int i = 0; i < binary_list_n; i++) {
		u_list[i].set_str(binary_list[i], 10);

		// Change precision of cout in order to see more digits
		cout.precision(count_digits_mpf_decimal(u_list[i]));
		cout << u_list[i] << endl;
	}

	/*----------------------------------- Start Experiment -----------------------------------*/

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

	// Choose x as 0.111... with precision m number of digits.
	mpf_class x;
	string tmp_string = "1";
	string x_string = "1";
	while (x_string.length() < n_digits_m) 
		x_string += tmp_string;
	x.set_str((const char*)(("0." + x_string).c_str()), 10);

	cout.precision(count_digits_mpf_decimal(x));
	cout << "We choose x: " << endl << x << endl;
	
	mpf_class y = chebyshev_economical(x, s);
	mpf_class z0 = chebyshev_economical(x, r);
	mpf_class z1 = chebyshev_economical(y, r);

	cout.precision(count_digits_mpf_decimal(y));
	cout << "Public key (x, y) = (x, Ts(x)) = (" << x << ", " << y << ")." << endl;
	cout << "Private key s = " << s << endl << endl;

	// Compute z2:
	cout << endl << "Ciphertext = (z0, z2), where z0 = Tr(x) = " << z0 << ", and z2 = u*Tr(y)" << endl;
	mpf_class* z2_list = new mpf_class[binary_list_n];
	for (int i = 0; i < binary_list_n; i++) {
		z2_list[i] = z1*u_list[i];

		// Change precision of cout in order to see more digits
		cout.precision(count_digits_mpf_decimal(z2_list[i]));
		cout << z2_list[i] << endl;
	}
	cout << "with r = " << r << endl;

	mpf_class Ts_z0 = chebyshev_economical(z0, s);
	
	// Recover plaintext:
	cout << endl << "Recover Plaintxt u = z2/Ts(z0) = " << endl;
	mpf_class* u_list_recover = new mpf_class[binary_list_n];
	for (int i = 0; i < binary_list_n; i++) {
		u_list_recover[i] = z2_list[i] / Ts_z0;

		// Change precision of cout in order to see more digits
		cout.precision(count_digits_mpf_decimal(u_list_recover[i]));
		cout << u_list_recover[i] << endl;
	}

	/*----------------------------------- Convert Floating Numbers to ASCII chars -----------------------------------*/

	int neccessary_digits = char_n_per_group * 8;
	cout << endl << "Took neccessary digits for each block, here is " << neccessary_digits << ", which represents " << char_n_per_group << " ASCII chars in binary: " << endl;
	//Convert floating numbers to string again.
	string* binary_list_recover = new string[binary_list_n];
	mpz_class tmp;
	mp_exp_t power;
	for (int i = 0; i < binary_list_n; i++) {
		//get_str() returns an integer form string with an variable represent power.
		//Example: "3.1415" -> "31415" with power = 1.
		power = 0;
		binary_list_recover[i] = u_list_recover[i].get_str(power, 10, char_n_per_group*8+2);
		//Add missing 0's at the front.
		for (int j = power; j < 0; j++) {
			binary_list_recover[i] = "0" + binary_list_recover[i];
		}
		//Add the missing 0's at the end of floating numbers.
		while (binary_list_recover[i].length()%8 != 0) {
			binary_list_recover[i] += '0';
		}
		cout << binary_list_recover[i] << endl;
	}

	cout << endl << "Recover message using recovered u: " << endl;
	string message_recover = binary_list_to_message(binary_list_recover, binary_list_n);
	cout << message_recover;

	delete[] binary_list;
	delete[] u_list;
	delete[] z2_list;
	delete[] u_list_recover;
	delete[] binary_list_recover;

	return 0;
}
