/****************************************************************************
  FileName     [ memTest.h ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test classes ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#ifndef MEM_TEST_H
#define MEM_TEST_H

#include <iostream>
#include <vector>
#include <cassert>
#include "memMgr.h"

using namespace std;

//----------------------------------------------------------------------
//    Classes for memory test objects
//----------------------------------------------------------------------
// Private class, only friend to class MemTest
//
class MemTestObj
{
friend class MemTest;
#ifdef MEM_MGR_H
   USE_MEM_MGR(MemTestObj);
#endif // MEM_MGR_H

public:
   MemTestObj() {}
   ~MemTestObj() { /* cout << "destruct " << endl; */  }

private:
   // sizeof(memTestObj) = 41 --> 44
   int   _dataI[10];
   char  _dataC;
};

class MemTest
{
public:
   MemTest() { _objList.reserve(1024); _arrList.reserve(1024); }
   ~MemTest() { }

   void reset(size_t b = 0) {
      _objList.clear(); _arrList.clear();
      #ifdef MEM_MGR_H
      MemTestObj::memReset(b);
      #endif // MEM_MGR_H
   }
   size_t getObjListSize() const { return _objList.size(); }
   size_t getArrListSize() const { return _arrList.size(); }

   // Allocate "n" number of MemTestObj elements
   void newObjs(size_t n) {
      // TODO
      //cout << "_objList.sizeof = " << _objList.size() << endl;
      for(size_t i=0; i < n; ++i ){
        MemTestObj* testObj = new MemTestObj;
        _objList.push_back(testObj);
        //_objList.push_back(new MemTestObj);
        //cout << "_objList[i].sizeof  = " << sizeof(_objList[i]) << endl;
      }
      //cout << "_objList.sizeof after push = " << _objList.size() << endl;
   }
   // Allocate "n" number of MemTestObj arrays with size "s"
   void newArrs(size_t n, size_t s) {
      // TODO
      for(size_t i=0; i < n; ++i ){
        //try{
            MemTestObj* testObj = new MemTestObj[s];
            _arrList.push_back(testObj);
        //} catch(bad_alloc){
           // cerr << "newArrs err" <<  endl;
        //}
      }
      
   }
   // Delete the object with position idx in _objList[]
   void deleteObj(size_t idx) {
      assert(idx < _objList.size());
      // TODO
      if(_objList[idx] != NULL) {
      //  cout << "delete _objList["  << idx <<"] _objList = " << _objList[idx] << endl; 
        delete _objList[idx];
        _objList[idx] = NULL;
      }
   }
   // Delete the array with position idx in _arrList[]
   void deleteArr(size_t idx) {
      assert(idx < _arrList.size());
      // TODO
      if(_arrList[idx] != 0) {
       // cout << "_deleteing" << endl;
        delete [] _arrList[idx];
       // cout << "_delete done" << endl;
        _arrList[idx] = 0;
      }
   }

   void print() const {
      #ifdef MEM_MGR_H
      MemTestObj::memPrint();
      #endif // MEM_MGR_H
      cout << "=========================================" << endl
           << "=             class MemTest             =" << endl
           << "=========================================" << endl
           << "Object list ---" << endl;
      size_t i = 0;
      while (i < _objList.size()) {
         cout << (_objList[i]? 'o' : 'x');
         //cout << _objList[i] << endl;
         if (++i % 50 == 0) cout << endl;
      }
      cout << endl << "Array list ---" << endl;
      i = 0;
      while (i < _arrList.size()) {
         cout << (_arrList[i]? 'o' : 'x');
         if (++i % 50 == 0) cout << endl;
      }
      cout << endl;
   }

private:
   vector<MemTestObj*>   _objList;
   vector<MemTestObj*>   _arrList;
};

#endif // MEM_TEST_H
