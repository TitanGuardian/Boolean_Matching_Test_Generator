#include "../include/Test.h"
Test::Test(const uint32_t &_gateCount, bool  is_use_dnfAIG, bool _need_only_one_circuit ) {
	is_cascade = !is_use_dnfAIG;
	gateCount = _gateCount;
	need_only_one_circuit=_need_only_one_circuit;
	funcVector = new std::vector<bool>;
	*funcVector = Test::rngFuncVector(gateCount);
	negative = new std::vector<bool>;
	*negative = Test::rngNegative(gateCount);
	permutation = new std::vector<uint32_t>;
	*permutation = Test::rngPermutation(gateCount);
	funcVectorSize = std::pow(2, gateCount);
	countOfFuncElem = _gateCount;
	countOfFuncElem = 0;
	func2gateLink.resize(16);
	tmpFile.open("./out/tmp.txt");
	test.open("./out/TestA.v");
	if (!need_only_one_circuit)  testMix.open("./out/TestB.v");
	fileConfig.open("./out/Current Configuration.txt");
	startMain();
	if (!need_only_one_circuit)  startMix();

	if (is_cascade) outFElink=cascade();
	else stratHandleFuncVect();

	if (!need_only_one_circuit) writeToV();
	else writeToVOnlyOne();

	outVec();
}

Test::Test (const uint32_t &_gateCount,const std::vector<bool> & _funcVector,  const std::vector<bool> & _negative, const std::vector<uint32_t>& _permutation, bool  is_use_dnfAIG, bool _need_only_one_circuit) {
	is_cascade = !is_use_dnfAIG;
	gateCount = _gateCount;
	need_only_one_circuit=_need_only_one_circuit;
	funcVector = new std::vector<bool>;
	*funcVector = _funcVector;
	negative = new std::vector<bool>;
	*negative = _negative;
	permutation = new std::vector<uint32_t>;
	*permutation = _permutation;
	funcVectorSize = std::pow(2, gateCount);
	countOfFuncElem = _gateCount;
	countOfFuncElem = 0;
	func2gateLink.resize(16);
	tmpFile.open("./out/tmp.txt");
	test.open("./out/TestA.v");
	if (!need_only_one_circuit) testMix.open("./out/TestB.v");
	fileConfig.open("./out/Current Configuration.txt");
	startMain();
  	if (!need_only_one_circuit) startMix();
	if (is_cascade) outFElink=cascade();
	else stratHandleFuncVect();
	if (!need_only_one_circuit) writeToV();
    	else writeToVOnlyOne();

	outVec();
}
Test::~Test() {
	tmpFile.close();
	test.close();
	fileConfig.close();
        if (!need_only_one_circuit) testMix.close();
	delete funcVector;
	delete permutation;
	delete negative;
}


void Test::writeToVOnlyOne() {
        test << "module top(";
        std::string str;

        for (uint32_t itr = 0; itr < gateCount; ++itr) {
                str = "in" + std::to_string(itr)+",";
                test << str;
        }
        test << "out); \n";
        test << "input in0";
        for (uint32_t itr = 1; itr < gateCount; ++itr) {
                str = ","+("in" + std::to_string(itr));
                test << str;
        }
        test << ";\noutput out; \n";
        test << "wire in0";
        for (uint32_t itr = 1; itr < gateCount; ++itr) {
                str = "," + ("in" + std::to_string(itr));
                test << str;

        }
        test << ";\nwire out; \n";


        test << "wire e0";

        for (uint32_t itr = 1; itr < countOfFuncElem; ++itr) {
                str = "," + ("e" + std::to_string(itr));
                test << str;

        }
        test << ";\n";




        for (uint32_t itr = 0; itr < listOfStartMain.size(); ++itr) {
                test << listOfStartMain[itr] << std::endl;
        }

        if (!is_cascade) {
                tmpFile.close();
                fileForRead.open("./out/tmp.txt");
                char buf[MAX_BUF_SIZE];
                do {
                        fileForRead.read(&buf[0], MAX_BUF_SIZE);
                        test.write(&buf[0], fileForRead.gcount());
                } while (fileForRead.gcount() > 0);
                fileForRead.close();
        }
        else {
                int itrC = 0;
                for (FE curFE : listOfFE) {
                        switch (curFE.type) {
                        case 0: { // buf
                                buffer += "buf (e"+std::to_string(curFE.out)+", e"+ std::to_string(curFE.in1)+");\n";
                                break;
                        }
                        case 1: { // not
                                buffer +=  "not (e" + std::to_string(curFE.out) + ", e" + std::to_string(curFE.in1) + ");\n";
                                break;
                        }
                        case 2: { // and
                                buffer +=  "and (e" + std::to_string(curFE.out) + ", e" + std::to_string(curFE.in1) + ", e" + std::to_string(curFE.in2) + ");\n";
                                break;
                        }
                        case 3: { // or

                                buffer +=  "or (e" + std::to_string(curFE.out) + ", e" + std::to_string(curFE.in1) + ", e" + std::to_string(curFE.in2) + ");\n";

                                break;
                        }
                        }
                        ++itrC;
                        if (itrC == 1000000) {
                                test << buffer;
                                itrC = 0;
                                buffer.clear();
                        }
                }
                test << buffer;
                buffer.clear();
                test << "buf (out, e" + std::to_string(outFElink) + ");" << std::endl;
        }
        //--
        test << "endmodule\n";
}




