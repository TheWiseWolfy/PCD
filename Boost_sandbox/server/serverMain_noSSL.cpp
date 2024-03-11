#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <algorithm>

namespace asio = boost::asio;
namespace beast = boost::beast;
namespace http = beast::http;
namespace ip = boost::asio::ip;

using tcp = asio::ip::tcp;




std::vector<char> loadFileIntoMemory(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file: " << filename << std::endl;
        return {};
    }

    file.seekg(0, std::ios::end);
    std::ifstream::pos_type fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    std::cout<< "File is " << fileSize << "big\n";


    std::vector<char> buffer(fileSize);
    if (!file.read(buffer.data(), fileSize)) {
        std::cerr << "Error: Failed to read file: " << filename << std::endl;
        return {};
    }

    std::cout<< "Buffer is " << buffer.size() << "big\n";
    file.close();
    return buffer;
}



void handleTCPTransmission(asio::io_context& ioc,const std::vector<char>& data, int maxChunkSize = 64){

    // Define variables to track statistics
    int numMessagesSent = 0;
    int numBytesSent = 0;
    std::chrono::steady_clock::time_point startTime, endTime;

    try {
        tcp::acceptor acceptor(ioc, tcp::endpoint(tcp::v4(), 18183)); // Replace with your desired port
        acceptor.listen();
        std::cout << "Server initialized for TCP\n";

        tcp::socket socket(ioc);
        std::cout << "Accepted TCP client\n";

        acceptor.accept(socket);

        boost::asio::streambuf receive_buffer;
        boost::asio::read_until(socket, receive_buffer, "\n");
        std::string received_message = boost::asio::buffer_cast<const char*>(receive_buffer.data());

        std::cout << "Received message from client: " << received_message << std::endl;

        // Start the timer at the beginning of the transmission
        startTime = std::chrono::steady_clock::now();

        int dataSize = data.size();
        int numberOfBooks = 5000;

        for(int i = 0; i < numberOfBooks; i++) {
            int bytesSent = 0;
            while (bytesSent < dataSize) {
                std::size_t chunkSize = std::min(maxChunkSize, dataSize - bytesSent);

                // Send the HTTP response to the client
                boost::system::error_code error;
                auto toSendBuffer = boost::asio::buffer(data.data() + bytesSent, chunkSize);

                std::string bufferString(boost::asio::buffer_cast<const char *>(toSendBuffer),
                                         boost::asio::buffer_size(toSendBuffer));

                //std::cout <<  bufferString << "\n"
                std::size_t bytesTransferred = boost::asio::write(socket, toSendBuffer, error);

                if (error && error != boost::asio::error::eof) {
                    std::cerr << "Error during send: " << error.message() << std::endl;
                    break; // Exit the loop if an error occurs
                }

                // Update statistics
                numMessagesSent++;
                numBytesSent += bytesTransferred; // Assuming bytesSent is updated in your loop
                bytesSent += bytesTransferred;

                if (error) {
                    throw boost::system::system_error(error); // Handle the error appropriately
                }
            }
        }

        endTime = std::chrono::steady_clock::now();
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    std::chrono::duration<double> transmissionTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
    // Print transmission statistics
    std::cout << "Transmission Time: " << transmissionTime.count() << " seconds" << std::endl;
    std::cout << "Number of Sent Messages: " << numMessagesSent << std::endl;
    std::cout << "Number of Bytes Sent: " << numBytesSent << std::endl;
}

void handleUDPTransmission(asio::io_context& ioc, const std::vector<char>& data, const asio::ip::udp::endpoint& endpoint, int maxChunkSize = 64) {
    try {
        asio::ip::udp::socket socket(ioc, asio::ip::udp::endpoint(asio::ip::udp::v4(), 18183)); // Replace with your desired port
        std::cout << "Server initialized for UDP\n";

        // Start the timer at the beginning of the transmission
        std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

        int dataSize = data.size();
        int numberOfBooks = 5000;

        for(int i = 0; i < numberOfBooks; i++) {
            int bytesSent = 0;
            while (bytesSent < dataSize) {
                std::size_t chunkSize = std::min(maxChunkSize, dataSize - bytesSent);

                // Send the data
                boost::system::error_code error;
                socket.send_to(asio::buffer(data.data() + bytesSent, chunkSize), endpoint, 0, error);

                if (error) {
                    std::cerr << "Error during send: " << error.message() << std::endl;
                    break; // Exit the loop if an error occurs
                }

                // Update statistics
                bytesSent += chunkSize;
            }
        }

        // Stop the timer
        std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();

        // Calculate transmission time
        std::chrono::duration<double> transmissionTime = std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);

        // Print transmission statistics
        std::cout << "Transmission Time: " << transmissionTime.count() << " seconds" << std::endl;
        std::cout << "Number of Sent Messages: " << numberOfBooks << std::endl;
        std::cout << "Number of Bytes Sent: " << dataSize * numberOfBooks << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}


int main(int argc, char *argv[]) {
    std::cout << "Server initializing....\n";

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <protocol>" << "\n";
        return 1;
    }
    std::string protocol = argv[1];

    std::string filename = "alice_in_wonderland.txt"; // Replace with your file path
    std::vector<char> fileData = loadFileIntoMemory(filename);

    asio::io_context ioc;
    if (protocol == "TCP") {
        handleTCPTransmission(ioc, fileData);
    }
    else if (protocol == "UDP") {
        handleUDPTransmission(ioc, fileData, asio::ip::udp::endpoint(asio::ip::udp::v4(), 18182));

    }else {
        std::cerr << "Invalid protocol. Supported protocols: TCP, UDP\n";
        return 1;
    }



    return 0;





















    return 0;
}

