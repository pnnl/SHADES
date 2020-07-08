//===------------------------------------------------------------*- C++ -*-===//
//
//                                     SHADES
//
//                            The SHAD Exploration System
//
//===----------------------------------------------------------------------===//
//
// Copyright 2020 Battelle Memorial Institute
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.
//
//===----------------------------------------------------------------------===//
#include <map>
#include <fstream>
#include <iostream>
#include <utility>
#include <string>
#include <iostream>
#include <unistd.h>

#include "zmq.hpp"
#include "shad/runtime/runtime.h"
#include "shad/util/measure.h"
#include "system_ops.h"
#include "graph_ops.h"

namespace shades {
std::map<std::string, graphDescriptor> graphs;
std::map<std::string, std::function<std::string(std::istringstream&)>>
execMap {

  {"numLocalities", __num_localities
  },
  {"hostName", __hostname
  },
  // graph ops
  {"loadGraph", __loadGraph   
  },
  {"listGraphs", __listGraphs
  }  ,
  {"deleteGraph", __deleteGraph
  }  
};

std::string exec(std::string command, std::istringstream& iss) {
  auto cmIt = execMap.find(command);
  std::cout << "\n--------executing-------\n" << std::endl;
  if(cmIt != execMap.end()) {
    return execMap[command](iss);
  }
  else {
    std::cout << "command " << command << " not yet supported\n\n";
    return "command not yet supported";
  }
}
} // samespace shades

namespace shad {
int main(int argc, char **argv) {
    // print hostname
  char hostBuffer[256];
  int len = 256;
  gethostname(hostBuffer, len);
  std::cout << "host: " << hostBuffer << std::endl;
  
  zmq::context_t context (1);
  zmq::socket_t socket (context, ZMQ_REP);
  socket.bind ("tcp://*:5555");
  std::cout << "*** SHADES server running ***" << std::endl;
  
  std::string command_op_type = "";
  while (command_op_type != "exit") {
    zmq::message_t request;

    //  Wait for next request from client
    socket.recv (&request);
    char* mystr = reinterpret_cast<char*>(request.data());
    std::string command(mystr, request.size());
    std::cout << "Received command string: " << command << std::endl;
    std::istringstream isscommand(command);
    isscommand >> command_op_type;
    
    std::cout << "----parsed command " << command_op_type << std::endl;
    
    auto res = shades::exec(command_op_type, isscommand);
    zmq::message_t reply (res.length()+1);
    memcpy ((void *) reply.data (), res.c_str(), res.length()+1);
    socket.send (reply);
  }
  std::cout << "\n\n----DISCONNECTING----\n\n" << std::endl;
  socket.close();
  return 0;
}

}  // namespace shad
