/****************************************************************************
  FileName     [ myHashMap.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashMap and Cache ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2009-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_MAP_H
#define MY_HASH_MAP_H

#include <vector>
#include <bitset> 
//#define debug_hash
using namespace std;

// TODO: Implement your own HashMap and Cache classes.

//-----------------------
// Define HashMap classes
//-----------------------
// To use HashMap ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class HashKey
// {
// public:
//    HashKey() {}
// 
//    size_t operator() () const { return 0; }
// 
//    bool operator == (const HashKey& k) const { return true; }
// 
// private:
// };
//
class PatternKey
{
public:
    PatternKey(unsigned patternValue): _pKeyValue(patternValue) {}
    size_t operator() () const {
       if(_pKeyValue < ~_pKeyValue)
            return _pKeyValue; 
       else
            return ~_pKeyValue;
    }
    bool operator == (const PatternKey& k) const {
       #ifdef hash_debug 
           cout << "   _pKeyValue =  " << _pKeyValue << ", k._pKeyValue = " << k._pKeyValue << endl;
       #endif
       return (_pKeyValue == k._pKeyValue ) || (~_pKeyValue == k._pKeyValue);
    }

private:
    unsigned _pKeyValue;
  
};

class HashKey
{
public:
   HashKey(size_t inID1,size_t inID2,string type,int rhs1, int rhs2):
       _fanin1(inID1),_fanin2(inID2), _type(type), _rhs1(rhs1) ,_rhs2(rhs2)  {}
   ~HashKey() {} 
   size_t operator() () const {
       return (_rhs1 ^ _fanin1) + (_rhs2 ^ _fanin2) ; 
   }

   bool operator == (const HashKey& k) const {
       bool isTypeEqual = (_type == k._type)? true:false;
       bool isFaninsEqual = 
           ((_fanin1 == k._fanin1) && (_fanin2 == k._fanin2) && (_rhs1 == k._rhs1) && (_rhs2 == k._rhs2)) || 
           ((_fanin2 == k._fanin1) && (_fanin1 == k._fanin2) && (_rhs2 == k._rhs1) && (_rhs1 == k._rhs2));
       return  isTypeEqual && isFaninsEqual; 
   }

private:
   size_t _fanin1;
   size_t _fanin2;
   int _rhs1;
   int _rhs2;
   string _type;
};

template <class HashKey, class HashData>
class HashMap
{
typedef pair<HashKey, HashData> HashNode;

public:
   HashMap() : _numBuckets(0), _buckets(0) {}
   HashMap(size_t b) : _numBuckets(0), _buckets(0) { init(b); }
   ~HashMap() { reset(); }

   // [Optional] TODO: implement the HashMap<HashKey, HashData>::iterator
   // o An iterator should be able to go through all the valid HashNodes
   //   in the HashMap
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   // (_bId, _bnId) range from (0, 0) to (_numBuckets, 0)
   //
   class iterator
   {
      friend class HashMap<HashKey, HashData>;

   public:

   private:
   };

   void init(size_t b) {
      reset(); _numBuckets = b; _buckets = new vector<HashNode>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<HashNode>& operator [] (size_t i) { return _buckets[i]; }
   const vector<HashNode>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //u
   // Point to the first valid data
   iterator begin() const { iterator(); }
   // Pass the end
   iterator end() const { iterator(); }
   // return true if no valid data
   bool empty() const { 
         for(int i=0 ; i <  numBuckets() ; i++) {
             if( !_buckets[i].empty()){
                 return false;
             }
        } 
        return true; 
   }
   // number of valid data
   size_t size() const { 
        size_t s = 0; 
        for(int i=0 ; i <  numBuckets() ; i++) {
             if( !_buckets[i].empty()){
                s += _buckets[i].size();      
             }   
        }   
        return s;  
   }
   bool check(const HashKey& k,HashData& d) const {
        size_t bucketIdx = bucketNum(k);
        size_t bucketSize = _buckets[bucketIdx].size();
         //bitset<sizeof(k()) * 4> s(k()); // bitset for debug use 
         //cout << " check  bucketIdx =  " <<  bucketIdx <<", sizeof(k()) = " << sizeof(k()) << ", patternValue = " << k()  << endl; 
            if(!_buckets[bucketIdx].empty()){
                for(int i = 0; i < bucketSize ; i++) {
               //         bitset<sizeof(k()) * 4> s(k()); // bitset for debug use 
               //         cout << "  bucketIdx =  " <<  bucketIdx << ", i = " << i << ", sizeof(k()) = " << sizeof(k()) << ", patternValue = " << s  << endl; 
                    #ifdef debug_hash
                        bitset<sizeof(k()) * 4> s(k()); // bitset for debug use 
                        cout << "  bucketIdx =  " <<  bucketIdx << ", i = " << i << ", sizeof(k()) = " << sizeof(k()) << ", patternValue = " << s  << endl; 
                        _buckets[bucketIdx][i].first() << ", k = " << k()  << endl; 
                    #endif
                    if(_buckets[bucketIdx][i].first == k){
                        d = _buckets[bucketIdx][i].second;
                        return true;
                    }
                }
            }
        //}
        return false;
   } 
   bool insert(const HashKey& k, HashData& d) {
        size_t bucketIdx = bucketNum(k);
        //size_t bucketSize = _buckets[bucketIdx].size();
         if(check(k,d)) {
                    return false;
         }
         #ifdef debug_hash
           bitset<sizeof(k()) * 4> s(k()); // bitset for debug use 
           cout << "  bucketIdx =  " <<  bucketIdx <<", sizeof(k()) = " << sizeof(k()) << ", patternValue = " << s  << endl; 
           //cout << "  bucketIdx =  " <<  bucketIdx <<", sizeof(k()) = " << sizeof(k()) << ", patternValue = " << k()  << endl; 
         #endif
         HashNode myNode(k,d); 
         _buckets[bucketIdx].push_back(myNode);
         //++bucketSize;
         return true; 
   }   

   void myPrintAll() const {
         for(int i=0 ; i <  numBuckets() ; i++) {
             if( !_buckets[i].empty()){
                 for(typename vector<HashNode>::iterator it =  _buckets[i].begin(); it !=  _buckets[i].end() ; it++ ) {
//                     cout << "Just print" << endl;
                     //cout << (*it) << endl;
                     cout << "buckets["<< i << "] , have Gate ID = " <<  (*it).second->getID() << endl;
                 }
             }

         }
    }


private:
   // Do not add any extra data member
   size_t                   _numBuckets;
   vector<HashNode>*        _buckets;

   size_t bucketNum(const HashKey& k) const {
      return (k() % _numBuckets); }

};


//---------------------
// Define Cache classes
//---------------------
// To use Cache ADT, you should define your own HashKey class.
// It should at least overload the "()" and "==" operators.
//
// class CacheKey
// {
// public:
//    CacheKey() {}
//    
//    size_t operator() () const { return 0; }
//   
//    bool operator == (const CacheKey&) const { return true; }
//       
// private:
// }; 
// 
template <class CacheKey, class CacheData>
class Cache
{
typedef pair<CacheKey, CacheData> CacheNode;

public:
   Cache() : _size(0), _cache(0) {}
   Cache(size_t s) : _size(0), _cache(0) { init(s); }
   ~Cache() { reset(); }

   // NO NEED to implement Cache::iterator class

   // TODO: implement these functions
   //
   // Initialize _cache with size s
   void init(size_t s) { }
   void reset() { }

   size_t size() const { return _size; }

   CacheNode& operator [] (size_t i) { return _cache[i]; }
   const CacheNode& operator [](size_t i) const { return _cache[i]; }

   // return false if cache miss
   bool read(const CacheKey& k, CacheData& d) const { return false; }
   // If k is already in the Cache, overwrite the CacheData
   void write(const CacheKey& k, const CacheData& d) { }

private:
   // Do not add any extra data member
   size_t         _size;
   CacheNode*     _cache;
};


#endif // MY_HASH_H