void Test::writeToV() {
	test << "module top(";
	testMix << "module top(";
	std::string str;

        for (uint32_t itr = 0; itr < gateCount; ++itr) {
		str = "in" + std::to_string(itr)+",";
		test << str;
		testMix << str;
	}
	test << "out); \n";
	testMix << "out); \n";
	test << "input in0";
	testMix << "input in0";
        for (uint32_t itr = 1; itr < gateCount; ++itr) {
		str = ","+("in" + std::to_string(itr));
		test << str;
		testMix << str;
	}
	test << ";\noutput out; \n";
	testMix << ";\noutput out; \n";

	test << "wire in0";
	testMix << "wire in0";
        for (uint32_t itr = 1; itr < gateCount; ++itr) {
		str = "," + ("in" + std::to_string(itr));
		test << str;
		testMix << str;
	}
	test << ";\nwire out; \n";
	testMix << ";\nwire out; \n";

	test << "wire e0";
	testMix << "wire e0";
        for (uint32_t itr = 1; itr < countOfFuncElem; ++itr) {
		str = "," + ("e" + std::to_string(itr));
		test << str;
		testMix << str;
	}
	test << ";\n";
	testMix << ";\n";


	//for testMix
	testMix << "wire inAfterInv0";
        for (uint32_t itr = 1; itr < gateCount; ++itr) {
		str = "," + ("inAfterInv" + std::to_string(itr));
		testMix << str;
	}
	testMix << ";\n";
	//---

        for (uint32_t itr = 0; itr < listOfStartMain.size(); ++itr) {
		test << listOfStartMain[itr] << std::endl;
	}
        for (uint32_t itr = 0; itr < listOfStartMix.size(); ++itr) {
		testMix << listOfStartMix[itr] << std::endl;
	}

	if (!is_cascade) {
		tmpFile.close();
		fileForRead.open("./out/tmp.txt");
		char buf[MAX_BUF_SIZE];
		do {
			fileForRead.read(&buf[0], MAX_BUF_SIZE);
			test.write(&buf[0], fileForRead.gcount());
			testMix.write(&buf[0], fileForRead.gcount());
		} while (fileForRead.gcount() > 0);
		fileForRead.close();
	}
	else {
		int itrC = 0;
		for (FE curFE : listOfFE) {
			switch (curFE.type) {
			case 0: { // buf
				buffer += "buf (e"+std::to_string(curFE.out)+", e"+ std::to_string(curFE.in1)+");\n";
				break;
			}
			case 1: { // not
				buffer +=  "not (e" + std::to_string(curFE.out) + ", e" + std::to_string(curFE.in1) + ");\n";
				break;
			}
			case 2: { // and
				buffer +=  "and (e" + std::to_string(curFE.out) + ", e" + std::to_string(curFE.in1) + ", e" + std::to_string(curFE.in2) + ");\n";
				break;
			}
			case 3: { // or

				buffer +=  "or (e" + std::to_string(curFE.out) + ", e" + std::to_string(curFE.in1) + ", e" + std::to_string(curFE.in2) + ");\n";

				break;
			}
			}
			++itrC;
			if (itrC == 1000000) {
				test << buffer;
				testMix << buffer;
				itrC = 0;
				buffer.clear();
			}
		}
		test << buffer;
		testMix << buffer;
		buffer.clear();
		test << "buf (out, e" + std::to_string(outFElink) + ");" << std::endl;
		testMix << "buf (out, e" + std::to_string(outFElink) + ");" << std::endl;
	}
	//--
	test << "endmodule\n";
	testMix << "endmodule\n";
}


