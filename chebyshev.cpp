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
mp_bitcnt_t digits2bit_mpf(int n_digits) {
	float tmp = 3.2 * n_digits;
	return round(tmp);
}

// Rounding arithmetic of mpf.
mpf_class round_mpf(mpf_class x) {
	if (x - floor(x) > 0.5) {
		return ceil(x);
	}
	else {
		return floor(x);
	}
}

// Return the number of digits of an integer.
int count_digits_mpz(mpz_class x) {
	int count = 0;
	while (x != 0) {
		x /= 10;
		count++;
	}
	return count;
}

// Return the number of digits of an integer.
int count_digits_mpf_integer(mpf_class x) {
	int count = 0;
	while (x > 1) {
		x /= 10;
		count++;
	}
	return count;
}

// Return the number of digits of an floating number.
int count_digits_mpf_decimal(mpf_class x) {
	int count = 0;
	while (x - floor(x) != 0) {
		x *= 10;
		count++;
	}
	return count;
}

// Return the number of digits of an integer in mpreal structure.
int count_digits_mpreal_integer(mpfr::mpreal x) {
	int count = 0;
	while (x != 0) {
		x /= 10;
		count++;
	}
	return count;
}

// Return the number of digits of the decimal part of a real number in mpfr structure.
int count_digits_mpreal_decimal(mpfr::mpreal x) {
	int count = 0;
	while (x - floor(x) != 0) {
		x *= 10;
		count++;
	}
	return count;
}

// Expand the decimal parts into integers.
mpz_class expand_mpf_decimal(mpf_class x, int n_digits) {
	mpf_class tmp;
	// Important to notice the sign, it will be different to get the first integer number!
	if (x > 0) {
		tmp = floor(x);
	}
	else {
		tmp = ceil(x);
	}	
	while (x - floor(x) != 0) {
		tmp *= 10;
		x *= 10;
	}
	x = x - tmp;

	mpz_class x_mpz;
	mp_exp_t power = 0;
	x_mpz.set_str(x.get_str(power, 10, digits2bit_mpf(n_digits)), 10);
	return x_mpz;
}

// Get "floor(x)" + "." + "a", merge the .
// We have to do this becuase sometimes the assignment loses digits, while assignment through str does not.
mpf_class merge_integer_and_decimal(mpf_class x, mpz_class a) {
	mpf_class tmp1;
	// Important to notice the sign, it will be different to get the integer number!
	if (x > 0) {
		tmp1 = floor(x);
	}
	else {
		tmp1 = ceil(x);
	}	
	mp_exp_t power = 0;

	string tmp_str = tmp1.get_str(power, 10, count_digits_mpf_integer(x));
	tmp_str += ".";
	
	// Avoid the situation of a is a negative number.
	a = abs(a);
	tmp_str += a.get_str();

	mpf_set_default_prec(digits2bit_mpf(tmp_str.length()));
	mpf_class tmp2;
	tmp2.set_str(tmp_str, 10);
	return tmp2;
}

// Using the formular a % b = a - (b * int(a/b))
mpfr::mpreal mod_mpreal(mpfr::mpreal a, mpfr::mpreal b) {
	mpfr::mpreal tmp = a/b;
	mpfr::mpreal tmp_integer, tmp_decimal;
	tmp_decimal = modf(tmp, tmp_integer);
	return a - b * tmp_integer;
}



/*------------------------------- Chebyshev Expensive & Economical Algorithm -------------------------------*/



// Do the expensive iteration while counting the number of calculations.
mpf_class chebyshev_expensive_count(mpf_class x, mpz_class s, mpz_class& count) {
	mpf_class t_prev = 1;
	mpf_class t_curr = x;
	mpf_class t_next;
	for (mpz_class i = 0; i < s; i++) {
		t_next = 2*x*t_curr - t_prev;
		t_prev = t_curr;
		t_curr = t_next;
		count += 1;
	}
	return t_prev;
}

// Do the expensive iteration while counting the number of calculations.
mpf_class chebyshev_expensive(mpf_class x, mpz_class s) {
	mpf_class t_prev = 1;
	mpf_class t_curr = x;
	mpf_class t_next;
	for (mpz_class i = 0; i < s; i++) {
		t_next = 2*x*t_curr - t_prev;
		t_prev = t_curr;
		t_curr = t_next;
	}
	return t_prev;
}

