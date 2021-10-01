//============================================================================
// Name        : cache_memory.cpp
// Author      : Shorouk Abdelhalim
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "functions.h"


int main(int argc, char *argv[]) {
// Variables definitions

	char command 			= 'n';
	long int address 		= 0;
	int block_size 			= 0;
	int cache_size 			= 0;
	char write_policy 		= 't';
	int associativity 		= 0;
	char replacement 		= 'l';
	float hit_rate;
	float miss_rate;
	const char* trace_file 	= "../ex_trace.trc";
	char line[20];
	string arg;


// get and read arguments

	 if (argc < 9) {
	        show_usage(argv[0]);
	        return 1;
	    }

	 for (int i = 1; i < argc; ++i) {
		 arg = argv[i];
	     if ((arg == "-h") || (arg == "--help")) {
	    	 show_usage(argv[0]);
	         return 0;
	     }else if ((arg == "-b") || (arg == "--blocksize")){
	        i++;
	        block_size = atoi(argv[i]);
	     }else if ((arg == "-c") || (arg == "--cachesize")){
	        i++;
	        cache_size = atoi(argv[i]);
	     }else if ((arg == "-w") || (arg == "--writepolicy")){
	    	i++;
	        write_policy = *argv[i];
	        if (!((write_policy == 't') || (write_policy == 'b') || (write_policy == 'T'))){
	        	cout <<"Write policy argument is not valid"<<"\n";
	        	return 1;
	        	}
	     }else if ((arg == "-a") || (arg == "--associativity")){
	        i++;
	        associativity = atoi(argv[i]);
	     }else if ((arg == "-r") || (arg == "--replacement")){
	        i++;
	        replacement = *argv[i]; // Increment 'i' so we don't get the argument as the next argv[i].
	        if (!((replacement == 'l') || (replacement == 'f') || (replacement =='r'))){
	        	cout << "Replacement policy is not valid"<< "\n";
	        	return 1;
	        }
	     }
	 }


// create and initiate cache simulator object
	 cache cache_sim(block_size, cache_size, associativity, write_policy);
	 cache_sim.print_cache_info();




// open transactions file , read line by line and send command to cache based on the replacement policy

	 FILE * read_file;
	 read_file = fopen (trace_file , "r");

	 if (read_file == NULL){
		 cout <<"Cannot read the trace file\n";
		 return 1;
	 }
	while (fgets (line, 20 , read_file) != NULL) {
		command = line[0];
		char *addr = &line[2];
		address = strtol (addr,NULL,16);

// call cache access function based on the replacement policy
		if (replacement == 'l'){
		    cache_sim.LRU_cache_access(command,address);
		} else if (replacement == 'f'){
		    cache_sim.FIFO_cache_access(command, address);
		} else if (replacement == 'r'){
		    cache_sim.Random_cache_access(command, address);
		}

	}


// append to output file statistics of the trace
	 hit_rate = float(cache_sim.hits)/cache_sim.nref;
	 miss_rate = float(cache_sim.misses)/cache_sim.nref;


	 FILE * pFile;
	 pFile = fopen (cache_sim.file_name,"a");
	 if (pFile!=NULL)
	 {
		 fprintf (pFile,"\n");
	     fprintf (pFile, "nref=%d, nread=%d, nwrite=%d\n",cache_sim.nref,cache_sim.nread,cache_sim.nwrite);
	     fprintf (pFile, "	hits = %d, hit rate = %.3f\n",cache_sim.hits,hit_rate);
	     fprintf (pFile, "	misses = %d, miss rate = %.3f\n",cache_sim.misses,1-hit_rate);
	     fprintf (pFile, "	main memory reads=%d, main memory writes=%d\n",cache_sim.main_read,cache_sim.main_write);
	     fclose (pFile);
	 } else {
	     	cout <<"Cannot open file for cache tracing statistics outputs/n";
	     	return 1;
	 }




	return 0;
}
