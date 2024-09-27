// Author: Xiaoqi LIU, GTIIT
// Date: September 2024

// This program aims test for a specific number of iterations (s, r), will the economical algorithm change the result from expensive algorithm.
// The result from this program shows that, although the difference is increasing graduately, and for a costly time of iterations, the difference between them staring to influence the result of recovered plaintext.
// But we don't need to worry if our number of iteration is chosen in a time-limited way. And even this happens, we can fix this program by increasing the precision of computation we used here!!! 

// In order to compile and run this program, you need to install two libraries: GMP, MPFR.
// Compile: g++ -o chebyshev_economical_test chebyshev_economical_test.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp
// Run: ./chebyshev_economical_test <precision l> <precision m> (l for assignment, m for compuation)
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
		cout << "Example: ./chebyshev_economical_test <precision l> <precision m> (l for assignment, m for compuation) " << endl;
		cout << "For example: ./chebyshev_economical_test 100 120" << endl;
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
	cout << "Which is " << int(mpf_get_default_prec() / 3.2) << " digits." << endl << endl;
	
	mpz_class s = 10;
	mpz_class r = s / 2;

	clock_t start, end;
	start = clock();

	mpz_class count1 = 0;
	mpz_class count2 = 0;
	mpf_class y = chebyshev_economical(x, s);

	// Suppose chebyshev_economical is original Tr(x), chebyshev_expensive is solution Tr'(x)
	// Theoretically, they are the same, and r' >> r, the only difference is their number of calculations.
	mpf_class z0_1 = chebyshev_economical_count(x, r, count1);
	mpf_class z0_2 = chebyshev_expensive_count(x, r, count2);

	mpf_class z1 = chebyshev_economical(y, r);
	mpf_class z2 = u * z1;

	mpf_class Ts_z0_1 = chebyshev_economical(z0_1, s);
	mpf_class Ts_z0_2 = chebyshev_economical(z0_2, s);

	mpf_class u_recover_1 = z2 / Ts_z0_1;
	mpf_class u_recover_2 = z2 / Ts_z0_2;
	cout.precision(count_digits_mpf_decimal(u_recover_2));

	end = clock();

	mpz_class length = 1;

	int max_iterations = 100;
	for (int i = 0; i < max_iterations; i++) {
		cout << s << endl;
		cout << "digits of private key s = " << length << endl << endl;

		cout << "For economical method: " << endl;
		cout << "number of calculations = " << count1 << endl;
		cout << "u_recover_1  = " << u_recover_1 << endl << endl;

		cout << "For expensive method: " << endl;
		cout << "number of calculations = " << count2 << endl;
		cout << "u_recover_2  = " << u_recover_2 << endl << endl;
		
		cout << "Difference = " << u_recover_1 - u_recover_2 << endl;
		cout << "Difference of number of calculations = " << count2 - count1 << endl;

		cout << "Time = " << double(end - start)/CLOCKS_PER_SEC << "s" << endl << endl;

		if (abs(u_recover_1 - u_recover_2) < error) {
			start = clock();

			s *= 10;
			length += 1;

			y = chebyshev_economical(x, s);
			r = s / 2;

			count1 = 0;
			count2 = 0;
			z0_1 = chebyshev_economical_count(x, r, count1);
			z0_2 = chebyshev_expensive_count(x, r, count2);

			z1 = chebyshev_economical(y, r);
			z2 = u * z1;

			Ts_z0_1 = chebyshev_economical(z0_1, s);
			Ts_z0_2 = chebyshev_economical(z0_2, s);

			u_recover_1 = z2 / Ts_z0_1;
			u_recover_2 = z2 / Ts_z0_2;
			cout.precision(count_digits_mpf_decimal(u_recover_2));

			end = clock();
		}
		else {
			cout << "Difference is greater than error!! Where error = " << error << endl;
			break;
		}
	}
	
	return 0;
}