/*------------------------------- Binary Representation of n -------------------------------*/
/*--------------------------- 0 implies no include, 1 imples yes ---------------------------*/
// 2^(4k) can cover every interges of k digits, which means we can allocate 3k+1 memory:
// such that [2^4k, 2^(4k-1), ..., 2^2, 2^1, 2^0] and Sum(2^i) = 2^(4k+1) - 1 > arbitrary integer with k digits.
int* binary_representation(mpz_class n, int len_n) {
	int* binary_rep = new int[4*len_n];

	// tmp = 2^(4k)
	mpz_class tmp = 1;
	for (int i = 0; i < 4*len_n - 1; i++) {
		tmp *= 2;
	}
	// from high to low order of 2
	for (int i = 0; i < 4*len_n; i++) {
		if (n >= tmp) {
			binary_rep[i] = 1;
			n -= tmp;
		}
		else {
			binary_rep[i] = 0;
		}
		tmp /= 2;
	}
	return binary_rep;
}


/*--------------------------------- Matrix Manipulations ---------------------------------*/


//The multiplication defined here is especially for the 2x2 matrices
//we are using for Chebyshev polynomial. I used 1x4 vector to reduce the memory occupation.
mpf_class* matrix_multiplication(mpf_class* u, mpf_class* v) {
	mpf_class* tmp = new mpf_class[4];
	tmp[0] = u[0]*v[0] + u[1]*v[2];
	tmp[1] = u[0]*v[1] + u[1]*v[3];
	tmp[2] = u[2]*v[0] + u[3]*v[2];
	tmp[3] = u[2]*v[1] + u[3]*v[3];
	for (int i = 0; i < 4; i++) {
		u[i] = tmp[i];
	}
	delete[] tmp;
	return u;
}

mpf_class* matrix_power(mpf_class* u, mpz_class n) {
	//We need to store u in another address first, because it changes during iterations.
	mpf_class* tmp = new mpf_class[4];
	for (int i = 0; i < 4; i++) {
		tmp[i] = u[i];
	}
	//u*u for n-1 times to get u^n.
	for (mpz_class i = 0; i < n - 1; i++) {
		u = matrix_multiplication(u, tmp);
	}
	delete[] tmp;
	return u;
}


/*---------------------------------- Economical Method -----------------------------------*/


// Do the economical iteration while counting the number of calculations.
//x is the variable you want for Tn(x), n is the index number of iterations you want from Chebyshev Polynomial.
mpf_class chebyshev_economical_count(mpf_class x, mpz_class n, mpz_class& count) {
	// This algorithm does one more in default, so we need to substract one of the number of iterations.
	n = n - 1;
	int len_n = count_digits_mpz(n);
	int* binary_rep = binary_representation(n, len_n);

	//After t*Mx, Tn-2(x) will be Tn-1(x), Tn-1(x) will be Tn(x),
	//and Tn(x) will be 2xTn - Tn-1, which is Tn+1(x).
	mpf_class* t = new mpf_class[4];
	t[0] = 1;	//Tn-2(x), here is T0(x)
	t[1] = x;	//Tn-1(x), here is T1(x)
	t[2] = x;	//Tn-1(x), here is T1(x)
	t[3] = 2*x - 1;	//Tn(x), here is T2(x)

	// Compute Mx^(2^k1 + 2^k2 + 2^k3 + ... + 2^kn), then t*Mx since multiplication is
	// associative, then t[0] will be changed from T0(x) to Tn(x).
	mpf_class* Mx = new mpf_class[4];
	Mx[0] = 0;
	Mx[1] = -1;
	Mx[2] = 1;
	Mx[3] = 2*x;

	// Need a tmp to store our original Mx every time.
	mpf_class* tmp = new mpf_class[4];
	for (int i = 0; i < 4*len_n; i++) {
		if (binary_rep[i] == 1) {
			tmp[0] = 0;
			tmp[1] = -1;
			tmp[2] = 1;
			tmp[3] = 2*x;
			// 2^k, matrix power from opposite direction.
			for (int j = 0; j < 4*len_n - 1 - i; j++) {
				tmp = matrix_multiplication(tmp, tmp);
				// everytime matrix_multiplication will do 3 times of calculation of our term.
				count += 2;
			}
			// If j == 4*len_n, the above does nothing and Mx = Mx*tmp, which is 2^0.
			Mx = matrix_multiplication(Mx, tmp);
			count += 2;
		}
	}
	t = matrix_multiplication(t, Mx);
	count += 2;

	mpf_class result = t[0];
	delete[] binary_rep;
	delete[] tmp;
	delete[] Mx;
	delete[] t;

	return result;
}

