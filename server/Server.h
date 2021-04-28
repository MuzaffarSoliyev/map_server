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
std::vector<std::pair<int, int>> path_hist;

int** lab;
int** visited;
int** path;

  




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

    void find_path(int n, int row, int col, int** lab, int** visited,
                   int** path, std::queue<int>& plan, int x_end, int y_end) {
      if (!visited[row][col]) {
        /* проверяем не вышли ли мы за границы лабиринта, есть ли клетка
        в массиве посещенных и можно ли через нее пройти*/
        if ((row + 1) < n && (row + 1) >= 0 && !visited[row + 1][col] &&
            (lab[row + 1][col] == 1 ||
             (row + 1 == x_end && col == y_end) /*lab[row + 1][col] == 'X'*/)) {
          path[row + 1][col] = path[row][col] + 1;
          plan.push(row + 1);
          plan.push(col);
        }
        if ((row - 1) < n && (row - 1) >= 0 && !visited[row - 1][col] &&
            (lab[row - 1][col] == 1 || (row - 1 == x_end && col == y_end)
             )) {
          path[row - 1][col] = path[row][col] + 1;
          plan.push(row - 1);
          plan.push(col);
        }
        if ((col + 1) < n && (col + 1) >= 0 && !visited[row][col + 1] &&
            (lab[row][col + 1] == 1 ||
             (row == x_end &&
              col + 1 == y_end) )) {
          path[row][col + 1] = path[row][col] + 1;
          plan.push(row);
          plan.push(col + 1);
        }
        if ((col - 1) < n && (col - 1) >= 0 && !visited[row][col - 1] &&
            (lab[row][col - 1] == 1 ||
             (row == x_end && col - 1 == y_end) )) {
          path[row][col - 1] = path[row][col] + 1;
          plan.push(row);
          plan.push(col - 1);
        }
        visited[row][col] = 1; /* отмечаем клетку в которой побывали */
      }
    }


    std::string parse_command_str(std::string str) {
        std::istringstream ss(str);
        std::string token;

        std::getline(ss, token, ' ');
        if (VERBOSE) {
            cout << "Command is " << token << ". Parsing command..." << endl;
        }
        
        std::ofstream out1;
        out1.open("server_" + std::to_string(step) + ".txt", std::ios_base::app);

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

          int x_start = std::stoi(own_vec[0]),
                 y_start = std::stoi(own_vec[1]),
                 x_end = std::stoi(position_vec[0]),
                 y_end = std::stoi(position_vec[1]), x, y,
                 len = 0;
          int tmp_x = -1, tmp_y = -1;

          if (x_start == x_end && y_start == y_end) {
            return "!OK " + own_position;
            can_change_position = true;
          }
          std::queue<int> plan;

          std::cout << "START POS: " << x_start << " " << y_start << std::endl; 
          std::cout << "END POS: " << x_end << " " << y_end << std::endl;
          std::cout << "STEP: " << step << std::endl;
          
          plan.push(x_start);  // заносим начальную клетку
          plan.push(y_start);  //в план посещения
          path[x_start][y_start] = 1;
          while (!plan.empty()) { /* пока очередь посещения клеток непустая*/
            x = plan.front();
            plan.pop();
            y = plan.front();
            plan.pop();
            this->find_path(n, x, y, lab, visited, path, plan, x_end,
                      y_end); /* продолжаем поиск пути*/
          }
          if (!visited[x_end][y_end]) {
            cout << "N" << endl;
            can_change_position = false;

          } else {
            cout << "Y" << endl;
            can_change_position = true;
            x = x_end;
            y = y_end;
            lab[x][y] = 8;
            //len++;
            while (path[x][y] != 2) { /* восстановление пути*/
              if ((x - 1) >= 0 && (x - 1) < n &&
                  (path[x - 1][y] == path[x][y] - 1)) {
                x = x - 1;
                lab[x][y] = 8;
                //len++;
              } else if ((x + 1) >= 0 && (x + 1) < n &&
                         (path[x + 1][y] == path[x][y] - 1)) {
                x = x + 1;
                lab[x][y] = 8;
                //len++;
              } else if ((y - 1) >= 0 && (y - 1) < n &&
                         (path[x][y - 1] == path[x][y] - 1)) {
                y = y - 1;
                lab[x][y] = 8;
                //len++;
              } else if ((y + 1) >= 0 && (y + 1) < n &&
                         (path[x][y + 1] == path[x][y] - 1)) {
                y = y + 1;
                lab[x][y] = 8;
                //len++;
              }
            }

            for (int i = 0; i < n; i++) {
              for (int j = 0; j < n; j++) {
                visited[i][j] = 0;
                path[i][j] = -1;
              }
            }
            
            for (int i = 0; i < n; i++) {
              for (int j = 0; j < n; j++) {
                cout << lab[i][j] << " ";
                if (lab[i][j] == 8) {
                  lab[i][j] = 1;
                  len++;
                  if (len == step) {
                    tmp_x = i;
                    tmp_y = j;
                  }
                }
                
              }
              cout << endl;
            }
          }
          cout << endl;
          cout << endl;
          cout << "LEN: " << len << endl;
          cout << "STEP: " << step << endl;
        
          if (can_change_position) {
            if (len <= step) {
              own_position = position;
              path_hist.push_back({x_end, y_end});
              return "!OK " + own_position;
            } else {
              path_hist.push_back({tmp_x, tmp_y});
              return "!OK " + std::to_string(tmp_x) + "," + std::to_string(tmp_y);
              
            }
          } else {
            return "Error you can not change position";
          }

          
        } else if(token == "Where?") {
          return std::to_string(path_hist[path_hist.size() - 1].first) + "," +
                 std::to_string(path_hist[path_hist.size() - 1].second); 
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

        
        lab = new int*[n];
        visited = new int*[n];
        path = new int*[n];


        for (int i = 0; i < n; i++) {
          lab[i] = new int[n]; /* массив для хранения лабиринта */
          visited[i] =
              new int[n]; /* массив для хранения информации о посещении клеток*/
          path[i] = new int[n]; /* массив для хранения найденных путей */
          for (int j = 0; j < n; j++) {
            visited[i][j] = 0;
            path[i][j] = -1;
            if (rand() % 2) {
              lab[i][j] = 1;
            } else {
              lab[i][j] = 0;
            }
            
          }
        }
        
    
        lab[0][0] = 1;
        path_hist.push_back({0, 0});
        std::ofstream out;
        out.open("map_" + std::to_string(step) +".txt");
        for (int i = 0; i < n; i++) {
          for (int j = 0; j < n; j++) {
            out << lab[i][j] << " ";
          }
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