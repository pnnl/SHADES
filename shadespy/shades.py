#//===------------------------------------------------------------*- C++ -*-===//
#//
#//                                     SHADES
#//
#//                            The SHAD Exploration System
#//
#//===----------------------------------------------------------------------===//
#//
#// Copyright 2020 Battelle Memorial Institute
#//
#// Licensed under the Apache License, Version 2.0 (the "License"); you may not
#// use this file except in compliance with the License. You may obtain a copy
#// of the License at
#//
#//     http://www.apache.org/licenses/LICENSE-2.0
#//
#// Unless required by applicable law or agreed to in writing, software
#// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
#// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
#// License for the specific language governing permissions and limitations
#// under the License.
#//
#//===----------------------------------------------------------------------===//

import zmq
context = zmq.Context()
socket = context.socket(zmq.REQ)

def connect_to_server(hostname, port):
  socket.connect("tcp://" + hostname + ":" + port)
  socket.send_string("hostName")
  message = socket.recv_string()
  print("Connected to SHADes server", message)
  return

def num_localities():
  socket.send_string("numLocalities")
  message = socket.recv_string()
  print(message)
  return

def shutdown():
  socket.send_string("exit")
  return

def load_graph(gname, fname):
  socket.send_string("loadGraph " + gname + " " + fname)
  message = socket.recv_string()
  print(message)
  return

def delete_graph(gname):
  socket.send_string("deleteGraph " + gname)
  message = socket.recv_string()
  print(message)
  return

def list_graphs():
  socket.send_string("listGraphs")
  message = socket.recv_string()
  print(message.format())
  return
