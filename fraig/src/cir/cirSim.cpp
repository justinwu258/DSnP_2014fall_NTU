/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-2014 LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"
#include <bitset>
using namespace std;
#define myI  10000
#define parallelizeBits 32
#define hashBucketSize 32
#define mySeed INT_MAX
#define maxTry 50
//#define debug_fileSim
//#define debug_FEC

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
int gSimArray[parallelizeBits][myI]; // create array to store PIs

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
    int i, j,patternCount = 0 , sameGrpNumCount = 0, failCount = 0 , maxFails = (M+O+1)/8 ;
    int fecGrpNums = 0;
    bool doInitFEC = false;
    unsigned patternValue = 0, tmpPatternValue; 
   
    while(1){
        // random set patternValue for PI
        CirMgr::setRandomPatternValue();
        CirMgr::outputRandomToSimLog();
        if(!doInitFEC) { CirMgr::initFEC();  doInitFEC = true; fecGrpNums = _fecGrps.size(); }
        CirMgr::checkFEC();
        if(_fecGrps.size() == fecGrpNums){ ++sameGrpNumCount; } 
        else                             { fecGrpNums = _fecGrps.size(); }
       
        if(sameGrpNumCount == maxTry) { 
            cout << "grp no change "<< maxTry <<" times" << endl; 
            break; 
        }
        else if(failCount >= maxFails) { 
            cout << "MAX_FAILS = " << maxFails<<endl; 
            break; 
        }
        cout << endl << endl;
        cout << "\r" << "Total #FEC Group = " << _fecGrps.size();
        
        ++failCount;
        ++patternCount;
    }
    _isSimulated = true;
    cout << "\r" << patternCount*parallelizeBits << "  patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile){

    string line;
    int lineCount = 0, tmpLineCount, i,j,k;
    int **simArray; // create array to store PIs , I: PI's nums
    int simArrayTail = parallelizeBits - 1;
    bool isReadError = false, doInitFEC = false;
    unsigned patternValue = 0; 
    
    //initialize array