void Test::startMain() {
        for (uint32_t itr = 0; itr < gateCount; ++itr) {
		listOfStartMain.push_back(std::string("buf (e"+ std::to_string(itr))+(", in"+ std::to_string(itr))+");");
		listOfStartMain.push_back(std::string("not (e" + std::to_string(itr+gateCount)) + (", in" + std::to_string(itr)) + ");");
		++countOfFuncElem;
		++countOfFuncElem;
	}



}
void Test::startMix() {
        for (uint32_t itr = 0; itr < gateCount; ++itr) {
		if ((*negative)[itr]) {
			listOfStartMix.push_back(std::string("not (inAfterInv" + std::to_string(itr)) + (", in" + std::to_string(itr)) + ");");
		}
		else {
			listOfStartMix.push_back(std::string("buf (inAfterInv" + std::to_string(itr)) + (", in" + std::to_string(itr)) + ");");
		}
	}
        for (uint32_t itr = 0; itr < gateCount; ++itr) {
		listOfStartMix.push_back(std::string("buf (e" + std::to_string(itr)) + (", inAfterInv" + std::to_string((*permutation)[itr])) + ");");
		listOfStartMix.push_back(std::string("not (e" + std::to_string(itr+gateCount)) + (", inAfterInv" + std::to_string((*permutation)[itr])) + ");");
	}
}



void Test::stratHandleFuncVect(){
        uint32_t i = 0, last=0;
        uint64_t it = funcVectorSize / 100;
	std::string a, b,tmp2;
	std::vector<bool> cubeDot(gateCount);
	for (i = 0; i < funcVectorSize; ++i) {
		if ((*funcVector)[i]) {
			cubeDot = transformTo2(i);
			/*
                        for (uint32_t j = 0; j < gateCount; ++j) {
				std::cout << cubeDot[j];
			}
			std::cout << std::endl;
			*/
			createKonjScheme(cubeDot);
		}
		else {}
		if ((it!=0) && ( i / it > last)) {
			++last;
			std::cout << last << "%" << std::endl;
		}
	}
	outList();
	while (queueDisj.size() > 1) {
		a = queueDisj.front();
		queueDisj.pop_front();
		b = queueDisj.front();
		queueDisj.pop_front();
		tmp2 = "e" + std::to_string(countOfFuncElem);
		++countOfFuncElem;
		//listOfElementsForWrite.push_back("and (" + tmp2 + ", " + a +", "+b+ ");");
		buffer += "and (" + tmp2 + ", " + a + ", " + b + ");" + "\n";
		queueDisj.push_back(tmp2);
	}
	a = queueDisj.front();
	queueDisj.pop_front();
	buffer += "not (out,"  + a + ");" + "\n";

	outList();


}

void Test::createKonjScheme(std::vector<bool> cubeDot) {
        uint32_t itr;
	std::string tmp,tmp2;
	for (itr = 0; itr < gateCount;++itr) {
		if (cubeDot[itr]) {
			tmp2 = "b" + std::to_string(itr);
			queueKonj.push_back(tmp2);
		}
		else {
			tmp2 = "e" + std::to_string(itr);
			queueKonj.push_back(tmp2);
		}
	}

	std::string a, b;
	while (queueKonj.size() > 1) {
		a = queueKonj.front();
		queueKonj.pop_front();
		b = queueKonj.front();
		queueKonj.pop_front();
		tmp2 = "e" + std::to_string(countOfFuncElem);
		++countOfFuncElem; 
		//listOfElementsForWrite.push_back("and (" + tmp2 + ", " + a +", "+b+ ");");
		buffer+="and (" + tmp2 + ", " + a + ", " + b + ");" + "\n";
		queueKonj.push_back(tmp2);	
	}
	a = queueKonj.front();
	queueKonj.pop_front();
	tmp2 = "e" + std::to_string(countOfFuncElem);
	++countOfFuncElem;
	buffer += "not (" + tmp2 + ", " + a + ");" + "\n";
	queueDisj.push_back(tmp2);
	if (buffer.size()>= 1000000) outList();
}


