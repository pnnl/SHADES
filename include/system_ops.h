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


#include <atomic>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

#include "shad/runtime/runtime.h"
#include "shad/util/measure.h"

std::string __hostname(std::istringstream&) {
  char hostBuffer[256];
  gethostname(hostBuffer, 256);
  return std::string(hostBuffer);
}

std::string __exit(std::istringstream&) {
  return "disconnected";
}

std::string __num_localities(std::istringstream&) {
  return std::to_string(shad::rt::numLocalities());
}
