#pragma once
#include <string>
#include <vector>

class ResourceLocator {
public:
    ResourceLocator();
    ~ResourceLocator();

    static std::vector<std::string> getMusicFragmentsWithin(std::string dirPath);

private:

};