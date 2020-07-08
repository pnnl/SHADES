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

#ifndef INCLUDE_GRAPH_OPS_H_
#define INCLUDE_GRAPH_OPS_H_

#include <atomic>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <utility>

#include "shad/extensions/graph_library/edge_index.h"
#include "shad/runtime/runtime.h"
#include "shad/util/measure.h"

namespace shades {

struct graphDescriptor {
  uint64_t oid;
  uint64_t numV;
  uint64_t numE;
  std::string fname;
  std::string to_string() {
    return std::to_string(oid) + " " + std::to_string(numV) + " "
           + std::to_string(numE) + " " + fname;
  }
};
extern std::map<std::string, graphDescriptor> graphs;


std::string __listGraphs(std::istringstream&);
std::string __deleteGraph(std::istringstream& iss);
std::string __loadGraph(std::istringstream& iss);
// std::string loadGraph(std::string name, std::string filename);

// COMMANDS IMPLEMENTATION
std::string __listGraphs(std::istringstream&) {
  if (graphs.size() == 0)
    return "No graph currently created or loaded";
  std::string ret = "Name OID NumVertices NumEdges InputFile";
  for (auto el : graphs) {
    ret += "\n" + el.first + " " + el.second.to_string();
  }
  return ret;
}

std::string __deleteGraph(std::istringstream& iss) {
  std::string name;
  iss >> name;
  auto gdesc = graphs.find(name);
  if (gdesc == graphs.end())
    return "Graph " + name + " has not been created or loaded, or has been deleted";
  shad::EdgeIndex<size_t, size_t>::ObjectID oid(gdesc->second.oid);
  shad::EdgeIndex<size_t, size_t>::Destroy(oid);
  graphs.erase(gdesc);
  return "Graph " + name + " has been deleted";;
}

// The GraphReader expects an input file in METIS dump format
shad::EdgeIndex<size_t, size_t>::ObjectID GraphReader(std::ifstream &GFS) {
  std::string line;
  unsigned long EdgeNumber, VertexNumber;

  std::getline(GFS, line);

  std::istringstream headlineStream(line);

  headlineStream >> VertexNumber >> EdgeNumber;
  EdgeNumber <<= 1;

  auto eiGraph = shad::EdgeIndex<size_t, size_t>::Create(VertexNumber);
  shad::rt::Handle handle;

  for (size_t i = 0L; i < VertexNumber; i++) {
    size_t destination;

    std::getline(GFS, line);
    std::istringstream lineStream(line);
    std::vector<size_t> edges;
    while (!lineStream.eof()) {
      lineStream >> destination;
      destination--;
      if (destination >= i) continue;
      edges.push_back(destination);
    }
    eiGraph->AsyncInsertEdgeList(handle, i, edges.data(), edges.size());
  }
  shad::rt::waitForCompletion(handle);
  return eiGraph->GetGlobalID();
}

std::string __loadGraph(std::istringstream& iss) {
  std::string name;
  std::string filename;
  iss >> name;
  iss >> filename;
  std::cout << "\nLoadingGraph(" << name << ");\n";
  shad::EdgeIndex<size_t, size_t>::ObjectID OID(-1);
  auto loadingTime = shad::measure<std::chrono::seconds>::duration([&]() {
    // The GraphReader expects an input file in METIS dump format
    std::ifstream inputFile;
    inputFile.open(filename.c_str(), std::ifstream::in);
    OID = GraphReader(inputFile);
  });
  auto eiPtr = shad::EdgeIndex<size_t, size_t>::GetPtr(OID);
  graphs[name] = {(uint64_t)OID, eiPtr->Size(), eiPtr->NumEdges(), filename};
  return   "Graph " + name + " loaded in " + std::to_string(loadingTime.count())
            + " seconds";
}


}  // namespace shades
#endif  // INCLUDE_GRAPH_OPS_H_
