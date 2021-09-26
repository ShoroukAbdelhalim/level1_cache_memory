//============================================================================
// Name        : cache_memory.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "functions.h"


int main(int argc, char *argv[]) {
	// Variables definitions

	string line;
	string fn;
	string arg;
	string cmd;
	string addr;
	char command;
	long int address;
	int block_size = 0;
	int cache_size = 0;
	char write_policy;
	int associativity = 0;
	char replacement = 'l';
	float hit_rate;
	float miss_rate;

	 if (argc < 9) {
	        show_usage(argv[0]);
	        return 1;
	    }

	    for (int i = 1; i < argc; ++i) {
	        arg = argv[i];

	        if ((arg == "-h") || (arg == "--help")) {
	            show_usage(argv[0]);
	            return 0;
	        } else if ((arg == "-b") || (arg == "--blocksize")){
	        	i++;
	        	block_size = atoi(argv[i]); // Increment 'i' so we don't get the argument as the next argv[i].

	        }else if ((arg == "-c") || (arg == "--cachesize")){
	        	i++;
	        	cache_size = atoi(argv[i]); // Increment 'i' so we don't get the argument as the next argv[i].

	        }else if ((arg == "-w") || (arg == "--writepolicy")){
	        	i++;
	        	write_policy = *argv[i]; // Increment 'i' so we don't get the argument as the next argv[i].
	        	if (!((write_policy == 't') || (write_policy == 'b'))){
	        		cout <<"Write policy argument is not valid"<<"\n";
	        		return 1;
	        	}

	        }else if ((arg == "-a") || (arg == "--associativity")){
	        	i++;
	        	associativity = atoi(argv[i]); // Increment 'i' so we don't get the argument as the next argv[i].

	        }else if ((arg == "-r") || (arg == "--replacement")){
	        	i++;
	        	replacement = *argv[i]; // Increment 'i' so we don't get the argument as the next argv[i].
	        	if (!((replacement == 'l') || (replacement == 'f') || (replacement =='r'))){
	        		cout << "Replacement policy is not valid"<< "\n";
	        		return 1;

	        	}

	        }
	    }

	    cache cache_sim(block_size, cache_size, associativity, write_policy);
	    cache_sim.print_cache_info();



		// open transactions file

		ifstream rfile("../ex_trace.trc");

		if (!(rfile.is_open())){
			cout << "Transactions File is not opened correctly"<<"\n";
		}

		string token;

		 while (getline(rfile, line)) {
		    cout << line << "\n";
		    stringstream ss;
			ss << line;
			ss >> command >> hex >> address;

		    if (replacement == 'l'){
		    	cache_sim.LRU_cache_access(command,address);
		    } else if (replacement == 'f'){
		    	cache_sim.FIFO_cache_access(command, address);
		    } else if (replacement == 'r'){
		    	cache_sim.Random_cache_access(command, address);
		    }

		  }

		 ofstream myfile;
		 myfile.open(cache_sim.file_name,myfile.out | myfile.app);

		 hit_rate = cache_sim.hits/cache_sim.nref;
		 miss_rate = cache_sim.misses/cache_sim.nref;


		 myfile<<"\n";
		 myfile<<"nref="<<cache_sim.nref<<", nread="<<cache_sim.nread<<", nwrite="<<cache_sim.nwrite<<"\n";
		 myfile <<"\thits = "<<cache_sim.hits<<", hit rate = "<<hit_rate<<"\n";
		 myfile <<"\tmisses = "<<cache_sim.misses<<", miss rate = "<<miss_rate<<"\n";
		 myfile<<"\tmain memory reads="<<cache_sim.main_read<<", main memory write="<<cache_sim.main_write<<"\n";


		rfile.close();
		myfile.close();


	return 0;
}
