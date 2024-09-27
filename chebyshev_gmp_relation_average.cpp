// Author: Xiaoqi LIU, GTIIT
// Date: September 2024

// After we get the largest s that is supported by l and m, we want to do 20 iterations and store the 
// statistically average values of number of operations, error, and time cost. The table in the paper is constructed using this program.

// In order to compile and run this program, you need to install two libraries: GMP and MPFR.
// Compile: g++ -o chebyshev_gmp_relation_average chebyshev_gmp_relation_average.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp
// Run: ./chebyshev_gmp_relation_average <precision l> <precision m> <length of secret key> (l for assignment, m for compuation)
#include <iostream>
#include <string>

#include <gmpxx.h>
#include <mpreal.h>
#include "chebyshev.hpp"

#include <time.h>

using namespace std;

int main (int argc, char* argv[]) {
	int n_digits_l, n_digits_m, n_s;
	if (argc == 4) {
		n_digits_l = atoi(argv[1]);
		n_digits_m = atoi(argv[2]);		
		n_s = atoi(argv[3]);
	}
	else {
		cout << "Notice: If you want to run this program, you need 2 inputs, such that: " << endl;
		cout << "Example: ./chebyshev_gmp_relation_average <precision l> <precision m> <length of secret key> (l for assignment, m for compuation, s = 10^length) " << endl;
		cout << "For example: ./chebyshev_gmp_relation_average 100 120 8" << endl;
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

	// Choose s = 10^n_s
	mpz_class s = 1;
	for (int i = 0; i < n_s; i++) {
		s *= 10;
	}

	/*----------------------------------- Start Experiment/Computation -----------------------------------*/


	mpf_set_default_prec(digits2bit_mpf(n_digits_m));
	cout << "Precision m of mpf for computation is: " << mpf_get_default_prec() << ", where 3.2 bit represents one decimal digit in average. ";
	cout << "Which is " << int(mpf_get_default_prec() / 3.2) << " digits." << endl;
	cout.precision(n_digits_m);
	
	clock_t start, end;
	double time_count = 0;

	mpz_class calculation_count;
	mpz_class calculation_count_count = 0;
	mpz_class r;
	mpz_class r_count = 0;
	mpf_class y, z0, z1, z2, Ts_z0, u_recover, difference;
	mpf_class difference_count = 0;

	gmp_randclass rand(gmp_randinit_default);
	
	int n_iterations = 20;
	mpf_class n_iterations_mpf;
	n_iterations_mpf.set_str("100", 10);

	for (int i = 0; i < n_iterations; i++) {
		cout << "Iteration " << i << endl;
		start = clock();

		// Get a random number between s/2 and s.
		r = rand.get_z_range(s/2) + s/2;
		r_count += r;


		calculation_count = 0;		
		y = chebyshev_economical_count(x, s, calculation_count);
		z0 = chebyshev_economical_count(x, r, calculation_count);
		z1 = chebyshev_economical_count(y, r, calculation_count);
		z2 = u * z1;

		Ts_z0 = chebyshev_economical_count(z0, s, calculation_count);
		calculation_count_count += calculation_count;
		
		u_recover = z2 / Ts_z0;

		difference = abs(u - u_recover);
		difference_count += difference;

		end = clock();
		time_count += double(end - start)/CLOCKS_PER_SEC;
		
		cout << "Calculation count = " << calculation_count << endl;
		if (difference > error) {
			cout << "Difference = " << difference << endl;
			cout << "Which is greater than error = " << error << endl;
		}
	}
	
	cout << endl << s << endl;
	cout << "digits of private key s = " << n_s << endl;

	cout << endl << "r was choosen by a random number between s and s/2, and during the " << n_iterations << " iterations: " << endl;
	cout << "average of r = " << r_count/n_iterations << endl;
	cout << "average of calculation count = " << calculation_count_count/n_iterations << endl;
	cout << "average of time = " << time_count/n_iterations << "s" << endl;

	cout << endl << "average of difference = " << difference_count/n_iterations_mpf << endl;
	cout << "while the error = " << error << endl;
	if (difference_count/n_iterations_mpf < error) {
		cout << endl << "For l = " << n_digits_l << ", m = " << n_digits_m << ", s = 10^" << n_s << ":" << endl;
		cout << "The average difference is smaller than the error, means the decryption succeeded!" << endl;
	}
	else {
		cout << endl << "For l = " << n_digits_l << ", m = " << n_digits_m << ", s = 10^" << n_s << ":" << endl;
		cout << "The average difference is greater than the error, means the decryption failed!" << endl;
	}

	return 0;
}


