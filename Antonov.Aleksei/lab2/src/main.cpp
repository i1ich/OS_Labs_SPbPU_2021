//
// Created by aleksei on 28.11.2021.
//
#include <iostream>
#include "Host.h"

using namespace std;
int main(int argc, char ** argv)
{
  if(argc != 2)
  {
    cout << "Wrong num of arguments" << endl;
    return EXIT_FAILURE;
  }

  Host host;
  if (host.connectClients(stoi(argv[1]))) {
    host.start();
    return 0;
  }
  return 1;
}