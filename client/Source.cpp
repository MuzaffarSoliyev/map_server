#include <boost/asio.hpp>
#include <iostream>
#include <mutex>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <fstream>


using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;
using std::string;
using std::vector;


std::string test_request_response(const string msg) {

    boost::asio::io_context io_context;

    // socket creation
    tcp::socket socket(io_context);
    // connection
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));

    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg), error);
    if (!error) {
        cout << "Client sent message! " << msg << endl;
    }
    else {
        cout << "send failed: " << error.message() << endl;
    }

    // response from server
    boost::asio::streambuf receive_buffer;
    boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), error);
    if (error && error != boost::asio::error::eof) {
        cout << "receive failed: " << error.message() << error.value() << endl;
    }
    else {
        const char* data =
            boost::asio::buffer_cast<const char*>(receive_buffer.data());

        string data_str(data);
        size_t bsn_pos = data_str.find('\n');

        cout << data_str.substr(0, bsn_pos + 1); // print returned result
        if (msg == "Where?") {
          return data_str.substr(0, bsn_pos + 1);
        } else {
          return data_str.substr(0, bsn_pos + 1);
        }
    }
}



int main(int argc, char** argv) {
    setlocale(LC_ALL, "Russian");
    std::fstream out;
    out.open("client.txt", std::ios_base::app);

    string res = "";
    string pos = std::to_string(1) + "," + std::to_string(1);
    string msg = "Am 0,0 want_to " + pos;
    out << msg << std::endl;
    res = test_request_response(msg);
    out << res << std::endl;
    /*
    msg = "Where?";
    res = test_request_response(msg);
    pos = std::to_string(2) + "," + std::to_string(2);
    msg = "Am " + res + " want_to " + pos;
    res = test_request_response(msg);
    msg = "Where?";
    res = test_request_response(msg);
    pos = std::to_string(3) + "," + std::to_string(2);
    msg = "Am " + res + " want_to " + pos;
    res = test_request_response(msg);
    msg = "Where?";
    res = test_request_response(msg);
    pos = std::to_string(3) + "," + std::to_string(4);
    msg = "Am " + res + " want_to " + pos;
    res = test_request_response(msg);
    */

    for (int i = 0; i < 100; i++) {
      msg = "Where?";
      out << msg << endl;
      res = test_request_response(msg);
      out << res << endl;
      int x = 1 + rand() % 10;
      int y = 1 + rand() % 10;
      pos = std::to_string(x) + "," + std::to_string(y);
      msg = "Am " + res + " want_to " + pos;
      out << msg << endl;
      res = test_request_response(msg);
      out << res << endl;
    }
    
    out.close();
    /*for (int i = 0; i < 10; i++) {
      string pos = std::to_string(i) + "," + std::to_string(i);
      string msg = "Am 0,0 want_to " + pos;
    
      test_request_response(msg);
    }
    
    // test many requests
    std::vector<std::thread> threads;
    const size_t USER_NUM = 10;

    for (int i = 0; i < USER_NUM; i++) {
        vector<string> requests;
        string pos = std::to_string(i) + "," + std::to_string(i);
        string msg = "Am 0,0 want_to " + pos;
        threads.push_back(std::thread(&test_request_response, msg));
    }

    for (int i = 0; i < USER_NUM; i++) {
        threads.at(i).join();
    }*/

    return 0;
}