//    initSimArray(simArray);
    simArray = new int *[parallelizeBits];
    for (j = 0; j < parallelizeBits; j++){
        simArray[j] = new int[I];
    }
    for (j = 0; j < parallelizeBits; ++j){  
        for(i = 0; i < I; ++i){
            *(*(simArray+j)+i) = 0;
        }    
    }    

    while(getline(patternFile,line)){  // read all data to 2D array
        //cout << "line length = " << line.length() << endl;
        for(i = 0; i < I; ++i) {
            if( (line[i] != '0') && (line[i] != '1')  )
            {
                isReadError = true;
                cout << "Line = "<< lineCount << ", line[" << i  << "] = "<< line[i] << endl;
                break;
            }
            simArray[simArrayTail-lineCount%parallelizeBits][i] = line[i] - '0';
        }
        if(isReadError) break;
        lineCount++;
        if(lineCount%parallelizeBits == 0) {
            //setPattern(simArray);
            #ifdef debug_fileSim
                CirMgr::printSimArray(simArray);
            #endif
            CirMgr::setPatternValue(simArray);
            CirMgr::outputToSimLog(simArray,simArrayTail, simArrayTail);
            if(!doInitFEC) { CirMgr::initFEC();  doInitFEC = true; }
            CirMgr::checkFEC();
            cout << "\r" << "Total #FEC Group = " << _fecGrps.size();
        }
    }
    #ifdef debug_fileSim
        cout << endl << endl;
    #endif 
    if(!isReadError){
        if(lineCount%parallelizeBits != 0 ){ // last bitwise , maybe not have parallelizeBits bit unsigned value
            for(i = 0; i < I; ++i ){  // not assigned postion , put 0 
                tmpLineCount = lineCount;
                for(j = 0; j < parallelizeBits - lineCount%parallelizeBits; ++j) {
                    simArray[simArrayTail-tmpLineCount%parallelizeBits][i] = 0;
                    ++tmpLineCount;
                }
            }
            #ifdef debug_fileSim
               CirMgr::printSimArray(simArray);
            #endif
            CirMgr::setPatternValue(simArray);
            CirMgr::outputToSimLog(simArray,simArrayTail, lineCount%parallelizeBits-1); //actual read's lineNum
            if(!doInitFEC) { CirMgr::initFEC();  doInitFEC = true; }
            CirMgr::checkFEC();
            cout << "\r" << "Total #FEC Group = " << _fecGrps.size();
        }
    } else {
        cout << "Something error when get input" << endl;
    }
    
    //cout << "\r" << "Total #FEC Group = " << _fecGrps.size();
    _isSimulated = true;
    cout << "\r" << lineCount << "  patterns simulated." << endl;
    for (j = 0; j < parallelizeBits; j++){
        delete simArray[j];
    }
    delete simArray;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::setRandomPatternValue(){
    int i, j, tmpPatternValue = 0;
    unsigned patternValue = 0, rhs1_value , rhs2_value,poIn_value; 
    for(i = 0; i < I; ++i) {
        patternValue = (unsigned) rnGen(mySeed);
        if(patternValue%7 == 0)
            _totalList[_piList[i]->getID()]->_patternValue = patternValue;
        else
            _totalList[_piList[i]->getID()]->_patternValue = patternValue << 1;
        tmpPatternValue = _totalList[_piList[i]->getID()]->_patternValue;
        #ifdef debug_fileSim
            bitset<sizeof(tmpPatternValue) * 8> s(tmpPatternValue); // bitset for debug use
            cout << "   sizeof(patternValue) = " << sizeof(tmpPatternValue);
            cout << ", i = " << i << ",  patternValue = " << s << endl;     
        #endif 
    }
    for(vector<CirGate*>::const_iterator it = _dfsList.begin(); it != _dfsList.end(); it++){
         if(*it != 0){
             if((*it)->_type == "AIG"){
                 if(((CirAIGGate*)(*it))->_rhs1_invert) rhs1_value = ~(*it)->_faninList[0]->_patternValue;
                 else                                   rhs1_value = (*it)->_faninList[0]->_patternValue;
                 if(((CirAIGGate*)(*it))->_rhs2_invert) rhs2_value = ~(*it)->_faninList[1]->_patternValue;
                 else                                   rhs2_value = (*it)->_faninList[1]->_patternValue;
                 (*it)->_patternValue = rhs1_value & rhs2_value;
             } else if((*it)->_type == "PO"){
                 if(((CirPOGate*)(*it))->_isInvert) poIn_value = ~(*it)->_faninList[0]->_patternValue;
                 else                               poIn_value = (*it)->_faninList[0]->_patternValue;
                 (*it)->_patternValue = poIn_value;
             }
         }
    } 
}
void
CirMgr::setPatternValue(int **simArray)
{
    int i, j;
    unsigned patternValue = 0, rhs1_value , rhs2_value,poIn_value; 
    #ifdef debug_fileSim
       for(vector<CirPIGate*>::const_iterator it = _piList.begin(); it != _piList.end(); it++){
           cout << " PI = " <<  (*it)->getID() << endl;
       }
    #endif
    for(i = 0; i < I; ++i) {
        patternValue = 0;
        for(j = 0; j < parallelizeBits; ++j){
            patternValue += simArray[j][i];
            if(j != parallelizeBits - 1)
                patternValue = patternValue << 1;
            
        }
        _totalList[_piList[i]->getID()]->_patternValue = patternValue;
        #ifdef debug_fileSim
            bitset<sizeof(patternValue) * 8> s(patternValue); // bitset for debug use
            //cout << "   sizeof(patternValue) = " << sizeof(patternValue);
            //cout << "i = " << i << ",  patternValue = " << s << endl;     
        #endif 
    }
    #ifdef debug_fileSim
        cout << endl;
    #endif 

    //set _dfsList patternValue
   for(vector<CirGate*>::const_iterator it = _dfsList.begin(); it != _dfsList.end(); it++){
        if(*it != 0){
            if((*it)->_type == "AIG"){
                if(((CirAIGGate*)(*it))->_rhs1_invert) rhs1_value = ~(*it)->_faninList[0]->_patternValue;
                else                                   rhs1_value = (*it)->_faninList[0]->_patternValue;
                if(((CirAIGGate*)(*it))->_rhs2_invert) rhs2_value = ~(*it)->_faninList[1]->_patternValue;
                else                                   rhs2_value = (*it)->_faninList[1]->_patternValue;
                (*it)->_patternValue = rhs1_value & rhs2_value;
                    //((*it)->_faninList[0]->_patternValue ^ ((CirAIGGate*)(*it))->_rhs1_invert) & 
                    //((*it)->_faninList[1]->_patternValue ^ ((CirAIGGate*)(*it))->_rhs2_invert) ;
           // cout << ", (*it)->_patternValue = " << (*it)->_patternValue << endl ;
            #ifdef debug_fileSim
                bitset<sizeof((*it)->_patternValue) * 8> s((*it)->_patternValue); // bitset for debug use
                cout << "   sizeof(patternValue) = " << sizeof((*it)->_patternValue);
                cout << "debug ID = "<< (*it)->getID() << " , gateType = "  <<  (*it)->_type <<  ", *it= "<< (*it);
                cout << ", _isVisited = " << (*it)->_isVisited << ", _pValue = " << s  <<endl;          
                cout << "  ---- fanin ----" << endl;
                for(vector<CirGate*>::const_iterator itG = (*it)->_faninList.begin(); itG != (*it)->_faninList.end(); itG++) {
                   cout << "  *itG = " <<  (*itG) << ",  *itG->ID = " <<  (*itG)->getID() << endl;
                }
            #endif
            } else if((*it)->_type == "PO"){
                if(((CirPOGate*)(*it))->_isInvert) poIn_value = ~(*it)->_faninList[0]->_patternValue;
                else                               poIn_value = (*it)->_faninList[0]->_patternValue;
                (*it)->_patternValue = poIn_value;
                    //((*it)->_faninList[0]->_patternValue ^ ((CirPOGate*)(*it))->_isInvert);
            }
        }
   } 

}

