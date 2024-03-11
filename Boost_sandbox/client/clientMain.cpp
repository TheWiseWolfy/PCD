#include <iostream>
#include <boost/asio.hpp>
#include <array>

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

int main() {
    int numMessagesRead = 0;
    int numBytesRead = 0;

    try {
        boost::asio::io_context io_context;

        // Protocol selection
        std::string protocol = "tcp"; // Change this to "udp" for UDP

        if (protocol == "tcp") {
            tcp::resolver resolver(io_context);
            auto endpoints = resolver.resolve("192.168.1.15", "18183");

            tcp::socket socket(io_context);
            boost::asio::connect(socket, endpoints);

            // Send an initial message to the server
            std::string initial_message = "Hello, server!\n";
            boost::asio::write(socket, boost::asio::buffer(initial_message));

            while (true) {
                boost::asio::streambuf buffer;
                boost::system::error_code error;

                boost::asio::read(socket, buffer, boost::asio::transfer_at_least(1), error);
                if (error == boost::asio::error::eof)
                    break; // Connection closed cleanly by peer.
                else if (error)
                    throw boost::system::system_error(error); // Some other error.

                // Read data from the buffer and display it
                std::string received_data(boost::asio::buffers_begin(buffer.data()),
                                          boost::asio::buffers_end(buffer.data()));

                std::cout << received_data << std::endl;
                numMessagesRead++;
                numBytesRead += received_data.size();
            }
        } else if (protocol == "udp") {
            udp::resolver resolver(io_context);
            udp::endpoint receiver_endpoint = *resolver.resolve(udp::v4(), "localhost", "18183").begin();

            udp::socket socket(io_context, udp::endpoint(udp::v4(), 0)); // Bind to any available port

            // Send an initial message to the server
            std::string initial_message = "Hello, server!\n";
            socket.send_to(boost::asio::buffer(initial_message), receiver_endpoint);

            while (true) {
                std::array<char, 128> buffer;
                udp::endpoint sender_endpoint;

                size_t length = socket.receive_from(boost::asio::buffer(buffer), sender_endpoint);
                numMessagesRead++;
                numBytesRead += length;

                std::cout.write(buffer.data(), length);
            }
        } else {
            std::cerr << "Invalid protocol specified!" << std::endl;
            return 1;
        }
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    // Output statistics after each session
    std::cout << "Session Summary:" << "\n";
    std::cout << "Number of Messages Read: " << numMessagesRead << "\n";
    std::cout << "Number of Bytes Read: " << numBytesRead << "\n";

    return 0;
}
