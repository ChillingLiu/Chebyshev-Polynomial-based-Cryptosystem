// Author: Xiaoqi LIU, GTIIT
// Date: September 2024

// This program aims to test the Bergamo's attack to the Chebyshev Cryptosystem in Numerical Implementation.
// The current codes will show the Bergamo's attacking procedure of the case theta = 5/18*pi.
// You can simply compile and run this program, use argument precision = 20, and 97 to get the exact results we've showed in paper.

// In order to compile and run this program, you need to install two libraries: GMP and MPFR.
// Compile: g++ -o chebyshev_attack_bergamo chebyshev_attack_bergamo.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp
// Run: ./chebyshev_attack_bergamo <precision of computation>
#include <iostream>
#include <string>
#include <stdbool.h>

#include <gmpxx.h>
#include <mpreal.h>
#include "chebyshev.hpp"

using namespace std;


int main (int argc, char* argv[]) {
	int n_digits;
	if (argc == 2) {
		n_digits = atoi(argv[1]);
	}
	else {
		cout << "Notice: If you want to run this program, you need 1 inputs, such that: " << endl;
		cout << "Example: ./chebyshev_attack_bergamo <precision>" << endl;
		cout << "For example: ./chebyshev_bergamo 20" << endl;
		return 0;
	}
	
	// The cout precision is the precision of the printing out data,
	// to make the output more accurate, be careful to change it before you print out.
	cout.precision(5);

	// The precedures of assign precision of MPF and MPFR are not the same!!!
	mpf_set_default_prec(digits2bit_mpf(n_digits));
	int prec = mpfr::digits2bits(n_digits);
	//int prec = digits2bit_mpf(n_digits);
	mpfr::mpreal::set_default_prec(prec);
	cout << "Precision of mpf is: " << mpf_get_default_prec() << ", where 3.2 bit represents one decimal digit in average. ";
	cout << "Which is " << int(mpf_get_default_prec() / 3.2) << " digits." << endl;
	cout << "Precision of mpreal is: " << mpfr::mpreal::get_default_prec() << endl;
	
	/*----------------------------------- Preparation -----------------------------------*/

	// we convert all mpreal numbers into mpf, because of the instability of mpreal.
	mp_rnd_t tmp_rnd = mpfr::mpreal::get_default_rnd();

	mpfr::mpreal pi = mpfr::const_pi(prec);
	mpf_class pi_mpf;
	pi_mpf.set_str(pi.toString(n_digits, 10, tmp_rnd), 10);
	cout.precision(count_digits_mpf_decimal(pi_mpf));
	cout << "pi = " << pi_mpf << endl;

/////////////////////////////
	bool symbolic = true;
	mpfr::mpreal theta, x_mpreal;
	mpf_class x;

	int numerator, denumerator;
	if (symbolic == true) {
		numerator = 5;
		denumerator = 18;
		theta = numerator*pi/denumerator;
		x_mpreal = mpfr::cos(theta);
		cout << "theta = " << theta << endl;

		x.set_str(x_mpreal.toString(n_digits, 10, tmp_rnd), 10);
		cout << "x = " << x << endl;
	}
	else {
		// Choose x as 0.111... with precision l number of digits.
		string tmp_string = "1";
		string x_string = "1";
		while (x_string.length() < n_digits) 
			x_string += tmp_string;
		x.set_str((const char*)(("0." + x_string).c_str()), 10);
	}	
//////////////////////////////	

	mpz_class s;
	s.set_str("106000", 10);

	mpf_class y = chebyshev_economical(x, s);

	cout << endl << "---------------------- Key Generation ---------------------" << endl;
	cout << "Choose x = " << x << ", and s = " << s << endl;
	cout.precision(count_digits_mpf_decimal(y));
	cout << "Then public key is (x, Ts(x)) = (" << x << ", " << y << ")." << endl;
	cout << "And private key is s = " << s << endl;

	mpf_class u;
	u.set_str("0.111111111", 10);

	mpz_class r;
	r.set_str("81500", 10);
	mpf_class z0 = chebyshev_economical(x, r);
	mpf_class z1 = chebyshev_economical(y, r);
	mpf_class z2 = u*z1;
	cout << endl << "------------------------ Encryption -----------------------" << endl;
	cout << "Choose plaintext u = " << u << ", choose r = " << r << endl;
	cout.precision(count_digits_mpf_decimal(z2));
	cout << "Then z0 = Tr(x) = " << z0 << ", z1 = Tr(y) = " << z1 << ", z2 = u*z1 = " << z2  << endl;
	cout << "And the ciphertext is c = (z0, z2) = (" << z0 << ", " << z2 << ")." << endl;

	mpf_class Ts_z0 = chebyshev_economical(z0, s);
	mpf_class u_recover = z2 / Ts_z0;
	cout << endl << "------------------------ Decryption -----------------------" << endl;
	cout << "We get the ciphertext (z0, z2), and we want to recover the plaintext u:" << endl;
	cout.precision(count_digits_mpf_decimal(u_recover));
	cout << "u = z2/Ts(z0) = " << u_recover << endl;


	/*----------------------------------- Start Attacking the System -----------------------------------*/


	cout << endl << endl << "================= Try to Attack the System using Bergamo's Method! =================" << endl;
	cout << endl << "---------------------- Preparation ---------------------" << endl;
	cout << "Eve knows Alice's public key: (x, Ts(x)) = (" << x << ", " << y << ")." << endl;
	cout << "and Bob's ciphertext: (Tr(x), u*Trs(x)) = (" << z0 << ", " << z2 << ")." << endl;
	cout << "Eve wants to recover the original message u using the above two conditions!" << endl;

	cout << endl << "---------------------- Computation ---------------------" << endl;
	cout << "Firstly, Eve needs to compute a r', such that Tr'(x) = Tr(x):" << endl;
	cout << "where r' = (+-arccos(Tr(x)) + 2k*pi)/arccos(x), k in N." << endl;

	// use r_ to represent r', where r_1 = r'(1) = +-arccos(Tr(x))/arccos(x), r_2 = r'(2) = 2pi/arccos(x).
	cout << endl << "---------------------- Procedure of Computation ---------------------" << endl;

/////////////////////////////
	mpfr::mpreal r_1 = mpfr::acos(z0.get_mpf_t()) / mpfr::acos(x.get_mpf_t());
	mpfr::mpreal r_2 = 2 * pi / mpfr::acos(x.get_mpf_t());
/////////////////////////////

	mpf_class r_1_mpf, r_2_mpf;
	r_1_mpf.set_str(r_1.toString(n_digits, 10, tmp_rnd), 10);
	r_2_mpf.set_str(r_2.toString(n_digits, 10, tmp_rnd), 10);

	cout.precision(count_digits_mpf_decimal(r_1_mpf));
	cout << "r'(1): " << r_1_mpf << endl;
	cout.precision(count_digits_mpf_decimal(r_2_mpf));
	cout << "r'(2): " << r_2_mpf << endl;

	cout << endl << "We get the decimal part of r'(1) and r'(2) and expand them to integers." << endl;
	cout << endl << "To do that, we need to know the precision(number of digits) of both of them:" << endl;
	// Get the desired number of digits of the decimal, while also changing the decimal into an integer.
	int n_r_1 = 0;
	int n_r_2 = 0;
	
	mpz_class a = expand_mpf_decimal(r_1_mpf, n_digits);
	mpz_class b = expand_mpf_decimal(r_2_mpf, n_digits);
	n_r_1 = count_digits_mpz(a);
	n_r_2 = count_digits_mpz(b);
			
	cout << "n_r'(1) = " << n_r_1 << endl;
	cout << "n_r'(2) = " << n_r_2 << endl;

	cout << "a = " << a << endl;
	cout << "b = " << b << endl;
	
	// Change r'(1) and r'(2) from a, b, in order to make they have same digits in the last addition part!!!
	r_1_mpf = merge_integer_and_decimal(r_1_mpf, a);
	r_2_mpf = merge_integer_and_decimal(r_2_mpf, b);
	cout << endl << "Assign r_1, r_2 again using the number of a, b in order to make sure they have exactly same digits in their last parts. ";
	cout << "Because our solution only works for a = b (mod c), we can not get an proper integer if there are even small difference between r_1 and a for the last one digit!!!" << endl;
	cout.precision(count_digits_mpf_decimal(r_1_mpf) + 5);
	cout << "r'(1): " << r_1_mpf << endl;
	cout.precision(count_digits_mpf_decimal(r_2_mpf) + 5);
	cout << "r'(2): " << r_2_mpf << endl;

	int n = max(n_r_1, n_r_2);
	if (n_r_1 < n) { 
		while (n_r_1 < n) {
			a *= 10;
			n_r_1++;
		}
	}
	else {
		while (n_r_2 < n) {
			b *= 10;
			n_r_2++;
		}
	}
	
	// Prepare the modular value with power of n.
	mpz_class c = 1;
	int n_c = 0;
	while (n_c < n) {
		c *= 10;
		n_c++;
	}
	
	// Change direction of a, from a + b*k = c, to b*k = (-a mod c) mod c.
	if (a > 0) {
		a = c - a;
	}
	else {
		a = -a;
	}

	cout << endl << "Now, we only consider decimal part: " << endl;
	cout << "(-a mod c) = " << a << endl;
	cout << "b = " << b << endl;
	cout << "c = " << c << endl;
	cout << "We want: a + b*k = c*z, namely, b*k = (-a mod c) mod c!" << endl;

	mpz_class k_1, z_1;
	mpz_class g = extended_euclidean(c, b, &z_1, &k_1);
	cout << endl << "gcd(" << b << ", " << c << ") = " << g << endl;

	// k_1 must be positive, because we want to use k_1*b + z_1*c as the number of iterations of Chebyshev Algo!
	if (k_1 < 0) {
		k_1 += c;
		z_1 -= b;
	}
	cout << "k_1 = " << k_1 << endl;
	cout << "z_1 = " << z_1 << endl;
	cout << "such that, k_1*b + z_1*c = " << k_1*b + z_1*c << endl;
	cout << "Where k_z, z_1 are Bezout's Identity's coefficents given by Extened Euclidean Algorithm." << endl;

	// Expanded the k_1 and z_1 by multiplying a/g, so that k_mpf*b + z_mpf*c = a.
	k_1 *= a;
	z_1 *= a;

	// convert every variables in mpz structure into mpf structure, in order to do the calculations with decimals.
	// we need to increase the precision in order to get all numbers from mpz.
	mpf_set_default_prec(digits2bit_mpf(count_digits_mpz(k_1)));
	mpf_class k_mpf, z_mpf, g_mpf, b_mpf, c_mpf;
	k_mpf.set_str(k_1.get_str(), 10);
	z_mpf.set_str(z_1.get_str(), 10);
	g_mpf.set_str(g.get_str(), 10);
	b_mpf.set_str(b.get_str(), 10);
	c_mpf.set_str(c.get_str(), 10);
	
	int method = 0;
	if (a % g != 0) {
		cout << endl << endl << "gcd(b, c) is not a divisor of a, means there is no solution!!!" << endl;
		return 0;
	}	
	if (g == 1) {
		method = 1;
		
		mpz_class b_inverse = mod_inverse(b, c);		
		cout << endl << "b^-1 = " << b_inverse << endl;

		mpz_class tmp = a*b_inverse;
		cout << "k = a*b^-1= " << tmp << endl;

		tmp = tmp % c;
		cout << "k mod c = " << tmp << endl;

		cout << "Set k into k_mpf. " << endl;
		mpf_set_default_prec(digits2bit_mpf(count_digits_mpz(tmp)));
		k_mpf.set_str(tmp.get_str(), 10);
	}
	else {
		method = 2;
	
		cout.precision(count_digits_mpf_integer(k_mpf) + count_digits_mpf_decimal(k_mpf));
		k_1 /= g;
		z_1 /= g;
		
		// Change precision of cout.
		cout.precision(count_digits_mpf_integer(k_mpf) + count_digits_mpf_decimal(k_mpf));
		cout << endl << "Now, we have: " << endl;
		cout << "k = k_1*a/g = " << k_1 << endl;
		cout << "z = k_1*a/g = " << z_1 << endl;
		cout << "such that, k*b + z*c = " << k_1 * b + z_1 * c << endl;
		
		// We need the k (mod c) in order to work in the integer ring Z[c]
		k_1 = k_1 % c;
		cout << "k mod c = " << k_1 << endl;

		k_mpf.set_str(k_1.get_str(), 10);	
	}

	// Get final solution!!!
	mpf_class solution = r_1_mpf + k_mpf * r_2_mpf;

	// Change precision of cout.
	cout.precision(count_digits_mpf_integer(solution) + count_digits_mpf_decimal(solution));
	cout << endl << "Solution is: r'(1) + k_mpf*r'(2) = " << solution << endl;
	cout << "While the decimal part = " << solution - floor(solution) << endl;

	solution = round_mpf(solution);
	
	mp_exp_t power = 0;
	mpz_class solution_mpz;
	solution_mpz.set_str(solution.get_str(power, 10, digits2bit_mpf(count_digits_mpf_integer(solution))), 10);

	// Add the lost 0's in .set_str()
	while (count_digits_mpz(solution_mpz) < power) {
		solution_mpz *= 10;
	}
	cout << "Rounded solution = " << solution_mpz << endl;
		
	solution_mpz = solution_mpz % c;
	cout << "Solution mod c = " << solution_mpz << endl;
	
	cout << endl << "For precision/number of digits of decimal part of initial start = " << n_digits << endl;
	cout << "The final r' = " << solution_mpz << endl;

	// Add precision for computation to make the result more precise, because the r' we get is relative large.
	mpf_set_default_prec(digits2bit_mpf(n_digits));
	// Change precision of cout.
	cout.precision(count_digits_mpf_decimal(chebyshev_economical(x, solution_mpz)));

	cout << "with Tr'(x) = " << chebyshev_economical(x, solution_mpz) << endl;
	cout << "While Tr(x) = " << chebyshev_economical(x, r) << ", with r = " << r << endl;

	//cout << endl << "And Tr'(Ts(x)) = " << chebyshev_economical(y, solution_mpz) << endl;
	//cout << "While Tr(Ts(x)) = " << chebyshev_economical(y, r) << endl;

	cout << endl << "Moreover, although the results of Tr(x), Tr'(x) showing here in numerical way are not the same. ";
	cout << "Let test them in theoretical or ideal way by cos(r*arccos(x)): " << endl;
	
	// Add precision for computation to make the result more precise, because the r' we get is relative large.
	mpfr::mpreal::set_default_prec(mpfr::digits2bits(n_digits));

	cout << "x = " << x << endl;
	cout.precision(count_digits_mpreal_decimal(mpfr::cos(mpfr::mpreal(r.get_mpz_t())*mpfr::acos(x.get_mpf_t()))));
	cout << "Tr'(x) = cos(r'*arccos(x)) = cos(" << solution_mpz << "*" << mpfr::acos(x.get_mpf_t()) << ") = " << endl; 
	cout << " = " << mpfr::cos(mpfr::mpreal(solution_mpz.get_mpz_t())*mpfr::acos(x.get_mpf_t())) << endl;
	cout << "Tr(x) = cos(r*arccos(x)) = cos(" << r << "*" << mpfr::acos(x.get_mpf_t()) << ") = " << endl; 
	cout << " = " << mpfr::cos(mpfr::mpreal(r.get_mpz_t())*mpfr::acos(x.get_mpf_t())) << endl;

	if (method == 1) {
		cout << endl << "We used method 1, such that k = a*b^-1" << endl;
	}	
	else if (method == 2) {
		cout << endl << "We used method 2, such that k = k_1*a/g" << endl;
	}
	else {
		cout << "ERROR! Declared method! " << endl;
	}
	cout.precision(20);
	cout << "r = " << r << endl;
	cout << "r' = " << solution_mpz << endl;
	if (symbolic == true) {
		cout << "r*" << numerator << " % 2*" << denumerator << " = " << (r*numerator) % (2*denumerator) << endl;
		cout << "r'*" << numerator << " % 2*" << denumerator << " = " << (solution_mpz*numerator) % (2*denumerator) << endl;	
	}
	cout << "Difference1 = " << chebyshev_economical(x, solution_mpz) - chebyshev_economical(x, r) << endl;
	cout << "Difference2 = " << mpfr::cos(mpfr::mpreal(solution_mpz.get_mpz_t())*mpfr::acos(x.get_mpf_t()))-mpfr::cos(mpfr::mpreal(r.get_mpz_t())*mpfr::acos(x.get_mpf_t())) << endl;


	/*cout << 5*solution_mpz << endl;
	solution_mpz = 5*solution_mpz/18;
	cout << solution_mpz << endl;
	cout << solution_mpz*18 << endl;
	cout << solution_mpz*18 + (r*numerator) % (2*denumerator) << endl;
	*/
	/*cout << x << endl;
	cout << r*s*x - 0.87266462*r*s << endl;
	cout << chebyshev_economical(x, r*s) << endl;
	cout << chebyshev_economical(0.64278761, r*s) << endl;
	*/return 0;
}
