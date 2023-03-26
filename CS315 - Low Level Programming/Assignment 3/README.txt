Diren D Bharwani
2002216
diren.dbharwani

CSD3115 - Assignment 3
--------

Reference Machine Specifications
--------

CPU Model         : Intel(R) Core(TM) i7-10750H CPU @ 2.60GHz
# of Cores        : 6
# of Threads      : 12
Hyperthreading?   : Yes
Size of L3 Cache  : 12288 KB      (12 MB)
CPU Frequency     : 2591.988 MHz  (2.60 GHz)
Number of FUs     : 8

  FU Port 0
  --------
  Operation                     Latency

  Integer Arithmetic            1
  Vector Arithmetic             1 
  Logic                         1
  Shift                         1
  Vector String Instructions    3
  Floating-point Addition       4
  Floating-point Multiplcation  4
  FMA                           4
  AES Encryption                4
  Integer Vector Multiplication 5
  Integer Division              Variable
  Floating-point Division       Variable
  Squareroot                    Variable
  Branch                        1-2
  --------

  FU Port 1
  --------
  Operation                     Latency

  Integer Arithmetic            1
  Vector Arithmetic             1 
  Logic                         1
  Shift                         1
  Integer Multiplication        3
  Bit Scan                      3
  Floating-point Addition       4
  Floating-point Multiplcation  4
  FMA                           4
  Integer Vector Multiplication 5
  --------

  FU Port 2
  --------
  Operation                     Latency

  Load                          -
  Address Generation            -
  --------

  FU Port 3
  --------
  Operation                     Latency

  Load                          -
  Address Generation            -
  --------

  FU Port 4
  --------
  Operation                     Latency

  Store                         -
  --------

  FU Port 5
  --------
  Operation                     Latency

  Integer Arithmetic            1
  Vector Arithmetic             1 
  Logic                         1
  Vector Permute                1 (3 if lane crossed)
  x87 Floating-point Addition   3
  SADBW                         3
  PCLMUL                        7
  --------

  FU Port 6
  --------
  Operation                     Latency

  Integer Arithmetic            1
  Logic                         1
  Shift                         1
  Jump                          1-2
  Branch                        1-2
  --------

  FU Port 7
  --------
  Operation                     Latency

  Load                          -
  Store                         -
  Address Generation            -
  --------

Latencies & Issue Data for Specific Operations

                  |          Integer        | Single/Double Precision |
     Operation    |   Latency   |   Issue   |   Latency   |   Issue   |
  Addition        |      1      |     6     |      4      |     6     |
  Multiplication  |      3      |     6     |      4      |     6     |
--------