// Do the economical iteration without counting the number of calculations.
//x is the variable you want for Tn(x), n is the index number of iterations you want from Chebyshev Polynomial.
mpf_class chebyshev_economical(mpf_class x, mpz_class n) {
	// This algorithm does one more in default, so we need to substract one of the number of iterations.
	n = n - 1;
	int len_n = count_digits_mpz(n);
	int* binary_rep = binary_representation(n, len_n);

	//After t*Mx, Tn-2(x) will be Tn-1(x), Tn-1(x) will be Tn(x),
	//and Tn(x) will be 2xTn - Tn-1, which is Tn+1(x).
	mpf_class* t = new mpf_class[4];
	t[0] = 1;	//Tn-2(x), here is T0(x)
	t[1] = x;	//Tn-1(x), here is T1(x)
	t[2] = x;	//Tn-1(x), here is T1(x)
	t[3] = 2*x - 1;	//Tn(x), here is T2(x)

	// Compute Mx^(2^k1 + 2^k2 + 2^k3 + ... + 2^kn), then t*Mx since multiplication is
	// associative, then t[0] will be changed from T0(x) to Tn(x).
	mpf_class* Mx = new mpf_class[4];
	Mx[0] = 0;
	Mx[1] = -1;
	Mx[2] = 1;
	Mx[3] = 2*x;

	// Need a tmp to store our original Mx every time.
	mpf_class* tmp = new mpf_class[4];
	for (int i = 0; i < 4*len_n; i++) {
		if (binary_rep[i] == 1) {
			tmp[0] = 0;
			tmp[1] = -1;
			tmp[2] = 1;
			tmp[3] = 2*x;
			// 2^k, matrix power from opposite direction.
			for (int j = 0; j < 4*len_n - 1 - i; j++) {
				tmp = matrix_multiplication(tmp, tmp);
			}
			// If j == 4*len_n, the above does nothing and Mx = Mx*tmp, which is 2^0.
			Mx = matrix_multiplication(Mx, tmp);
		}
	}
	t = matrix_multiplication(t, Mx);

	mpf_class result = t[0];
	delete[] binary_rep;
	delete[] tmp;
	delete[] Mx;
	delete[] t;

	return result;
}



/*------------------------------- For Encryption Scheme - Block & Evp -------------------------------*/



/*------------------------------- ASCII and Binary Manipulation -------------------------------*/

// convert a group of 8 digits to a character
char bin_to_ascii(string bin) {
	bitset<8> temp(bin);
	return temp.to_ulong();
}

// convert a character to a group of eight digits
string ascii_to_bin(char ch) {
	bitset<8> temp(ch);
	return temp.to_string();
}


string* message_to_binary_list(string message, int binary_list_n, int char_n_per_group) {
	string* binary_list = new string[binary_list_n];
	//Initialization of each binary group.
	for (int i = 0; i < binary_list_n; i++) {
		binary_list[i] = "0.";
	}

	//8*n digits (n ASCII chars, each char 8 bits) per group.
	for (int i = 0; i < binary_list_n; i++) {
		for (int j = i*char_n_per_group; j < (i+1)*char_n_per_group && j < message.length(); j++) {
			binary_list[i].append(ascii_to_bin(message[j]));
		}
	}
	return binary_list;
}

string binary_list_to_message(string* binary_list, int binary_list_n) {
	string message;
	for (int i = 0; i < binary_list_n; i++) {
		for (int j = 0; j < binary_list[i].length()/8; j++) {
			//bin_to_ascii takes 8 bits as an input.
			message += bin_to_ascii(binary_list[i].substr(j*8, 8));
		}
	}
	return message;
}



/*----------------------------- Extended Euclidean Algorithm and Modular Inverse for Bergamo's Attack ------------------------------*/



mpz_class extended_euclidean(mpz_class a, mpz_class b, mpz_class* x, mpz_class* y) {
	if (a == 0) {
		*x = 0;
		*y = 1;
		return b;
	}
	mpz_class x1, y1;
	mpz_class gcd = extended_euclidean(b%a, a, &x1, &y1);
	*x = y1 - (b/a) * x1;
	*y = x1;
	return gcd;
}

// This function works if and only if gcd(a, b) = 1
mpz_class mod_inverse(mpz_class a, mpz_class b) {
	mpz_class x, y;
	mpz_class g = extended_euclidean(a, b, &x, &y);
	if (g != 1) {
		cout << "Inverse doens't exist, for " << a << " and " << b << endl;
		cout << "Terminated! Please check your code!" << endl;
		exit(1);
	}
	else {
		mpz_class inv = (x % b + b) % b;
		return inv;
	}
}












