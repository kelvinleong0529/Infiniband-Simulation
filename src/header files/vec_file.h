///////////////////////////////////////////////////////////////////////////
//
//         InfiniBand FLIT (Credit) Level OMNet++ Simulation Model
//
// Copyright (c) 2004-2013 Mellanox Technologies, Ltd. All rights reserved.
// This software is available to you under the terms of the GNU
// General Public License (GPL) Version 2, available from the file
// COPYING in the main directory of this source tree.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
// BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////
//
// A utility class parsing a file of vector of values and fill in a given 
// corresponding STL vector.
//
// Usage: 
// 1. Assign every vector of data in a file a specific unique index
// 2. In each usage module define the vectors service as reference:
//    #include <vec_file.h>
//    vecFiles vecMgr = vecFiles::get();
// 3. Get the vector index by using some standard parameter
//    int vec_idx = par("vec_idx")
// 4. Get the vector file by using constant or standard param:
//    string vec_file_name = par("vec_file");
// 5. Define the vector by ref:
//    vector<int> *v = vecMgr.getIntVec(vec_file_name,vec_idx);
// NOTE: the reference value must be set at declaration
//

#ifndef __VEC_FILE__
#define __VEC_FILE__

#include <iostream>
#include <map>
#include <vector>

class vecFile 
{
 private:
  std::vector<std::vector<int> >   intData;
  std::vector<std::vector<float> > floatData;
  int parse(std::string fileName, int asInt = 1);
 public:
  std::vector<int>   *getIntVec(  unsigned int objIdx);
  std::vector<float> *getFloatVec(unsigned int objIdx);
  friend class vecFiles;
};

typedef std::map<std::string, class vecFile*, std::less<std::string > > map_str_p_vf;

class vecFiles
{
 private:
  static vecFiles * singleton;

  map_str_p_vf files;
  vecFiles();
  class vecFile *parseNewFile(std::string fileName, int isInt);

 public:
  std::vector<int>   *getIntVec  (std::string fileName, int objIdx);
  std::vector<float> *getFloatVec(std::string fileName, int objIdx);
  static vecFiles *get();
  friend class vecFile;
};

#endif /* __VEC_FILE__ */
