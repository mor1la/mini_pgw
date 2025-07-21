
class UdpServer {
public:
    void start();
private:
    int socket_fd_ = -1;
};