void
CirMgr::initFEC()
{
    //HashMap<PatternKey,CirGate*> fecHash(getHashSize(_dfsList.size()));
    HashMap<PatternKey,IdList*> fecHash(getHashSize(hashBucketSize));
    IdList* constIdList = new IdList();
    PatternKey pConstKey(0);
    constIdList->push_back(_totalList[0]->getID());
    fecHash.insert((pConstKey),constIdList);
    
    int dfs_i = 0;
    for(vector<CirGate*>::const_iterator it = _dfsList.begin(); it != _dfsList.end(); it++){
         if(*it != 0){
             if((*it)->_type == "AIG"){
                 PatternKey pKey((*it)->_patternValue);
                 //CirGate* d = (*it);
                 IdList* tmpIdList;
                 #ifdef debug_FEC
                     cout << "checks gate ID = " << (*it)->getID() << endl;
                 #endif
                 if(fecHash.check((*it)->_patternValue,tmpIdList)){
                     tmpIdList->push_back(_dfsList[dfs_i]->getID()); 
            //             cout << "fecHash patternValue exist , same gate ID = " << _dfsList[dfs_i]->getID();
            //             bitset<sizeof((*it)->_patternValue) * 8> s((*it)->_patternValue); // bitset for debug use
            //             cout <<  ", pKey = " << s << endl;
            //             cout << "IdList.size() = " << (*tmpIdList).size() << endl; 
                     #ifdef debug_FEC
                         cout << "fecHash patternValue exist , same gate ID = " << _dfsList[dfs_i]->getID();
                         bitset<sizeof((*it)->_patternValue) * 8> s((*it)->_patternValue); // bitset for debug use
                         cout <<  ", pKey = " << s << endl;
                         cout << "IdList.size() = " << (*tmpIdList).size() << endl; 
                     #endif
                   //  if(tmpIdList >=2){   //same ID add to _fecGrps
                   //     _fecGrps.push_back(tmpIdList);
                   //  }
                 }
                 else {
                     IdList* myIdList = new IdList();
                   //      bitset<sizeof((*it)->_patternValue) * 8> s((*it)->_patternValue); // bitset for debug use
                   //      cout << "insert gate to fecHash, insert ID = " << _dfsList[dfs_i]->getID() << ", pKey = " << s
                   //           << endl;
                        
                     #ifdef debug_FEC
                         bitset<sizeof((*it)->_patternValue) * 8> s((*it)->_patternValue); // bitset for debug use
                         cout << "insert gate to fecHash, insert ID = " << _dfsList[dfs_i]->getID() << ", pKey = " << s
                              << endl;
                     #endif
                     myIdList->push_back(_dfsList[dfs_i]->getID());
                     fecHash.insert((pKey),myIdList);
                 }
                 //cout << " 2. IdList.size() = " << (*tmpIdList).size() << endl; 
             }   
         }
         (*it)->_dfsIdx = dfs_i;  // record occurence order in DFS list
         ++dfs_i;
    }
    #ifdef debug_FEC
        //fecHash.myPrintAll();  
    #endif

    //add IdList to _fecGrps 
    for(int i = 0; i < fecHash.numBuckets(); ++i){
        if( !fecHash[i].empty()){
            #ifdef debug_FEC
            cout << "  i = " << i <<  ", fecHash[i].size() = " <<  fecHash[i].size() << endl;
            #endif 
            for(int j = 0; j < fecHash[i].size(); ++j) {
            #ifdef debug_FEC
                cout << "  j = " << i <<  ", fecHash[i][j].second->size() = " <<  fecHash[i][j].second->size() << endl;
            #endif
                if(fecHash[i][j].second->size() > 1) {
                    _fecGrps.push_back(fecHash[i][j].second);
                } else {
                    _totalList[(*fecHash[i][j].second)[0]]->_fecGrpsIdx = -1;
                    _totalList[(*fecHash[i][j].second)[0]]->_fecInv = false;
                    //cout << "reset grp for gateID = " <<  (*fecHash[i][j].second)[0] << endl;
                    delete fecHash[i][j].second;
                }
                //cout << fecHash[i][j]; 
            }
        } 
    }
}