void Test::outList() {
	tmpFile <<buffer;
	buffer.clear();
}

std::vector<bool> Test::transformTo2(uint32_t number) {
        uint32_t itr;
	std::vector<bool> bitset(gateCount);
	itr = gateCount;
	do {
		--itr;
		bitset[itr] = number % 2;
		
		number /= 2;
	} while (itr != 0);

	return bitset;
}

uint32_t Test::rngSmall() {
	std::srand(unsigned(std::time(0)));
	return (std::abs(std::rand()*std::rand() + std::rand())) % 6 + 1;
}
uint32_t Test::rngMed() {
	std::srand(unsigned(std::time(0)));
	return (std::abs(std::rand()*std::rand() + std::rand())) % 14 + 7;
}
uint32_t Test::rngLarge() {
	std::srand(unsigned(std::time(0)));
	return (std::abs(std::rand()*std::rand()+ std::rand())) % (MAX_GATE_NUMBER) + 20;
}

std::vector <bool> Test::rngFuncVector(uint32_t _gateCount) {
        uint64_t fNumber = std::pow(2, _gateCount);
	std::vector<bool> fVect(fNumber);
        uint32_t i;
	std::srand(unsigned(std::time(0)));
	for (i = 0; i < fNumber; ++i) {
		fVect[i] = (std::abs(std::rand()*std::rand() + std::rand())) %2;
	}
	return fVect;
}

std::vector<uint32_t> Test::identityPermutation(uint32_t _gateCount) {
        std::vector<uint32_t> newIP(_gateCount);
        for (uint32_t i = 0; i < _gateCount; ++i) {
		newIP[i] = i;
	}
	return newIP;
}

std::vector<uint32_t> Test::rngPermutation(uint32_t _gateCount) {
        std::vector<uint32_t> newIP(_gateCount);
	std::srand(unsigned(std::time(0)));
        uint32_t j,tmp;
        for (uint32_t i = 0; i < _gateCount; ++i) {
		newIP[i] = i;
	}
        for (uint32_t i = _gateCount-1; i > 0; --i) {
		j = (std::abs(std::rand()*std::rand() + std::rand())) % (i + 1);
		tmp = newIP[j];
		newIP[j]=newIP[i];
		newIP[i] = tmp;
	}
	return newIP;
}

std::vector <bool> Test::rngNegative(uint32_t _gateCount) {
	std::vector<bool> neg(_gateCount);
        uint32_t i;
	std::srand(unsigned(std::time(0)));
	for (i = 0; i < _gateCount; ++i) {
		neg[i] = (std::abs(std::rand()*std::rand() + std::rand())) % 2;
	}
	return neg;
}
std::vector <bool> Test::noNegative(uint32_t _gateCount) {
	std::vector<bool> neg(_gateCount);
        uint32_t i;
	for (i = 0; i < _gateCount; ++i) {
		neg[i] = false;
	}
	return neg;
}




//

