emp-printer example/sha256x100.rel prover > p.cpp
emp-printer example/sha256x100.rel verifier > v.cpp
g++ p.cpp -lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -std=c++11 -march=native -pthread -Wall -funroll-loops -lcrypto -O3 -op.out
g++ v.cpp -lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -std=c++11 -march=native -pthread -Wall -funroll-loops -lcrypto -O3 -ov.out
time ./p.out 1 12345 127.0.0.1 4 example/sha.ins example/sha.wit & ./v.out 2 12345 127.0.0.1 4 example/sha.ins

emp-printer example_arithmetic/matprod100.rel prover > p.cpp
emp-printer example_arithmetic/matprod100.rel verifier > v.cpp
g++ p.cpp -lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -std=c++11 -march=native -pthread -Wall -funroll-loops -lcrypto -O3 -op.out
g++ v.cpp -lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -std=c++11 -march=native -pthread -Wall -funroll-loops -lcrypto -O3 -ov.out
time ./p.out 1 12345 127.0.0.1 4 example_arithmetic/matprod100.ins example_arithmetic/matprod100.wit & ./v.out 2 12345 127.0.0.1 4 example_arithmetic/matprod100.ins
