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

using namespace std;
#define myI  10000
#define parallelizeBits 32
//#define debug_fileSim

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
}

void
CirMgr::fileSim(ifstream& patternFile){

    string line;
    int lineCount = 0, tmpLineCount, i,j,k;
    int **simArray; // create array to store PIs , I: PI's nums
    int simArrayTail = parallelizeBits - 1;
    bool isReadError = false;
    unsigned patternValue = 0; 
    
    simArray = new int *[32];
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
        lineCount++;
        if(lineCount%parallelizeBits == 0) {
            //setPattern(simArray);
            #ifdef debug_fileSim
                CirMgr::printSimArray(simArray);
            #endif
            CirMgr::setPatternValue(simArray);
        }
    }
    cout << endl << endl;
    if(!isReadError){
        if(lineCount%parallelizeBits != 0 ){ // last bitwise , maybe not have parallelizeBits bit unsigned value
            for(i = 0; i < I; ++i ){  // not assigned postion , put 0 
                tmpLineCount = lineCount;
                for(j = 0; j < parallelizeBits - lineCount%32; ++j) {
                    simArray[simArrayTail-tmpLineCount%parallelizeBits][i] = 0;
                    ++tmpLineCount;
                }
            }
            #ifdef debug_fileSim
               CirMgr::printSimArray(simArray);
            #endif
            CirMgr::setPatternValue(simArray);
        }
    } else {
        cout << "Something error when get input" << endl;
    }
}
//
//void
//CirMgr::fileSim(ifstream& patternFile)
//{
//    //in CirCmd , file open check is done
//    //Direct read patternFile 
//    string line;
//    int lineCount = 0, tmpLineCount, i,j,k;
//    int simArray[parallelizeBits][I]; // create array to store PIs , I: PI's nums
//    int simArrayTail = parallelizeBits - 1;
//    bool isReadError = false;
//    unsigned patternValue = 0; 
//    //initial simArray
//    for (j = 0; j < parallelizeBits; ++j){  
//        for(i = 0; i < I; ++i){
//            simArray[j][i] = 0;
//        }    
//    }    
//
//    while(getline(patternFile,line)){  // read all data to 2D array
//        //cout << "line length = " << line.length() << endl;
//        for(i = 0; i < I; ++i) {
//            if( (line[i] != '0') && (line[i] != '1')  )
//            {
//                isReadError = true;
//                cout << "Line = "<< lineCount << ", line[" << i  << "] = "<< line[i] << endl;
//                break;
//            }
//            simArray[simArrayTail-lineCount%parallelizeBits][i] = line[i] - '0';
//            #ifdef debug_fileSim
//                cout << "line[" << i << "] = " << line[i];
//                cout << "simArray[" << lineCount%parallelizeBits << "][" << i << "] = " << line[i] << ",   ";
//            #endif
//        }
//        #ifdef debug_fileSim
//            cout << endl <<"I = " << I << endl;
//        #endif
//        lineCount++;
//        if(lineCount%parallelizeBits == 0) {
//            //setPattern(simArray);
//            //CirMgr::printSimArray(simArray);
//            for(i = 0; i < I; ++i) {
//                #ifdef debug_fileSim
//                    cout << "Hi, PI" << i << " = " << i+1 << ",  pattern = ";  
//                #endif
//                patternValue = 0;
//                for(j = 0; j < parallelizeBits; ++j){
//                    #ifdef debug_fileSim
//                        cout << "simArray[" << j <<"][" << i <<"] = " <<  simArray[j][i] << ", "; 
//                        cout << simArray[j][i];
//                        cout << endl;
//                    #endif
//                    //bitset<sizeof(patternValue) * 8> s(patternValue);
//                    patternValue += simArray[j][i];
//                    //cout << "(1) i = " << i << "j = " << j << ",  patternValue = " << s << endl;  
//                    if(j != parallelizeBits - 1)
//                        patternValue = patternValue << 1;
//                    //cout << "  i = " << i << "j = " << j << ",  patternValue = " << s << endl;  
//                    
//                }
//                    bitset<sizeof(patternValue) * 8> s(patternValue);
//                    cout << "i = " << i << ",  patternValue = " << s << endl;  
//                    #ifdef debug_fileSim
//                        cout << "simArray[" << j <<"][" << i <<"] = " <<  simArray[j][i] << ", "; 
//                        cout << endl; 
//                    #endif
//            }
//        }
//        
//    }
//    cout << endl << endl;
//    if(!isReadError){
//        if(lineCount%parallelizeBits != 0 ){ // last bitwise , maybe not have parallelizeBits bit unsigned value
//            #ifdef debug_fileSim
//                cout << endl <<"tmp I = " << I << endl;
//                cout << endl <<"I = " << I << endl;
//            #endif
//        
//            for(i = 0; i < I; ++i ){  // not assigned postion , put 0 
//                tmpLineCount = lineCount;
//                    #ifdef debug_fileSim
//                    cout << "put zero , parallelizeBits = " << parallelizeBits <<", lineCount%32 = " << lineCount%32  << endl;
//                    #endif
//                for(j = 0; j < parallelizeBits - lineCount%32; ++j) {
//                    simArray[simArrayTail-tmpLineCount%parallelizeBits][i] = 0;
//                    ++tmpLineCount;
//                    //cout << "put zero " << endl;
//                }
//            }
//            for(i = 0; i < I; ++i) {
//                    #ifdef debug_fileSim
//                        cout << ", PI" << i << " = " << i+1 << ",  pattern = ";  
//                    #endif
//                patternValue = 0;
//                for(j = 0; j < parallelizeBits; ++j){
//                    #ifdef debug_fileSim
//                        cout << "i = " << i << ", j = " << j << ", "; 
//                        cout << simArray[j][i];
//                        cout << endl;
//                    #endif
//                    patternValue += simArray[j][i];
//                    if(j != parallelizeBits - 1)
//                        patternValue = patternValue << 1;
//                }
//                    bitset<sizeof(patternValue) * 8> s(patternValue);
//                    cout << "i = " << i << ",  patternValue = " << s << endl;  
//                #ifdef debug_fileSim
//                    cout << "   , notFinish" << endl; 
//                #endif
//            }
//        }
//    } else {
//        cout << "Something error when get input" << endl;
//    }
//}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
void
CirMgr::setPatternValue(int **simArray)
{
    int i, j;
    unsigned patternValue = 0; 
    for(i = 0; i < I; ++i) {
        patternValue = 0;
        for(j = 0; j < parallelizeBits; ++j){
            patternValue += simArray[j][i];
            if(j != parallelizeBits - 1)
                patternValue = patternValue << 1;
            
        }
        bitset<sizeof(patternValue) * 8> s(patternValue);
        cout << "i = " << i << ",  patternValue = " << s << endl;      
    }
    cout << endl;

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


