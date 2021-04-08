#pragma once


struct ServerConfig {
    std::string dir;
    std::string ip;
    size_t port;
    size_t ndim1;
    size_t ndim2;      
    size_t step;    
    size_t workers;
    bool verbose;
};