#include <string>

class uWeb
{
public:
    uWeb();
    bool run(const std::string &interface, const std::string &data);

private:
    void sendData(const int socket, const std::string &data) const;
};
