ALL: example chebyshev_economical_test chebyshev_gmp_relation chebyshev_gmp_relation_average chebyshev_encryption_block chebyshev_encryption_evp test_mpfr chebyshev_attack_bergamo

example:
	g++ -o example example.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp
	
chebyshev_economical_test:
	g++ -o chebyshev_economical_test chebyshev_economical_test.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp

chebyshev_gmp_relation:
	g++ -o chebyshev_gmp_relation chebyshev_gmp_relation.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp

chebyshev_gmp_relation_average:
	g++ -o chebyshev_gmp_relation_average chebyshev_gmp_relation_average.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp

chebyshev_encryption_block:
	g++ -o chebyshev_encryption_block chebyshev_encryption_block.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp

chebyshev_encryption_evp:
	g++ -o chebyshev_encryption_evp chebyshev_encryption_evp.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp -lssl -lcrypto




test_mpfr:
	g++ -o test_mpfr test_mpfr.cpp -lmpfr -lgmpxx -lgmp

chebyshev_attack_bergamo:
	g++ -o chebyshev_attack_bergamo chebyshev_attack_bergamo.cpp chebyshev.cpp -lmpfr -lgmpxx -lgmp
	
	
	
		
clean:
	rm -rf example chebyshev_economical_test chebyshev_gmp_relation chebyshev_gmp_relation_average chebyshev_encryption_block chebyshev_encryption_evp test_mpfr chebyshev_attack_bergamo