uint64_t Test::cascade() {
        uint64_t outRes;
	std::deque<Decomposition> queueOfDecomp; 
	FE tmpFE0,tmpFE1,tmpFE2;
	Decomposition tmpDec,tmpDec2;


	if (gateCount==2) {
		outRes = create2GateFunc(0);
	}
	else {
		tmpDec.currVar = 0;
		tmpDec.left = 0;
		tmpDec.right = funcVectorSize - 1;
		tmpDec.preOut = outRes = countOfFuncElem;
		countOfFuncElem++;
		queueOfDecomp.push_back(tmpDec);

		while (queueOfDecomp.size()) {
			tmpDec2 = queueOfDecomp.front();
			queueOfDecomp.pop_front();

			if (tmpDec2.currVar < gateCount - 3) {
				// decomposition negative
				tmpFE0.in1 = notX(tmpDec2.currVar);
				tmpFE0.in2 = countOfFuncElem;
				countOfFuncElem++;
				tmpFE0.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE0.type = _AND_;
				listOfFE.push_back(tmpFE0); // add to list of FE
				tmpDec.currVar = tmpDec2.currVar + 1;
				tmpDec.left = tmpDec2.left;
				tmpDec.right = tmpDec2.left + (tmpDec2.right - tmpDec2.left) / 2;
				tmpDec.preOut = tmpFE0.in2;
				queueOfDecomp.push_back(tmpDec);

				// decomposition positive 

				tmpFE1.in1 = tmpDec2.currVar;
				tmpFE1.in2 = countOfFuncElem;
				countOfFuncElem++;
				tmpFE1.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE1.type = _AND_;
				listOfFE.push_back(tmpFE1); // add to list of FE
				tmpDec.currVar = tmpDec2.currVar + 1;
				tmpDec.left = tmpDec2.right - (tmpDec2.right - tmpDec2.left) / 2;
				tmpDec.right = tmpDec2.right;
				tmpDec.preOut = tmpFE1.in2;
				queueOfDecomp.push_back(tmpDec);

				//composition 

				tmpFE2.in1 = tmpFE0.out;
				tmpFE2.in2 = tmpFE1.out;
				tmpFE2.out = tmpDec2.preOut;
				tmpFE2.type = _OR_;
				listOfFE.push_back(tmpFE2); // add to list of FE
			}
			else {
				// decomposition negative
				tmpFE0.in1 = notX(tmpDec2.currVar);
				tmpFE0.in2 = create2GateFunc(tmpDec2.left);
				tmpFE0.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE0.type = _AND_;
				listOfFE.push_back(tmpFE0); // add to list of FE


				// decomposition positive 

				tmpFE1.in1 = tmpDec2.currVar;
				tmpFE1.in2 = create2GateFunc(tmpDec2.right - 3);
				tmpFE1.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE1.type = _AND_;
				listOfFE.push_back(tmpFE1); // add to list of FE

				//composition 

				tmpFE2.in1 = tmpFE0.out;
				tmpFE2.in2 = tmpFE1.out;
				tmpFE2.out = tmpDec2.preOut;
				tmpFE2.type = _OR_;
				listOfFE.push_back(tmpFE2); // add to list of FE
			}

		}
	}
	return outRes;
}


