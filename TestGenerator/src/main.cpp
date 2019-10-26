#include <iostream>
#include "../include/Test.h"
#include "../../boost/program_options.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <cstdint>
#include <cmath>

extern "C"  int balanceDD();
extern "C"  void init_genrand64(unsigned long long seed);
extern "C"  unsigned long long genrand64_int64(void);


namespace
{
    const size_t ERROR_IN_COMMAND_LINE = 1;
    const size_t SUCCESS = 0;
    const size_t ERROR_UNHANDLED_EXCEPTION = 2;

} // namespace

int main(int argc, char** argv)
{
    try
    {
        namespace po = boost::program_options;
        po::options_description desc("Options");
        desc.add_options()
                ("help,h", "Print help messages")
                ("neq,n", "Create 2 not equivalent circuits")
                ("inputcount,i",  po::value<uint32_t>(), "Number of circuit gates")
                ("small,s", "create small (2-6 gates) circuit")
                ("medium,m", "create medium (7-19 gates) circuit")
                ("large,l", "create large (20+ gates) circuit")
                ("allpositive,p", "no gate negativation")
                ("onlyone,o", "generate only one circuit")
                ("dnf,d", "use DNF")
                ("abc,a", "abc optimisation")
                ("many,g", po::value<uint32_t>(), "create many (m) tests | arg: m = count")
                ("nopermutation,t", "no gate permutation");
                //("movetotests, c", "move output files to GeneratedTests folder");

        po::variables_map vm;
        try
        {
            po::store(po::parse_command_line(argc, argv, desc),
                      vm); // can throw

            /** --help option
             */
            if ( vm.count("help")  )
            {
                std::cout << "Basic Command Line Parameter App" << std::endl
                          << desc << std::endl;
                return SUCCESS;
            }

            po::notify(vm); // throws on error, so do after help in case
            // there are any problems
        }
        catch(po::error& e)
        {
            std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
            std::cerr << desc << std::endl;
            return ERROR_IN_COMMAND_LINE;
        }

        // application code here //
        std::srand(time(0));
        init_genrand64(std::rand());

        uint32_t gate_count= 10;
        uint32_t itr , itrcnt=1;
        std::string basestr = "../GeneratedTests/", tmpstr;

        if (vm.count("small")) gate_count = Test::rngSmall();
        if (vm.count("medium")) gate_count = Test::rngMed();
        if (vm.count("large")) gate_count = Test::rngLarge();
        if (vm.count("inputcount")) gate_count = vm["inputcount"].as<uint32_t>();
        time_t now=time(0);
        if (vm.count("many")) itrcnt = vm["many"].as<uint32_t>();

        if (vm.count("neq")) basestr+="neq/";
        else basestr += "eq/";

        if (gate_count<=6) basestr+="small/";
        else if (gate_count<=20) basestr+="medium/";
             else basestr += "large/";
        uint32_t localconf;

        tmpstr = basestr + "conf.txt";

        std::ifstream confin (tmpstr.c_str());
        confin>>tmpstr;
        confin.close();
		try {
			localconf = std::stoi(tmpstr); 
		}
		catch (...) {
			localconf = 0;
		}
        std::cout<<"Generating "<<itrcnt<<" circuits"<<std::endl;
        for (itr = 0 ; itr < itrcnt ; ++itr) {
            if (vm.count("neq")) {

                std::vector<bool> _funcVector = Test::rngFuncVector(gate_count);
                std::vector<bool> _negative = vm.count("allpositive") ? Test::noNegative(gate_count)
                                                                      : Test::rngNegative(gate_count);
                std::vector <uint32_t> _permutation = vm.count("nopermutation") ? Test::identityPermutation(gate_count)
                                                                                : Test::rngPermutation(gate_count);
                bool _dnf = vm.count("dnf") ? 1 : 0;
                Test *test;
                test = new(std::nothrow) Test(gate_count,
                                              _funcVector,
                                              _negative,
                                              _permutation,
                                              _dnf,
                                              1);
                delete test;
                system("mv -f ./out/TestA.v ./out/TestB.v");

                // function changing
                uint64_t _change = genrand64_int64();
                _change = _change % (unsigned long long) std::pow(2, gate_count);
                std::cout << _change << " " << (unsigned long long) std::pow(2, gate_count) << std::endl;
                _funcVector[_change] = !_funcVector[_change];
                //

                test = new(std::nothrow) Test(gate_count,
                                              _funcVector,
                                              _negative,
                                              _permutation,
                                              _dnf,
                                              1);
                delete test;

            } else {
                Test *test;
                test = new(std::nothrow) Test(gate_count,
                                              Test::rngFuncVector(gate_count),
                                              vm.count("allpositive") ? Test::noNegative(gate_count)
                                                                      : Test::rngNegative(gate_count),
                                              vm.count("nopermutation") ? Test::identityPermutation(gate_count)
                                                                        : Test::rngPermutation(gate_count),
                                              vm.count("dnf") ? 1 : 0,
                                              vm.count("onlyone") ? 1 : 0);
                delete test;
            }


            if (vm.count("abc")) {
                std::cout << "optimisation start" << std::endl;
                balanceDD();
                system("python ./src/translate.py ./out/TestA.v");
                system("python ./src/translate.py ./out/TestB.v");
            }

            if (vm.count("movetotests")){
                tmpstr = basestr + "TestA" + std::to_string(localconf) + ".v";
                tmpstr = "mv -f ./out/TestA.v " + tmpstr;
                system(tmpstr.c_str());
                tmpstr = basestr + "TestB" + std::to_string(localconf) + ".v";
                tmpstr = "mv -f ./out/TestB.v " + tmpstr;
                system(tmpstr.c_str());
                ++localconf;
            }
        }


        tmpstr = basestr+"conf.txt";
        std::ofstream confout (tmpstr.c_str());
        confout<<localconf;
        confout.close();

        std::cout << time(0) - now<<std::endl;
    }
    catch(std::exception& e)
    {
        std::cerr << "Unhandled Exception reached the top of main: "
                  << e.what() << ", application will now exit" << std::endl;
        return ERROR_UNHANDLED_EXCEPTION;

    }



    return SUCCESS;

}


//cec ../BooleanMatching/TestGenerator/out/TestA.v ../BooleanMatching/TestGenerator/out/TestB.v