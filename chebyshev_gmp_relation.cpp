// Author: Xiaoqi LIU, GTIIT
// Date: September 2024

// This program aims test the relationship between l, m, and s as we mentioned in the paper.
// Namely, for fixed precision l, and m, we want to know how large s and r (r=s-1) is that can be supported to succeed in decryption. 

// In order to compile and run this program, you need to install two libraries: GMP and MPFR.
// Compile: g++ -o chebyshev_gmp_relation chebyshev_gmp_relation.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp
// Run: ./chebyshev_gmp_relation <precision l> <precision m> (l for assignment, m for compuation)
#include <iostream>
#include <string>

#include <gmpxx.h>
#include <mpreal.h>
#include "chebyshev.hpp"

#include <time.h>

using namespace std;

int main (int argc, char* argv[]) {
	int n_digits_l, n_digits_m;
	if (argc == 3) {
		n_digits_l = atoi(argv[1]);
		n_digits_m = atoi(argv[2]);		
	}
	else {
		cout << "Notice: If you want to run this program, you need 2 inputs, such that: " << endl;
		cout << "Example: ./chebyshev_gmp_relation <precision l> <precision m> (l for assignment, m for compuation) " << endl;
		cout << "For example: ./chebyshev_gmp_relation 100 120" << endl;
		return 0;
	}
	
	/*----------------------------------- Preparation/Assignment -----------------------------------*/

	// The cout precision is the precision of the printing out data,
	// to make the output more accurate, be careful to change it before you print out.
	cout.precision(50);
	mpf_set_default_prec(digits2bit_mpf(n_digits_l));
	cout << "Precision l of mpf for assignment is: " << mpf_get_default_prec() << ", where 3.2 bit represents one decimal digit in average. ";
	cout << "Which is " << int(mpf_get_default_prec() / 3.2) << " digits." << endl;

	// Choose x as 0.1111... with precision l number of digits.
	mpf_class x;
	string tmp_string = "1";
	string x_string = "1";
	while (x_string.length() < n_digits_l) 
		x_string += tmp_string;
	x.set_str((const char*)(("0." + x_string).c_str()), 10);

	cout.precision(count_digits_mpf_decimal(x));
	cout << "We choose x: " << endl << x << endl;

	// Choose u as 0.0101... with precision l number of digits.
	mpf_class u;
	tmp_string = "01";
	string plaintext_string = "01";
	while (plaintext_string.length() < n_digits_l) 
		plaintext_string += tmp_string;
	u.set_str((const char*)(("0." + plaintext_string).c_str()), 10);

	cout.precision(count_digits_mpf_decimal(u));
	cout << "We choose u: " << endl << u << endl;

	// We must use str_set, becuase if we set it to 0.1 and multiply by several times of 0.1,
	// the assignmnet is not pure 0.1, there are many digits which is not 0 since it's under the precision of 100,
	// like 0.1000000000213123123..., in that case, after our multiplication, the total number of digits is 200.
	mpf_class error;
	tmp_string = "0";
	string error_string = "0";
	while (error_string.length() < n_digits_l - 1) 
		error_string += tmp_string;
	error.set_str((const char*)(("0." + error_string + "1").c_str()), 10);
	cout << "The error we used here to determine the correctness of recovered u is: " << error << endl <<endl;


	/*----------------------------------- Start Experiment/Computation -----------------------------------*/


	mpf_set_default_prec(digits2bit_mpf(n_digits_m));
	cout << "Precision m of mpf for computation is: " << mpf_get_default_prec() << ", where 3.2 bit represents one decimal digit in average. ";
	cout << "Which is " << int(mpf_get_default_prec() / 3.2) << " digits." << endl;
	
	mpz_class s = 10;

	clock_t start, end;
	start = clock();

	mpz_class count = 0;
	mpf_class y = chebyshev_economical_count(x, s, count);
	//mpz_class r = s / 2;
	mpz_class r = s - 1;
	mpf_class z0 = chebyshev_economical_count(x, r, count);
	mpf_class z1 = chebyshev_economical_count(y, r, count);
	mpf_class z2 = u * z1;

	mpf_class Ts_z0 = chebyshev_economical_count(z0, s, count);
	mpf_class u_recover = z2 / Ts_z0;
	cout.precision(count_digits_mpf_decimal(u_recover));

	end = clock();

	mpz_class length = 1;

	int max_iterations = 100;
	for (int i = 0; i < max_iterations; i++) {
		cout << s << endl;
		cout << "digits of private key s = " << length << endl;
		cout << "number of calculations = " << count << endl;
		cout << "u_origin  = " << u << endl;
		cout << "u_recover = " << u_recover << endl;
		cout << "Difference = " << u - u_recover << endl;
		cout << "Time = " << double(end - start)/CLOCKS_PER_SEC << "s" << endl << endl;

		if (abs(u - u_recover) < error) {
			start = clock();

			s *= s;
			length *= 2;

			count = 0;
			y = chebyshev_economical_count(x, s, count);
			//r = s / 2;
			r = s - 1;
			z0 = chebyshev_economical_count(x, r, count);
			z1 = chebyshev_economical_count(y, r, count);
			z2 = u * z1;

			Ts_z0 = chebyshev_economical_count(z0, s, count);
			u_recover = z2 / Ts_z0;
			cout.precision(count_digits_mpf_decimal(u_recover));

			end = clock();
		}
		else {
			cout << "Difference is greater than error!! Where error = " << error << endl;
			cout << "In this case, it supports s = 10^" << length/2 << endl;
			break;
		}
	}
	
	return 0;
}


