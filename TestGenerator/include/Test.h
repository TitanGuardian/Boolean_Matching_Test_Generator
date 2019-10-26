#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <set>
#include <map>
#include <deque>
#include <fstream>
#include <cstdint>


#define MAX_GATE_NUMBER 24
#define MAX_BUF_SIZE 32768

#define _BUF_ 0
#define _NOT_ 1
#define _AND_ 2
#define _OR_  3
#define _WIRE_ 4

#define notX(a) (a+gateCount)

class Test {
private:
	//structs
	struct FE {
		uint32_t in1, in2;
		uint8_t type; // 0 buf , 1 not ,2 and ,3 or
		uint32_t out;
	};
	struct Decomposition {
		uint32_t currVar;
		uint32_t preOut;
		uint64_t left, right;
		/*Decomposition(uint32_t _lastVar, uint32_t _preOut, uint64_t  _left, uint64_t  _right) :
			lastVar(_lastVar),
			preOut(_preOut),
			left(_left),
			right(_right) {};
			*/
	};
	struct Func2gateExist {
		bool is_exist;
		uint32_t link;
	};
	//data
    	bool need_only_one_circuit;
	std::vector<bool> *funcVector;
	uint32_t gateCount;
	uint64_t funcVectorSize;
	std::vector<uint32_t> *permutation;
	std::vector<bool> *negative;
	//std::vector<std::string> listOfElementsForWrite;
	std::vector<FE> listOfFE;
	std::map<std::vector<char>, uint32_t> existKonj;
	uint64_t countOfFuncElem;
	std::vector<std::string> listOfStartMain;
	std::vector<std::string> listOfStartMix;
	std::deque<std::string>  queueKonj;
	std::deque<std::string>  queueDisj;
	std::ofstream tmpFile, test, testMix, fileConfig;
	std::ifstream fileForRead;
	std::string buffer;
	std::vector<Func2gateExist> func2gateLink;
	uint64_t outFElink;
	bool is_cascade;
	//close function
	void stratHandleFuncVect();
	std::vector<bool> transformTo2(uint32_t number);
	void createKonjScheme(std::vector<bool> cubeDot);
	void startMain();
	void startMix();
	void outList();
	void writeToV();
 	void writeToVOnlyOne();
	uint64_t cascade();
	uint32_t create2GateFunc(const uint64_t &left);

public:
	Test(const uint32_t & _gateCount,bool  is_use_dnfAIG = 0,bool _need_only_one_circuit=0);

	Test(    const uint32_t &_gateCount,
		 const std::vector<bool>& _funcVector,
		 const std::vector<bool> & _negative,
		 const std::vector<uint32_t>& _permutation,
		 bool  is_use_dnfAIG = 0,
		 bool _need_only_one_circuit=0
	    );
		
	static uint32_t rngSmall();
	static uint32_t rngMed();
	static uint32_t rngLarge();
	
	static std::vector<uint32_t> identityPermutation(uint32_t _gateCount);
	static std::vector<uint32_t> rngPermutation(uint32_t _gateCount);
	static std::vector<bool> rngFuncVector(uint32_t _gateCount);
	static std::vector<bool> rngNegative(uint32_t _gateCount);
	static std::vector<bool> noNegative(uint32_t _gateCount);

	void outVec();

	//int writeVL(std::string name);

	~Test();
};
