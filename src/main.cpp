#include "server.hpp"
#include <iostream>

int main() {
    Server server("/home/exigent07/CPP/web-server/config/conf.json");
    server.startListening();
}