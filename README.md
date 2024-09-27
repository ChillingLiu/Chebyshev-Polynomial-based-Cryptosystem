# Chebyshev-Polynomial-based-Cryptosystem
Numerical Experiments of Chebyshev Polynomial Based Cryptosystem and Bergamo's Attack using GMP and MPFR

The project consists the source codes of our research paper that is submitting currently.

Our paper discussed the numerical experiments within the Chebyshev polynoimal based crytosystem and Bergamo's attack. We pointed out the relevance of the length of plaintexts and the precision of arithmetical calculations in the cryptosystem, and proved the Bergamo's attack only succeeds in symbolic computations, and the numerical implementation of the cryptosystem remains its robustness.

To cite our work:
Liu, X; Morales-Luna, G. Experimenting with Chebshev chaotic encryption scheme. Cryptography 2024, 1, 0. https://doi.org/



The source codes are written in C++, and we used GNU Multiple Precision library (GMP) to realize the precise arithmetic and MPFR library to realize the trignometric functions for Bergamo's attack. In order to compile the programs, you need to install these 2 libaraies in advance. You may also need to install OpenSSL library to compile the envelope ciphering method file.

The project consists of following files:
Compiler: Makefile
Library file: chebyshev.hpp, chebyshev.cpp
Economical algorithm of Chebyshev polynomial: chebyshev_economical_test.cpp
Relation between GMP library and the Cryptosystem: chebyshev_gmp_relation.cpp, chebyshev_gmp_relation_average.cpp
Encryption Methods: chebyshev_encryption_block.cpp, chebyshev_encryption_evp.cpp
MPFR library stability: test_mpfr.cpp
Bergamo's Attack: chebyshev_attack_bergamo.cpp



If this is the first time you see the Chebyshev Polynomial based Cryptosystem, I recommand you to read and test the programs in the following order:

1. example.cpp:
This example consists of 2 parts, firstly it will produce a list with large numbers, here is "s_array[]", you can increase the number by increasing the number of iterations, and it will use the s_array to produce a "y_array[]" which is the result of Ts(x), namely, the image of x of chebyshev polynomial in s degree, both in expensive and economical algorithms. Then it will use the last element of s_array as secret key s, and secondly last element as r, and perform the encryption & decryption procedures of Chebyshev based cryptosystem.
Be careful to notice that the correctness of recovered plaintext u is related to the precision of MPF structure, and here I set it to be 320, which is 100 digits.

2. chebyshev_economical_test:
This program aims test for a specific number of iterations (s, r), will the economical algorithm change the result from expensive algorithm. The result from this program shows that, although the difference is increasing graduately, and for a costly time of iterations, the difference between them staring to influence the result of recovered plaintext.
But we don't need to worry if our number of iteration is chosen in a time-limited way. And even this happens, we can fix this program by increasing the precision of computation we used here!!! 

3. chebyshev_gmp_relation.cpp:
This program aims test the relationship between l, m, and s as we mentioned in the paper. Namely, for fixed precision l, and m, we want to know how large s and r (r=s-1) is that can be supported to succeed in decryption. 

4. chebyshev_gmp_relation_average.cpp:
After we get the largest s that is supported by l and m, we want to do 20 iterations and store the statistically average values of number of operations, error, and time cost. 
The table in the paper is constructed using this program.

5. chebyshev_encryption_block.cpp:
This program aims test the encryption & decryption of the sysmetric blocking method we metioned in paper.

6. chebyshev_encryption_evp.cpp:
This program aims test the encryption & decryption of the eveloping method we metioned in paper (we use evp from OpenSSL).

7. test_mpfr.cpp:
This program tests the stability of cos() and arccos() function of MPFR library.

8. chebyshev_attack_bergamo.cpp:
This program aims to test the Bergamo's attack to the Chebyshev Cryptosystem in Numerical Implementation.
The current codes will show the Bergamo's attacking procedure of the case theta = 5/18*pi. You can simply compile and run this program, use argument precision = 20, and 97 to get the exact results we've showed in paper.


