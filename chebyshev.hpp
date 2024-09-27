// Author: Xiaoqi LIU, GTIIT
// Date: September 2024

// This Program is the Library of our Chebyshev Article
#include <iostream>
#include <string>
#include <bitset>

#include <gmpxx.h>
#include <mpreal.h>

using namespace std;


/*------------------------------- Defined Functions & Arithmetic to deal with MPF and MPREAL structures -------------------------------*/

// To change the precision of mpf structure.
mp_bitcnt_t digits2bit_mpf(int n_digits);

// Rounding arithmetic of mpf.
mpf_class round_mpf(mpf_class x);

// Return the number of digits of an integer.
int count_digits_mpz(mpz_class x);

// Return the number of digits of an integer.
int count_digits_mpf_integer(mpf_class x);

// Return the number of digits of an floating number.
int count_digits_mpf_decimal(mpf_class x);

// Return the number of digits of an integer in mpreal structure.
int count_digits_mpreal_integer(mpfr::mpreal x);

// Return the number of digits of the decimal part of a real number in mpfr structure.
int count_digits_mpreal_decimal(mpfr::mpreal x);

// Expand the decimal parts into integers.
mpz_class expand_mpf_decimal(mpf_class x, int n_digits);

// Get "floor(x)" + "." + "a", merge the .
// We have to do this becuase sometimes the assignment loses digits, while assignment through str does not.
mpf_class merge_integer_and_decimal(mpf_class x, mpz_class a);

// Using the formular a % b = a - (b * int(a/b))
mpfr::mpreal mod_mpreal(mpfr::mpreal a, mpfr::mpreal b);


/*------------------------------- Chebyshev Expensive & Economical Algorithm -------------------------------*/


// Do the expensive iteration while counting the number of calculations.
mpf_class chebyshev_expensive_count(mpf_class x, mpz_class s, mpz_class& count);

// Do the expensive iteration without counting the number of calculations.
mpf_class chebyshev_expensive(mpf_class x, mpz_class s);

int* binary_representation(mpz_class n, int len_n);

mpf_class* matrix_multiplication(mpf_class* u, mpf_class* v);

mpf_class* matrix_power(mpf_class* u, mpz_class n);

// Do the economical iteration while counting the number of calculations.
mpf_class chebyshev_economical_count(mpf_class x, mpz_class n, mpz_class& count);

// Do the economical iteration without counting the number of calculations.
mpf_class chebyshev_economical(mpf_class x, mpz_class n);


/*------------------------------- For Encryption Scheme - Block & Evp -------------------------------*/
/*------------------------------- ASCII and Binary Manipulation -------------------------------*/


// convert a group of 8 digits to a character
char bin_to_ascii(string bin);

// convert a character to a group of eight digits
string ascii_to_bin(char ch);

string* message_to_binary_list(string message, int binary_list_n, int char_n_per_group);

string binary_list_to_message(string* binary_list, int binary_list_n);


/*----------------------------- Extended Euclidean Algorithm and Modular Inverse for Bergamo's Attack ------------------------------*/


mpz_class extended_euclidean(mpz_class a, mpz_class b, mpz_class* x, mpz_class* y);

// This function works if and only if gcd(a, b) = 1
mpz_class mod_inverse(mpz_class a, mpz_class b);



