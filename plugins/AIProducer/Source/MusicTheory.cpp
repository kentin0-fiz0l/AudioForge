#include "MusicTheory.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace MusicTheory {

const std::vector<int>& scaleIntervals(bool minor) {
    static const std::vector<int> major = {0, 2, 4, 5, 7, 9, 11};
    static const std::vector<int> naturalMinor = {0, 2, 3, 5, 7, 8, 10};
    return minor ? naturalMinor : major;
}

int degreeToPitch(int root, bool minor, int degree) {
    const int octave = degree >= 0 ? degree / 7 : (degree - 6) / 7; // Rounds down
    const int step = degree - octave * 7;
    return root + octave * 12 + scaleIntervals(minor)[static_cast<size_t>(step)];
}

std::vector<int> parseProgression(const std::string& progression) {
    static const std::vector<std::string> numerals = {"I", "II", "III", "IV", "V", "VI", "VII"};

    std::vector<int> degrees;
    std::istringstream stream(progression);
    std::string token;

    while (std::getline(stream, token, '-')) {
        std::transform(token.begin(), token.end(), token.begin(),
                       [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
        const auto found = std::find(numerals.begin(), numerals.end(), token);
        degrees.push_back(found == numerals.end() ? 0 : static_cast<int>(found - numerals.begin()));
    }

    return degrees;
}

} // namespace MusicTheory
