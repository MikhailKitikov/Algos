#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

std::vector<int> zFunction(std::string alpha) {
    int len = alpha.size();
    std::vector<int> zFunc(len, 0);
    int zBlockLeft = 0, zBlockRight = 0;

    for (int index = 1; index < len; ++index) {
        int zVal = 0;
        if (index <= zBlockRight) {
            zVal = std::min(zFunc[index - zBlockLeft], zBlockRight - index + 1);
        }
        while ((index + zVal < len) && (alpha[zVal + index] == alpha[zVal])) {
            ++zVal;
        }
        if (index + zVal > zBlockRight + 1) {
            zBlockLeft = index;
            zBlockRight = index + zVal - 1;
        }
        zFunc[index] = zVal;
    }

    return zFunc;
}

int main() {
    std::string pattern, text;
    std::cin >> pattern >> text;
    int patternLen = pattern.size(), textLen = text.size();

    std::string patternReverse(pattern), textReverse(text);
    reverse(patternReverse.begin(), patternReverse.end());
    reverse(textReverse.begin(), textReverse.end());

    std::vector<int> zFuncForward = zFunction(pattern + '$' + text);
    std::vector<int> zFuncBackward = zFunction(patternReverse + '$' + textReverse);

    int countFound = 0;
    std::vector<int> foundPos;

    for (int index = 0; index <= textLen - patternLen; ++index) {

        int sLeft = zFuncForward[index + patternLen + 1];
        int sRight = zFuncBackward[textLen - index + 1];

        if (sLeft == patternLen || sLeft + sRight + 1 == patternLen) {
            ++countFound;
            foundPos.push_back(index + 1);
        }
    }

    std::cout << countFound;

    if (countFound) {
        std::cout << '\n';
        for (auto x : foundPos) {
            std::cout << x << ' ';
        }
    }

    return 0;
}
