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

#include "vec_file.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

#ifdef __MINGW32_MAJOR_VERSION
#include <string.h>
#define strtok_r(s, d, p) strtok(s, d)
#endif

using namespace std;

int vecFile::parse(string fileName, int isInt)
{
  char buf[4095];
  int lineNum = 0;
  int res, tokens = 0;
  unsigned int idx;
  char c;
#ifndef __MINGW32_MAJOR_VERSION
  char *lasts;
#endif
  char *tok;
  FILE *f = fopen(fileName.c_str(), "r");
  if (f == NULL)
  {
    cerr << "vec_file: can not open file:" << fileName << endl;
    return 1;
  }

  while (fgets(buf, 4095, f) != NULL)
  {
    lineNum++;
    if (strlen(buf) > 4095)
    {
      cout << "-E- vector file:" << fileName
           << " line:" << lineNum << " length:" << strlen(buf)
           << " is too long (>1024). Please split into multiple lines."
           << endl;
      return 1;
    }

    // parse the line using sscanf
    tok = strtok_r(buf, " ,", &lasts);
    // get the index
    if ((res = sscanf(tok, "%d%c", &idx, &c)) != 2)
    {
      cout << "-E- vector file:" << fileName
           << " line:" << lineNum << " bad format: should start with '<idx>:'"
           << endl;
      return 1;
    }
    // c must be a ':'
    if (c != ':')
    {
      cout << "-E- vector file:" << fileName
           << " line:" << lineNum << " bad format: did not find ':'"
           << endl;
      return 1;
    }

    // make sure the vector size is adequate
    if (isInt)
    {
      vector<int> tmp;
      if (intData.size() <= idx)
      {
        for (unsigned int i = intData.size(); i <= idx; i++)
          intData.push_back(tmp);
      }
    }
    else
    {
      vector<float> tmp;
      if (floatData.size() <= idx)
      {
        for (unsigned int i = floatData.size(); i <= idx; i++)
          floatData.push_back(tmp);
      }
    }

    // now read next values
    tok = strtok_r(NULL, " ,", &lasts);
    while (tok != NULL)
    {
      tokens++;
      if (isInt)
      {
        int vInt;
        if ((res = sscanf(tok, "%d", &vInt)) == 0)
        {
          cout << "-E- vector file:" << fileName
               << " line:" << lineNum
               << " bad format: expected an INT" << endl;
          return 1;
        }
        if (res > 0)
        {
          intData.at(idx).push_back(vInt);
        }
      }
      else
      {
        float vFloat;
        if ((res = sscanf(tok, "%f", &vFloat)) == 0)
        {
          cout << "-E- vector file:" << fileName
               << " line:" << lineNum
               << " bad format: expected a FLOAT" << endl;
          return 1;
        }
        if (res > 0)
        {
          floatData.at(idx).push_back(vFloat);
        }
      }
      tok = strtok_r(NULL, " ,", &lasts);
    }
  }

  cout << "-I- parsed:" << fileName << " with " << lineNum << " lines "
       << idx << " objects and total of "
       << tokens << " values" << endl;

  fclose(f);
  return 0;
}

vector<int> *
vecFile::getIntVec(unsigned int objIdx)
{
  if (intData.size() <= objIdx)
    return NULL;
  else
    return &intData.at(objIdx);
}

vector<float> *
vecFile::getFloatVec(unsigned int objIdx)
{
  if (floatData.size() <= objIdx)
    return NULL;
  else
    return &floatData.at(objIdx);
}

/////////////////////////////////////////////////////////////////////////////
vecFiles *vecFiles::singleton = 0;

vecFiles::vecFiles()
{
}

vecFiles *vecFiles::get()
{
  if (singleton == 0)
  {
    singleton = new vecFiles;
  }
  return singleton;
}

class vecFile *
vecFiles::parseNewFile(string fileName, int isInt)
{
  vecFile *vf = new vecFile;
  if (vf->parse(fileName, isInt))
  {
    delete vf;
    return NULL;
  }

  files[fileName] = vf;

  return vf;
}

vector<int> *
vecFiles::getIntVec(string fileName, int objIdx)
{
  vecFile *f;
  // try to find the file or parse a new file if unknown
  map_str_p_vf::iterator fI = files.find(fileName);

  if (fI == files.end())
  {
    f = parseNewFile(fileName, 1 /* is int */);
  }
  else
  {
    f = (*fI).second;
  }

  if (f == NULL)
    return NULL;

  return f->getIntVec(objIdx);
}

vector<float> *
vecFiles::getFloatVec(string fileName, int objIdx)
{
  vecFile *f;
  // try to find the file or parse a new file if unknown
  map_str_p_vf::iterator fI = files.find(fileName);

  if (fI == files.end())
  {
    f = parseNewFile(fileName, 0 /* is float */);
  }
  else
  {
    f = (*fI).second;
  }

  if (f == NULL)
    return NULL;

  return f->getFloatVec(objIdx);
}

#ifdef TEST_VEC_FILE

#include "vec_file.h"
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
  vecFiles *v = vecFiles::get();

  for (int obj = 0; obj < 10; obj++)
  {
    vector<float> *flv = v->getFloatVec("test_float.vec", obj);
    if (flv == NULL)
      continue;
    cout << "obj:" << obj << " ";
    for (int i = 0; i < flv->size(); i++)
      cout << (*flv)[i] << " ";
    cout << endl;
  }

  vector<int> *vec2 = v->getIntVec("test_int.vec", 0);
  (*vec2)[2] = 12346;
  for (int obj = 0; obj < 10; obj++)
  {
    vector<int> *vec = v->getIntVec("test_int.vec", obj);
    if (vec == NULL)
      continue;
    cout << "obj:" << obj << " ";
    for (int i = 0; i < vec->size(); i++)
      cout << (*vec)[i] << " ";
    cout << endl;
  }

  return 0;
}
#endif
