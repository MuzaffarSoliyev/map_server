#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include "ServerConfig.h"

#include <iostream>
#include <cstdio>

using namespace boost::asio;
using ip::tcp;
using std::cout;
using std::endl;


std::mutex mutex_;

//size_t num_files;
size_t ndim1;
size_t ndim2;
int step;
std::string dir;
bool VERBOSE;
int n = 10;
std::vector<std::vector<int>> map(n);
std::vector<std::vector<int>> used(n);
std::vector<std::pair<int, int>> path;

  




class con_handler : public boost::enable_shared_from_this<con_handler> {
public:
    using ptr_to_connection = boost::shared_ptr<con_handler>;


    explicit con_handler(boost::asio::io_context& io_context)
        : socket_(io_context) {}


    static ptr_to_connection create(boost::asio::io_context& io_context) {
        return ptr_to_connection(new con_handler(io_context));
    }


    tcp::socket& socket() { return socket_; }


    void start() {
        socket_.async_read_some(
            boost::asio::buffer(in_message, BUFFER_SIZE),
            boost::bind(&con_handler::handle_read, shared_from_this(),
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }


    void handle_read(const boost::system::error_code& err,
        size_t bytes_transferred) {
        if (!err) {
            if (VERBOSE) {
                cout << "Server recieved from client: " << in_message << endl;
            }
            std::string send_str = parse_command_str(in_message);
            send_str += "\n";
            socket_.async_write_some(
                boost::asio::buffer(send_str, send_str.size()),
                boost::bind(&con_handler::handle_write, shared_from_this(),
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
            //cout << out_message.size();
        }
        else {
            std::cerr << "error: " << err.message() << std::endl;
            socket_.close();
        }
    }

    void handle_write(const boost::system::error_code& err,
        size_t bytes_transferred) {
        if (!err) {
            if (VERBOSE) {
                cout << "Server successfully sent to the client: "
                    << endl;
            }
        }
        else {
            std::cerr << "error: " << err.message() << endl;
            socket_.close();
        }
    }


private:
    static const size_t BUFFER_SIZE = 1024;  /// fixed size buffer
    tcp::socket socket_;
    char in_message[BUFFER_SIZE];
    std::string out_message;


    std::string parse_command_str(std::string str) {
        std::istringstream ss(str);
        std::string token;

        std::getline(ss, token, ' ');
        if (VERBOSE) {
            cout << "Command is " << token << ". Parsing command..." << endl;
        }
        
        std::ofstream out1;
        out1.open("server.txt", std::ios_base::app);

        cout << "______start_____"<<endl;
        cout << str;
        cout << "______stop_____"<<endl;
        out1 << "Step: " << step << std::endl;
        out1 << str << std::endl;
        out1.close();
        if (token == "Am") {
          
         

          std::string own_position;
          std::string want_to;
          std::string position;

          std::getline(ss, own_position, ' ');
          std::getline(ss, want_to, ' ');
          std::getline(ss, position, ' ');
          cout << own_position << endl;
          cout << want_to << endl;
          cout << position << endl;

          // Got own position and want to

          // ------ main function ------

          // bool bfs(vector<vector<int>>& map, int n,
          std::vector<std::string> own_vec;
          boost::split(own_vec, own_position, boost::is_any_of(","));
          std::vector<std::string> position_vec;
          boost::split(position_vec, position, boost::is_any_of(","));

          bool can_change_position = false;

          
          std::cout << endl;

          std::pair<int, int> s = {std::stoi(own_vec[0]),
                                   std::stoi(own_vec[1])};
          std::pair<int, int> e = {std::stoi(position_vec[0]),
                                   std::stoi(position_vec[1])};
          std::cout << "START POS: " << s.first << " " << s.second << std::endl; 
          std::cout << "END POS: " << e.first << " " << e.second << std::endl;
          std::cout << "STEP: " << step << std::endl;
          std::queue<std::pair<int, int>> q;

          q.push(s);
          used[s.first][s.second] = true;
          while (!q.empty()) {
            std::pair<int, int> v = q.front();

            q.pop();
            if (v.first < n - step) {
              if (map[v.first + step][v.second] &&
                  !used[v.first + step][v.second]) {
                q.push({v.first + step, v.second});
                used[v.first + step][v.second] = 1;
              }
            }
            if (v.second < n - step) {
              if (map[v.first][v.second + step] &&
                  !used[v.first][v.second + step]) {
                q.push({v.first, v.second + step});
                used[v.first][v.second + step] = 1;
              }
            }
            if (v.first - step > 0) {
              if (map[v.first - step][v.second] &&
                  !used[v.first - step][v.second]) {
                q.push({v.first - step, v.second});
                used[v.first - step][v.second] = 1;
              }
            }
            if (v.second - step > 0) {
              if (map[v.first][v.second - step] &&
                  !used[v.first][v.second - step]) {
                q.push({v.first, v.second - step});
                used[v.first][v.second - step] = 1;
              }
            }
            /*
            if (v.first < n - 1) {
              if (map[v.first + 1][v.second] && !used[v.first + 1][v.second]) {
                q.push({v.first + 1, v.second});
                used[v.first + 1][v.second] = 1;
              }
            }
            if (v.second < n - 1) {
              if (map[v.first][v.second + 1] && !used[v.first][v.second + 1]) {
                q.push({v.first, v.second + 1});
                used[v.first][v.second + 1] = 1;
              }
            }
            if (v.first > 0) {
              if (map[v.first - 1][v.second] && !used[v.first - 1][v.second]) {
                q.push({v.first - 1, v.second});
                used[v.first - 1][v.second] = 1;
              }
            }
            if (v.second > 0) {
              if (map[v.first][v.second - 1] && !used[v.first][v.second - 1]) {
                q.push({v.first, v.second - 1});
                used[v.first][v.second - 1] = 1;
              }
            }
            */
          }

          if (used[e.first][e.second] == 1) {
            can_change_position = true;
          } else {
            can_change_position = false;
          }
        
          if (can_change_position) {
            own_position = position;
            path.push_back(e);
            return "!OK" + own_position;
          } else {
            return "Error you can not change position";
          }

          
        } else if(token == "Where?") {
          return std::to_string(path[path.size() - 1].first) + "," +
                 std::to_string(path[path.size() - 1].second); 
        }
        

    }

};


class Server {

public:

    Server(boost::asio::io_context& io_context, ServerConfig config)
        : acceptor_(
            io_context,
            tcp::endpoint(boost::asio::ip::address::from_string(config.ip),
                config.port)),
        io_context_(io_context)
    {
        start_accept();
        ndim1 = config.ndim1;
        ndim2 = config.ndim2;
        step = config.step;
        dir = config.dir;
        n = ndim1;
        for (int i = 0; i < n; i++) {
          for (int j = 0; j < n; j++) {
            used[i].push_back(false);
            if (rand() % 2) {
              map[i].push_back(1);
            } else {
              map[i].push_back(0);
            }
          }
        }
        map[0][0] = 1;
        path.push_back({0,0});
        std::ofstream out;
        out.open("map.txt");
        for (int i = 0; i < n; i++) {
          for (int j = 0; j < n; j++) {
            //std::cout << map[i][j] << " ";
            out << map[i][j] << " ";
          }
          //std::cout << std::endl;
          out << std::endl;
        }
        out.close();

        VERBOSE = config.verbose;
    }

    void handle_accept(con_handler::ptr_to_connection connection,
        const boost::system::error_code& err) {
        if (!err) {
            connection->start();
        }
        start_accept();
    }

private:
    tcp::acceptor acceptor_;
    io_context& io_context_;


    void start_accept() {
        // socket
        con_handler::ptr_to_connection connection =
            con_handler::create(io_context_);

        // asynchronically accept user response
        acceptor_.async_accept(
            connection->socket(),
            boost::bind(&Server::handle_accept, this, connection,
                boost::asio::placeholders::error));
    }
};