void
CirMgr::checkFEC()
{
    vector<IdList*> newGrps;
    for(int i = 0; i < _fecGrps.size(); ++i) {
        if(_fecGrps[i] != 0) {
            HashMap<PatternKey,IdList*> fecHash(getHashSize(hashBucketSize));
            for(int j = 0; j < _fecGrps[i]->size(); ++j) {  // go through IdList in _fecGrps
                CirGate* d = _totalList[(*_fecGrps[i])[j]];
                PatternKey pKey(d->_patternValue);
                IdList* tmpIdList;
                //_totalList[(*_fecGrps[i])[j]];
                if(fecHash.check(d->_patternValue,tmpIdList)){
                    tmpIdList->push_back(d->getID()); 
                } else {
                    IdList* myIdList = new IdList();
                    myIdList->push_back(d->getID());
                    fecHash.insert((pKey),myIdList);
                }
            }
            for(int i = 0; i < fecHash.numBuckets(); ++i){
                if( !fecHash[i].empty()){
                    for(int j = 0; j < fecHash[i].size(); ++j) {
                        if(fecHash[i][j].second->size() > 1) {
                            newGrps.push_back(fecHash[i][j].second);
                        } else {
                            _totalList[(*fecHash[i][j].second)[0]]->_fecGrpsIdx = -1;
                            _totalList[(*fecHash[i][j].second)[0]]->_fecInv = false;
                            //cout << "reset grp for gateID = " <<  (*fecHash[i][j].second)[0] << endl;
                            delete fecHash[i][j].second;
                        }
                        //cout << fecHash[i][j]; 
                    }
                    
                }
            }
            #ifdef debug_FEC
                cout << " Group " << i << ", Grp size = " << _fecGrps[i]->size() <<", ID List is : ";
                for(int j = 0; j < _fecGrps[i]->size(); ++j) {
                    cout << (*_fecGrps[i])[j] << ", " ;
                }
                cout << endl;
            #endif
        }
    }
    _fecGrps.swap(newGrps);
    for(int i = 0; i < _fecGrps.size(); ++i) {
        //cout << " Group " << i << ", Grp size = " << _fecGrps[i]->size() <<", ID List is : ";
        sort( (*_fecGrps[i]).begin(),(*_fecGrps[i]).end());
        for(int j = 0; j < _fecGrps[i]->size(); ++j) {
            //cout << (*_fecGrps[i])[j] << ", " ;
            if(_totalList[(*_fecGrps[i])[0]]->_patternValue == _totalList[(*_fecGrps[i])[j]]->_patternValue)
                _totalList[(*_fecGrps[i])[j]]->_fecInv = false;
            else
                _totalList[(*_fecGrps[i])[j]]->_fecInv = true;
            _totalList[(*_fecGrps[i])[j]]->_fecGrpsIdx = i;
        }
        //cout << endl;
    }
    for(int i = 0; i < newGrps.size(); ++i) {
        delete newGrps[i];
    }
}

void
CirMgr::outputRandomToSimLog(){
    int i,j;
    unsigned pValue;
    
    if(_simLog != 0) {
        for(j = 0; j < parallelizeBits; ++j){
            for(i = 0; i < I; ++i) {
                pValue = _totalList[_piList[i]->getID()]->_patternValue;
                bitset<sizeof(pValue) * 8> s(pValue); // bitset for debug use
                *_simLog << s[j];
            }
            *_simLog << " ";
            for(i = 0; i < O; ++i) {
                pValue = _totalList[_poList[i]->getID()]->_patternValue;
                bitset<sizeof(pValue) * 8> s(pValue);
                *_simLog << s[j];
                //*_simLog << " PO  sizeof(pValue) = " << sizeof(pValue) << ", pValue = " << s ;    
            }
            *_simLog << endl;
        }
    }
}
void
CirMgr::outputToSimLog(int **simArray, int simArrayTail, int bound){
    int i,j;
    unsigned pValue, tailSize = simArrayTail;
    if(_simLog != 0) {
        for(j = simArrayTail; j >= simArrayTail-bound; --j){
            for(i = 0; i < I; ++i) {
                *_simLog << simArray[j][i];
            }
            *_simLog << " ";
            for(i = 0; i < O; ++i) {
                pValue = _totalList[_poList[i]->getID()]->_patternValue;
                bitset<sizeof(pValue) * 8> s(pValue);
                *_simLog << s[tailSize - j];
                //*_simLog << " PO  sizeof(pValue) = " << sizeof(pValue) << ", pValue = " << s ;
                
            }
            *_simLog << endl;
        }
    }
}
void
CirMgr::printSimArray(int **simArray)
{
    int i, j;
    for(i = 0; i < I; ++i) {
        for (j = 0; j < parallelizeBits; ++j){  
            cout << *(*(simArray+j)+i);
        }    
        cout << endl;
    }   
    cout << endl; 
    
}


