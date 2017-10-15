#include "uweb.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cout << argv[0] << " interface file" << std::endl;
        std::cout << "Example:" << std::endl;
        std::cout << argv[0] << " bat0 /var/www/keyXdataBat0" << std::endl;
        return -1;
    }

    std::ifstream keyFile(argv[2], std::ios::binary);
    if (!keyFile.is_open())
    {
        std::cerr << "Can't read file " << argv[2] << std::endl;
        return -1;
    }

    std::string data((std::istreambuf_iterator<char>(keyFile)),
                     std::istreambuf_iterator<char>());

    keyFile.close();

    uWeb web;

    if (web.run(argv[1], data))
    {
        return 0;
    }

    return -1;
}
