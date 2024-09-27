// Author: Xiaoqi LIU, GTIIT
// Date: September 2024

// This program tests the stability of cos() and arccos() function of MPFR library.

// In order to compile and run this program, you only need to install libraries: GMP, MPFR
// Compile: g++ -o test_mpfr test_mpfr.cpp -lmpfr -lgmpxx -lgmp
#include <iostream>
#include <string>
#include <time.h>

#include <gmpxx.h>
#include <mpreal.h>

using namespace std;

// Return the number of digits of the fraction part of a real number in mpfr structure.
// Pass x as an address (&x), in order to change x from a fraction into an integer.
int count_digits_mpreal(mpfr::mpreal x) {
	int count = 0;
	while (x - round(x) != 0) {
		x *= 10;
		count++;
	}
	return count;
}


int main () {
	int n_digits = 100;
	int prec = mpfr::digits2bits(n_digits);
	mpf_set_default_prec(prec);
	mpfr::mpreal::set_default_prec(prec);

	mpfr::mpreal x("1.298874548742870552260976878418211420383418410426");
	int count = count_digits_mpreal(x);
	cout.precision(count + 1);


	mpfr::mpreal y = mpfr::cos(x);
	mpfr::mpreal z = mpfr::acos(y);	
	cout << "x = " << z << endl << endl;

	int tmp = 10000;
	int iteration_number = tmp;
	while (iteration_number > 0) {
		iteration_number--;
		y = mpfr::cos(z);
		z = mpfr::acos(y);
	}
	cout << "After " << tmp << " iterations, the final " << endl << "x = " << z << endl << endl;

	return 0;
}
