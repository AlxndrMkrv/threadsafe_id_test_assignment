#include "ThreadSafeID.hxx"
#include <stdexcept>

/* ************************************************************************** *
 *                         SingleGroup implementation                         */

bool SingleGroup::IsValid(const char &letter, const char &number,
                          int *letterIndex)
{
    if (letterIndex)
        *letterIndex = -1;

    // Find letter index
    for (int i = 0; i < LettersNumber; ++i)
        if (letter == ValidLetters[i]) {
            if (letterIndex)
                *letterIndex = i;
            return FirstNumber <= number && number <= LastNumber;
        }

    // Return false if letter not found
    return false;
}

SingleGroup::SingleGroup() :
    _id{'A', '1'}, _letterIndex(0)
{}

void SingleGroup::operator=(const SingleGroup &other)
{
    _id[0] = other._id[0];
    _id[1] = other._id[1];
    _letterIndex = other._letterIndex;
}

void SingleGroup::set(const char &letter, const char &number, bool *success)
{
    // Reset success flag if provided
    if (success)
        *success = false;

    int letterIndex;
    // If letter and number is valid
    if (IsValid(letter, number, &letterIndex)) {
        _letterIndex = letterIndex;
        _id[0] = letter;
        _id[1] = number;
        // Set success flag if provided
        if (success)
            *success = true;
    }
}

void SingleGroup::increment(bool *overflow)
{
    // Reset the overflow flag if given
    if (overflow)
        *overflow = false;

    // Increment the number and then the letter if the number is overflowed.
    // If the letter also overflowed, start over.
    if (++_id[1] > LastNumber) {
        _id[1] = FirstNumber;
        if (++_letterIndex >= LettersNumber) {
            _letterIndex = 0;
            // Set overflow flag if given
            if (overflow)
                *overflow = true;
        }
        _id[0] = ValidLetters[_letterIndex];
    }
}

std::string SingleGroup::value() const
{
    return std::string() + _id[0] + _id[1];
}

/* ************************************************************************** *
 *                        ThreadSafeID implementation                         */

std::vector<SingleGroup> ThreadSafeID::ExtractGroups(const std::string &id)
{
    std::vector<SingleGroup> groups = {};
    bool success = false;
    // Proceed if the length of the string is ok
    if (2 <= id.size() && id.size() <= MaxStringSize) {
        const auto lastGroupItr = id.end() - 2;
        const unsigned int backstep = 3; // 2 chars + separator
        auto itr = lastGroupItr;
        while (itr >= id.begin()) {
            // After the first group check if the separator presents
            if (itr == lastGroupItr || *(itr+2) == Separator) {
                groups.push_back(SingleGroup());
                groups.back().set(*itr, *(itr+1), &success);
                // Break if set(letter, number) failed
                if (! success)
                    break;
            }
            // Break if the separator is invalid
            else {
                success = false;
                break;
            }
            // Move the iterator one group back
            itr -= backstep;
        }

        // Check if the iterator is exactly one group back from the ID begin().
        // This filters out incomplete IDs like "-A5-A1"
        if (id.begin() - itr != backstep)
            success = false;
    };

    // Return the extracted groups on success and an empty vector on fail
    return success ? groups : std::vector<SingleGroup>{};
}

ThreadSafeID::ThreadSafeID() :
    _id{SingleGroup()}
{
    _id.reserve(GroupsLimit);
}

void ThreadSafeID::set(const std::string &id)
{
    std::lock_guard<decltype(_mutex)> lock(_mutex);

    // Extract groups from the given string
    std::vector<SingleGroup> newValue = ExtractGroups(id);

    // Refresh the inner ID or throw an exception if extraction failed
    if (! newValue.size())
        throw std::invalid_argument("Invalid ID");
    else
        _id = newValue;
}

std::string ThreadSafeID::increment()
{
    std::lock_guard<decltype(_mutex)> lock(_mutex);

    // Propagate incrementation thru the groups. Stop on overflow drop
    bool overflow = true;
    for (auto &itr : _id) {
        if (overflow)
            itr.increment(&overflow);
        else
            break;
    }

    // If overflow is still high, add a new group or start over
    if (overflow) {
        if (_id.size() < GroupsLimit)
            _id.push_back(SingleGroup());
        else
            _id = {SingleGroup()};
    }

    return value();
}

std::string ThreadSafeID::value()
{
    std::lock_guard<decltype(_mutex)> lock(_mutex);

    std::string output;
    for (int i = _id.size() - 1; i >= 0; --i) {
        output += _id[i].value();
        // Add separator if the group is not last
        if (i)
            output += Separator;
    }
    return output;
}
