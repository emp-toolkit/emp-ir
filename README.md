# EMP-IR
EMP-IR bridges the [EMP-ZK proof system](https://github.com/emp-toolkit/emp-zk) into the [SIEVE IR](https://stealthsoftwareinc.github.io/wizkit-blog/2021/09/20/introducing-the-sieve-ir.html) via two methods.
First, the EMP Compiler can translate from the IR into EMP's C++ dialect/library.
Second, the WizToolKit BOLT API is used to provide an interpreter for the IR.

## EMP Compiler
The EMP Compiler will translate from the SIEVE IR to EMP's C++ dialect which can later be compiled with a C++ compiler.
Once compiled, the the proof can be executed extremely quickly.
This method of execution works best in cases where the relation will be reused many times or the relation makes heavy use of repetition (e.g. loops and function gates).

The compiler will compile a relation once, and the executable relation may be reused many times with different instances and short witnesses.

The Compiler may be invoked as follows to produce either prover or verifier code.
Compiler arguments are positional -- first the source file then indicate whether to produce code for the ZK prover or verifier.
It will emit C++ code to the `stdout` which may be piped to the compiler or redirected to a source file.

```bash
emp-compile ir_source.rel prover
emp-compile ir_source.rel verifier
```

The following flags are required to compile this output.

```
[ On Apple M1 ] -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib

-lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -lcrypto -pthread

-std=c++11 -march=native -Wall -funroll-loops -O3
```

For example a relation could be compiled like this.

```bash
# setup the flags
FLAGS="-lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -lcrypto -pthread -std=c++11 -march=native -Wall -funroll-loops -O3"

# Compile the prover
emp-compile ir_source.rel prover > prover.cpp
g++ prover.cpp $FLAGS -o prover

#Compile the verifier
emp-compile ir_source.rel verifier > verifier.cpp
g++ verifier.cpp $FLAGS -o verifier
```

To run the prover and verifier use the following positional flags.

1. IP Address
2. Port number
3. Number of background threads. The optimal number of background threads can vary and should be determined experimentally. Usually it is between 1 and 4.
4. Instance file (`filename.ins`)
5. **Prover only** short witness file (`filename.wit`)

Here is an example.
Upon success, both processes will exit silently.
However, if the prover fails to convince the verifier of the relation's truthfullness, the verifier will emit an error message.

```bash
./prover 0.0.0.0 12345 1 instance.ins short_witness.wit &
./verifier 127.0.0.1 12345 1 instance.ins
```

## EMP with WizToolKit BOLT Interpreters
The `emp-interpret` command will prove a relation without a lengthy compile step.
It uses [WizToolKit's BOLT family of interpreters](https://stealthsoftwareinc.github.io/wiztoolkit/docs/v1.0.1/backends.html) with the EMP-ZK backend.
It may be used in cases where compilation is undesirable, such as if the relation is fairly small and may change regularly.
It may also be preferable for use with IR-Simple relations (no loops or functions) which are too large for compilation to EMP C++.

There are three modes of interpretation.

 * **bolt** (default): behaves similar to a just-in-time compiler by creating an optimal in-memory structure before interpretation.
   It is ideal for relations which make good use of functions and for-loops when compilation is undesirable.
 * **stream**: interleaves parsing with handling each gate which eliminates the overhead of building an in-memory parse tree.
   It is the best mode for large IR-Simple relations but it cannot handle loops, functions, or switch statements.
 * **plasmasnooze**: is a naive interpreter which can handle loops, functions, and switch statements.
   It is good for relations which are mostly flat circuits with a small amount of control flow which cannot be handled in streaming.

To invoke `emp-interpret` the following positional command-line arguments are used.

1. Mode (optional, defaults to bolt)
2. IP Address
3. Port number
4. Number of background threads
5. Relation file (`filename.rel`)
6. Instance file (`filename.ins`)
7. **Prover only** short witness file (`filename.wit`)

Here is an example.

```bash
emp-interpret bolt 0.0.0.0 12345 1 relation.rel instance.ins short_witness.wit &
emp-interpret bolt 127.0.0.1 12345 1 relation.rel instance.ins
```

## IR 1 to 0 Converter
The `emp-IR120.sh` script will convert a relation into IR-Simple ("IR1 to IR0" being the shorthand from which the script's name is derived).
It works using the EMP compiler to build a C++ program which, rather than proving or verifying, will print out each gate.

```
emp-IR120.sh source.rel destination-ir0.rel
```

## Installation

1. Follow these [instructions to install WizToolKit](https://stealthsoftwareinc.github.io/wiztoolkit/docs/v1.0.1/install.html)
2. Install EMP-ZK with this script `wget https://raw.githubusercontent.com/emp-toolkit/emp-readme/master/scripts/install.py && python3 install.py -install --tool --ot --zk`
3. Install EMP-IR from this repo by running: `cmake . && make && sudo make install`

## Examples

### Example for Boolean circuits
This example will compile and prove a SHA256 circuit.

1. `emp-compile example/sha256.rel prover > p.cpp`
2. `emp-compile example/sha256.rel verifier > v.cpp`
3. `g++ p.cpp -lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -std=c++11 -march=native -pthread -Wall -funroll-loops -lcrypto -O3 -op.out`
4. `g++ v.cpp -lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -std=c++11 -march=native -pthread -Wall -funroll-loops -lcrypto -O3 -ov.out`
5. `./p.out 0.0.0.0 12345 4 example/sha.ins example/sha.wit & ./v.out 127.0.0.1 12345 4 example/sha.ins`

Alternatively, with the emp-interpret command.

1. `emp-interpret 0.0.0.0 12345 4 example/sha256.rel example/sha.ins example/sha.wit & emp-interpret 127.0.0.1 12345 4 example/sha256.rel example/sha.ins`

### Example for arithmetic circuits
This example will compile and prove a small matrix product circuit.

1. `emp-compile example_arithmetic/matprod5.rel prover > p.cpp`
2. `emp-compile example_arithmetic/matprod5.rel verifier > v.cpp`
3. `g++ p.cpp -lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -std=c++11 -march=native -pthread -Wall -funroll-loops -lcrypto -O3 -op.out`
4. `g++ v.cpp -lstealth_logging -lwiztoolkit -lemp-tool -lemp-zk -I/usr/local/opt/openssl/include -L/usr/local/opt/openssl/lib -std=c++11 -march=native -pthread -Wall -funroll-loops -lcrypto -O3 -ov.out`
5. `./p.out 0.0.0.0 12345 4 example_arithmetic/matprod5.ins example_arithmetic/matprod5.wit & ./v.out 127.0.0.1 12345 4 example_arithmetic/matprod5.ins`

Alternatively, with the emp-interpret command.

1. `emp-interpret 0.0.0.0 12345 4 example_arithmetic/matprod5.rel example_arithmetic/matprod5.ins example_arithmetic/matprod5.wit & emp-interpret 127.0.0.1 12345 4 example_arithmetic/matprod5.rel example_arithmetic/matprod5.ins`

### Mega circuit
These examples will create super large relations using IR-Simple.

- `emp-IR120.sh example/sha256x100.rel test.rel`
- `emp-IR120.sh example/sha256x1000.rel test.rel`
- `emp-IR120.sh example/sha256x10000.rel test.rel`
- `emp-IR120.sh example_arithmetic/matprod100.rel test.rel`
- `emp-IR120.sh example_arithmetic/matprod500.rel test.rel`
- `emp-IR120.sh example_arithmetic/matprod1000.rel test.rel`

WARNING: the largest file is about 60GB!
Verify by `wtk-firealarm test.rel example/sha.ins example/sha.wit` or
`wtk-firealarm test.rel example_arithmetic/matprod{n}.ins example_arithmetic/matprod{n}.wit`.

## Question
For any question, please contact the developers Kimberlee I. Model and Xiao Wang.

## Licensing and Acknowledgements

This work is open source licensed using the [MIT License](./LICENSE).

**Distribution Statement A:** Approved for Public Release, Distribution Unlimited.

This material is based upon work supported by DARPA under Contract No. HR001120C0087.  Any opinions, findings and conclusions or recommendations expressed in this material are those of the author(s) and do not necessarily reflect the views of DARPA.

Matrix multiplication examples were generated using [wiztoolkit](https://github.com/stealthsoftwareinc/wiztoolkit/blob/master/src/main/python/matrix_prod.py)
