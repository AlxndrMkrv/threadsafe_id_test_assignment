#pragma once

#include <string>
#include <vector>
#include <mutex>

class SingleGroup
{
public:
    static constexpr unsigned char LettersNumber = 19;
    static constexpr char ValidLetters [LettersNumber] = {'A', 'B', 'C', 'E',
                                                          'H', 'I', 'K', 'L',
                                                          'N', 'O', 'P', 'R',
                                                          'S', 'T', 'U', 'W',
                                                          'X', 'Y', 'Z'};
    static constexpr char FirstNumber = '1';
    static constexpr char LastNumber = '9';

public:
    static bool IsValid(const char &letter, const char &number,
                        int *letterIndex = nullptr);

public:
    SingleGroup();
    void operator=(const SingleGroup &other);

    void set(const char &letter, const char &number, bool * success = nullptr);
    void increment(bool * overflow = nullptr);
    std::string value() const;

private:
    char _id [2];
    unsigned int _letterIndex;
};


class ThreadSafeID
{
public:
    static constexpr unsigned int GroupsLimit = 10;
    static constexpr unsigned int MaxStringSize =
        GroupsLimit * 2 + (GroupsLimit-1);
    static constexpr char Separator = '-';

public:
    static std::vector<SingleGroup> ExtractGroups (const std::string &id);

public:
    ThreadSafeID();
    void operator=(ThreadSafeID &) = delete;

    void set(const std::string &id);
    std::string increment();
    std::string value();

private:
    std::vector<SingleGroup> _id;
    std::recursive_mutex _mutex;
};
