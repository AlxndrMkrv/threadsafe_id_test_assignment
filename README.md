## Thread-safe ID

### Objective

The objective of this test assignment is to create a wrapper object that can handle sequence identifiers with the following rules:

- ID consists of one to ten groups of characters separated by a dash;
- a group is formed by a single letter and a single number like "X4";
- letters "D", "F", "G", "J", "M", "Q", "V" and number "0" must not appear in an ID;
- the first value is "A1" then "A2" and so on until "Z9" is reached. After that the next incrementation must add a new group and nullify the first one so the ID would be "A1-A1";
- an implementation must provide two thread-safe methods: incrementation and set;
- an implementation must be cross-platform and without dependencies.

### Solution

The project consists of three files:
```
# The required implementation
── ThreadSafeID.hxx 
── ThreadSafeID.cxx

# Unit-tests
── Test.cxx
```

And can be compiled with either:
```
GCC:
g++ Test.cxx ThreadSafeID.cxx -o check

Clang:
clang++ Test.cxx ThreadSafeID.cxx -o check

MSVC:
cl.exe /Fe: check.exe /EHsc Test.cxx ThreadSafeID.cxx
```

Upon executing ```check[.exe]``` the program does several unit-tests and prints the results.

*Note that the last test may fail on Linux due to a system limitations. If this happend check /proc/sys/kernel/threads-max and ```prlimit``` output*