uint32_t Test::create2GateFunc(const uint64_t &left) {
	FE tmpFE;
        uint8_t func;
        uint32_t out;
	func = (*funcVector)[left] * 8 + ((*funcVector)[left+1]) * 4 + ((*funcVector)[left+2]) * 2 + ((*funcVector)[left+3]);

	if (!func2gateLink[func].is_exist) {
		switch (func) {

			case 0: {
				// 0000
				tmpFE.in1 = gateCount - 2;
				tmpFE.in2 = notX(gateCount - 2);
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _AND_;
				listOfFE.push_back(tmpFE);

				func2gateLink[0].is_exist = true;
				func2gateLink[0].link = tmpFE.out;
				break;
			}
			case 1: {
				// 0001
				tmpFE.in1 = gateCount - 2;
				tmpFE.in2 = gateCount - 2 + 1;
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _AND_;
				listOfFE.push_back(tmpFE);

				func2gateLink[1].is_exist = true;
				func2gateLink[1].link = tmpFE.out;
				break;
			}
			case 2: {
				//0010
				tmpFE.in1 = gateCount - 2;
				tmpFE.in2 = notX(gateCount - 2 + 1);
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _AND_;
				listOfFE.push_back(tmpFE);

				func2gateLink[2].is_exist = true;
				func2gateLink[2].link = tmpFE.out;
				break;
			}
			case 3: {
				//0011
				func2gateLink[3].is_exist = true;
				func2gateLink[3].link = gateCount-2;

				break;
			}
			case 4: {
				//0100
				tmpFE.in1 = gateCount - 2 + 1;
				tmpFE.in2 = notX(gateCount - 2);
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _AND_;
				listOfFE.push_back(tmpFE);

				func2gateLink[4].is_exist = true;
				func2gateLink[4].link = tmpFE.out;
				break;
			}
			case 5: {
				//0101
				func2gateLink[5].is_exist = true;
				func2gateLink[5].link = gateCount - 1;

				break;
			}
			case 6: {
				//0110
                                uint32_t a, b;

				tmpFE.in1 = gateCount - 2;
				tmpFE.in2 = notX(gateCount - 2 + 1);
				a=tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _AND_;
				listOfFE.push_back(tmpFE);

				tmpFE.in1 = gateCount - 2 + 1;
				tmpFE.in2 = notX(gateCount - 2);
				b= tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _AND_;
				listOfFE.push_back(tmpFE);

				tmpFE.in1 = a;
				tmpFE.in2 = b;
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _OR_;
				listOfFE.push_back(tmpFE);

				func2gateLink[6].is_exist = true;
				func2gateLink[6].link = tmpFE.out;
				break;
			}
			case 7: {
				//0111
				tmpFE.in1 = gateCount - 2;
				tmpFE.in2 = gateCount - 2 + 1;
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _OR_;
				listOfFE.push_back(tmpFE);

				func2gateLink[7].is_exist = true;
				func2gateLink[7].link = tmpFE.out;


				break;
			}
			case 8: {
				//1000
				tmpFE.in1 = notX(gateCount - 2);
				tmpFE.in2 = notX(gateCount - 2 + 1);
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _AND_;
				listOfFE.push_back(tmpFE);

				func2gateLink[8].is_exist = true;
				func2gateLink[8].link = tmpFE.out;
				break;
			}
			case 9: {
				//1001
                                uint32_t a, b;

				tmpFE.in1 = notX(gateCount - 2);
				tmpFE.in2 = notX(gateCount - 2 + 1);
				a = tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _AND_;
				listOfFE.push_back(tmpFE);

				tmpFE.in1 = gateCount - 2 + 1;
				tmpFE.in2 = gateCount - 2;
				b = tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _AND_;
				listOfFE.push_back(tmpFE);

				tmpFE.in1 = a;
				tmpFE.in2 = b;
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _OR_;
				listOfFE.push_back(tmpFE);

				func2gateLink[9].is_exist = true;
				func2gateLink[9].link = tmpFE.out;
				break;
			}
			case 10: {
				//1010
				func2gateLink[10].is_exist = true;
				func2gateLink[10].link = notX(gateCount - 1);
				break;
			}
			case 11: {
				//1011
				tmpFE.in1 = gateCount - 2;
				tmpFE.in2 = notX(gateCount - 2 + 1);
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _OR_;
				listOfFE.push_back(tmpFE);

				func2gateLink[11].is_exist = true;
				func2gateLink[11].link = tmpFE.out;
				break;
			}
			case 12: {
				//1100
				func2gateLink[12].is_exist = true;
				func2gateLink[12].link = notX(gateCount - 2);
				break;
			}
			case 13: {
				//1101
				tmpFE.in1 = gateCount - 2 + 1;
				tmpFE.in2 = notX(gateCount - 2);
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _OR_;
				listOfFE.push_back(tmpFE);

				func2gateLink[13].is_exist = true;
				func2gateLink[13].link = tmpFE.out;
				break;
			}
			case 14: {
				//1110
				tmpFE.in1 = notX(gateCount - 2);
				tmpFE.in2 = notX(gateCount - 2 + 1);
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _OR_;
				listOfFE.push_back(tmpFE);

				func2gateLink[14].is_exist = true;
				func2gateLink[14].link = tmpFE.out;
				break;
			}
			case 15: {
				//1111
				tmpFE.in1 = gateCount - 2;
				tmpFE.in2 = notX(gateCount - 2);
				tmpFE.out = countOfFuncElem;
				countOfFuncElem++;
				tmpFE.type = _OR_;
				listOfFE.push_back(tmpFE);

				func2gateLink[15].is_exist = true;
				func2gateLink[15].link = tmpFE.out;
				break;
			}
			default: {
				out = 0;
			}
		}
	}
	out = func2gateLink[func].link;
	return out;
}







// erase
void Test::outVec() {
	buffer +=  "Count of inputs: "+std::to_string(gateCount)+ "\n";

	buffer +=  "Permutation: \n";
        for (uint32_t b : *permutation) {
		buffer +=  std::to_string(b) + " ";
	}
	buffer +=  "\n";
	buffer +=  "Negativation: \n";
        for (uint32_t c : *negative) {
		buffer += std::to_string(c) + " ";
	}
	buffer +=  "\n";
	buffer +=  "Function vector:\n";
	for (bool a : *funcVector) {
		buffer += std::to_string(a) + " ";
	}


	fileConfig << buffer;
	buffer.clear();
}


