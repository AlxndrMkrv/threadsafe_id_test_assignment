#include <iostream>
#include <set>
#include <array>
#include <random>
#include <chrono>
#include <future>
#include "ThreadSafeID.hxx"

const std::vector<std::string> ValidGroups =
    {"A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9",
     "B1", "B2", "B3", "B4", "B5", "B6", "B7", "B8", "B9",
     "C1", "C2", "C3", "C4", "C5", "C6", "C7", "C8", "C9",
     "E1", "E2", "E3", "E4", "E5", "E6", "E7", "E8", "E9",
     "H1", "H2", "H3", "H4", "H5", "H6", "H7", "H8", "H9",
     "I1", "I2", "I3", "I4", "I5", "I6", "I7", "I8", "I9",
     "K1", "K2", "K3", "K4", "K5", "K6", "K7", "K8", "K9",
     "L1", "L2", "L3", "L4", "L5", "L6", "L7", "L8", "L9",
     "N1", "N2", "N3", "N4", "N5", "N6", "N7", "N8", "N9",
     "O1", "O2", "O3", "O4", "O5", "O6", "O7", "O8", "O9",
     "P1", "P2", "P3", "P4", "P5", "P6", "P7", "P8", "P9",
     "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9",
     "S1", "S2", "S3", "S4", "S5", "S6", "S7", "S8", "S9",
     "T1", "T2", "T3", "T4", "T5", "T6", "T7", "T8", "T9",
     "U1", "U2", "U3", "U4", "U5", "U6", "U7", "U8", "U9",
     "W1", "W2", "W3", "W4", "W5", "W6", "W7", "W8", "W9",
     "X1", "X2", "X3", "X4", "X5", "X6", "X7", "X8", "X9",
     "Y1", "Y2", "Y3", "Y4", "Y5", "Y6", "Y7", "Y8", "Y9",
     "Z1", "Z2", "Z3", "Z4", "Z5", "Z6", "Z7", "Z8", "Z9"};

const std::vector<std::string> InvalidGroups =
    {"D1", "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9",
     "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9",
     "G1", "G2", "G3", "G4", "G5", "G6", "G7", "G8", "G9",
     "J1", "J2", "J3", "J4", "J5", "J6", "J7", "J8", "J9",
     "M1", "M2", "M3", "M4", "M5", "M6", "M7", "M8", "M9",
     "Q1", "Q2", "Q3", "Q4", "Q5", "Q6", "Q7", "Q8", "Q9",
     "V1", "V2", "V3", "V4", "V5", "V6", "V7", "V8", "V9",
     "A0", "B0", "C0", "D0", "E0", "F0", "G0", "H0", "I0",
     "J0", "K0", "L0", "M0", "N0", "O0", "P0", "Q0", "R0",
     "S0", "T0", "U0", "V0", "W0", "X0", "Y0", "Z0"};

// Initialize the random number generator with current time
std::mt19937 rng(
    std::chrono::system_clock::now().time_since_epoch().count());

/* Create a random ID consisted of 1 to 10 groups */
std::string RandomId() {
    auto getRandom = [&](const unsigned int &a, const unsigned int &b) {
        return std::uniform_int_distribution<unsigned int>(a, b-1)(rng);
    };

    std::string stringId;
    for (int group = getRandom(1, ThreadSafeID::GroupsLimit); group >= 0;
         --group) {
        stringId += ValidGroups[getRandom(0, ValidGroups.size())];
        if (group)
            stringId += '-';
    }
    return stringId;
}


/* Test the group's IsValid() method */
bool TestGroupValidityEvaluation() {
    for (auto id : ValidGroups)
        if (! SingleGroup::IsValid(id[0], id[1]))
            return false;

    for (auto id : InvalidGroups)
        if (SingleGroup::IsValid(id[0], id[1]))
            return false;

    return true;
}


/* Test the group's increment() method */
bool TestGroupIncrementation() {
    SingleGroup obj;

    for (auto &id : ValidGroups) {
        if (obj.value() != id)
            return false;
        obj.increment();
    }

    return obj.value() == ValidGroups[0];
}


/* Test the set() method */
bool TestIdSet(int tries = 1000) {
    ThreadSafeID id;

    while (--tries) {
        std::string stringId = RandomId();

        // Try to set string id.
        try {
            id.set(stringId);
        } catch (std::exception &) {
            return false;
        }

        // Stop if strings not equal
        if (id.value() != stringId)
            return false;
    }
    return true;
}


/* Test that invalid IDs will throw exceptions */
bool TestInvalidIdSet() {
    ThreadSafeID id;

    try {
        id.set("-A2");
        return false;
    } catch (std::exception &) {}

    try {
        id.set("A5-");
        return false;
    } catch (std::exception &) {}

    try {
        id.set("A8_A5");
        return false;
    } catch (std::exception &) {}

    try {
        id.set("4-A 6");
        return false;
    } catch (std::exception &) {}

    return true;
}


/* Test the creation of new groups */
bool TestIdOverflow() {
    ThreadSafeID id;
    id.set("Z9");
    if (id.increment() != "A1-A1")
        return false;

    id.set("Z9-Z9");
    if (id.increment() != "A1-A1-A1")
        return false;

    id.set("Z9-Z9-Z9-Z9-Z9-Z9-Z9-Z9-Z9-Z9");
    if (id.increment() != "A1")
        return false;

    return true;
}


/* Test the multi-threaded access to increment() and set() methods.
 * Note: if this test fails with std::system_error,
 * check /proc/sys/kernel/threads-max and prlimit NPROC value then try to
 * reduce the iterations number */
bool TestThreads(const int &iterations = 30000) {
    ThreadSafeID id, checkId;
    std::vector<std::future<void>> futures;
    for (int i = 0; i < iterations; ++i)
        futures.push_back(std::async(std::launch::async,
                                     [&id](){id.increment();}));

    for (auto &itr : futures)
        itr.get();

    for (int i = 0; i < iterations; ++i)
        checkId.increment();

    /* ID incremented by multiple threads must be equal to
     * ID incremented by this thread */
    if (checkId.value() != id.value())
        return false;

    futures.clear();

    /* The following code will lead to an application fault on unsafe set()
     * implementation */
    for (int i = 0; i < iterations; ++i)
        futures.push_back(std::async(std::launch::async,
                                     [&id](){id.set(RandomId());}));
    for (auto &itr : futures)
        itr.get();

    return true;
}


int main () {
    std::cout << std::thread::hardware_concurrency() << std::endl;

    std::cout << "Test single group validity evaluation: "
              << (TestGroupValidityEvaluation() ? "Ok" : "Fail") << std::endl;

    std::cout << "Test single group incrementation: "
              << (TestGroupIncrementation() ? "Ok" : "Fail") << std::endl;

    std::cout << "Test full ID set() method: "
              << (TestIdSet() ? "Ok" : "Fail") << std::endl;

    std::cout << "Test setting invalid IDs: "
              << (TestInvalidIdSet() ? "Ok" : "Fail") << std::endl;

    std::cout << "Test full ID incrementation overflow: "
              << (TestIdOverflow() ? "Ok" : "Fail") << std::endl;

    std::cout << "Test multi-threaded access: "
              << (TestThreads() ? "Ok" : "Fail") << std::endl;
    return 0;
}
