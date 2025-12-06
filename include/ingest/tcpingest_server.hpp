#include "ingest_server.hpp"

namespace Ingest {

    class TcpIngestServer : public IngestServer {
    public:
        TcpIngestServer(EventStream::EventBus& bus, int port);
        ~TcpIngestServer();
        void start() override;
        void stop() override;
    
    private:
        void acceptConnections() override;
        void handleClient(int client_fd , std::string client_address);
        
        int serverPort;
        int server_fd;
        std::atomic<bool> isRunning;
        std::thread acceptThread;
        EventStream::EventBus& eventBus;
        std::vector<std::thread> clientThreads;
    };
}