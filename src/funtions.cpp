#include <iostream>
#include "functions.h"

using namespace std;

void show_usage(string name)
{
    cout << "Usage: " << name << " <option(s)> argument\n"
         << "Options:\n"
         << "\t-h,--help         \tShow this help message.\n"
         << "\t-b,--blocksize    \tData block size\n"
         << "\t-c,--cachesize    \tTotal cache size\n"
         << "\t-w,--writepolicy  \tt for write_through with non-write-allocation, T for write_through with write-allocation & b for write_back with write-allocation\n"
         << "\t-a,--associativity\tNumber of ways in each associative set\n"
         << "\t[-r,--replacement]\tl for least recently used, r for random & f for first-in first-out\n"
         << "\n";
}

cache :: cache (int block, int cache, int associat, char write){
// set cache specs variables
		block_size        = block;
		cache_size        = cache;
		associativity     = associat;
		write_policy      = write;
		num_entries       = cache_size / block_size / associativity;
		offset_bits_num   = log2(block_size);
		entry_bits_num    = log2(num_entries);
		tag_bits_num      = (24 - entry_bits_num )- offset_bits_num;
		start_time        = std::chrono::high_resolution_clock::now();
// remove old output file
		remove(file_name);
	}
int cache :: print_cache_info (void){

	int sets   					= cache_size/block_size/associativity;
	int blocks 					= cache_size/block_size;
	int extra_space 			= ((tag_bits_num ) * blocks) / 8 ;
	float extra_space_percent	= (float(extra_space) / cache_size) * 100;

// craft cache size in terms of B, KB

	int cache_size_value;
	string cache_size_unit;
	if (cache_size < 1024) {
		cache_size_value = cache_size;
		cache_size_unit= " B";
	}else {
		cache_size_value = cache_size/1024;
		cache_size_unit = "KB";
	}


// craft write policy name string
	string write_policy_name;
	if (write_policy == 't') {
		write_policy_name = "Write-through with non-write allocation";
	}
	else if (write_policy == 'T') {
		write_policy_name = "Write-through with write allocation";
	}
	else if (write_policy == 'b') {
		write_policy_name = "Write-back with write allocation" ;
	}


// open file and type cache's specs
	FILE * pFile;
	pFile = fopen (file_name,"w");

	if (pFile!=NULL)
	{
		const char *ccache_size_unit = cache_size_unit.c_str();
		fprintf (pFile, "%d%s %d-way associative cache:\n",cache_size_value,ccache_size_unit,associativity);
		fprintf (pFile, "	Block size = %d bytes\n",block_size);
		fprintf (pFile, "	Number of [sets,blocks] = [%d,%d]\n",sets,blocks);
		fprintf (pFile, "	Extra space for tag storage = %d bytes ( %.2f%%)\n",extra_space,extra_space_percent);
		fprintf (pFile, "	Bits for [tag,index,offset] = [%d, %d, %d] = 24\n",tag_bits_num,entry_bits_num,offset_bits_num);
		const char *cwrite_policy_name = write_policy_name.c_str();
		fprintf (pFile, "	Write policy = %s\n\n",cwrite_policy_name);
		fprintf (pFile, "Hex 	Binary Address                      Set   Blk 		  Memory\n");
		fprintf (pFile, "Address (tag/index/offset)             Tag Index off   Way UWay  Read Write\n");
		fprintf (pFile, "======= ========================== ======== ===== === ==== ==== ==== ====\n");

	    fclose (pFile);
	} else {
		cout <<"Cannot open file for cache specs outputs/n";
		return 1;
	}

	cout << "Please, find ex_output.out.txt file for the output"<<"\n";
	return 0;
}

int cache :: LRU_cache_access(char cmd, long int addr){

// init variables for LRU replacement policy accessing cache
	bool hit					= false;
	int way 					= -1;
	int uway 					= -1;
	int main_memory_read 		= 0;
	int main_memory_write 		= 0;
	bool dirty_flag 			= false;
	long int cache_line_index, base, cache_index;
	double min = 99999999999999;
	int victim_index = -1;
	string r;
	main_memory_read = 0;
	main_memory_write = 0;



// set variables for cache memory access
	nref++;
    command = cmd;
    address = addr;

	base = address / block_size;
	cache_index = base % num_entries;
	cache_line_index = address % block_size;

	r = bitset< 24 >( address ).to_string();
	offset_bits = r.substr (tag_bits_num+entry_bits_num,offset_bits_num);
	entry_bits = r.substr (tag_bits_num,entry_bits_num);
	tag_bits = r.substr (0,tag_bits_num);


    if (command == 'R') nread++;
    else nwrite++;



// create structure cache_element for every cache block, contains address valuse, last time called and dirty bit
    struct cache_element cache_item;
    cache_item.last_used_time = -1;
    cache_item.cache_line_vector.resize(block_size, -1);
    cache_item.dirty = false;

// create vector for cache object based on associativity and number of entries
    static vector<vector< struct cache_element > > cache_object (associativity,vector < struct cache_element> (num_entries,  cache_item));

// check availability of the required block inside cache
    	for(unsigned int i = 0; i < cache_object.size(); i++){

    		if(cache_object[i][cache_index].cache_line_vector[cache_line_index] == address){
    			hits++;

    			//Cache hit -- update last_used_time
    			cache_object[i][cache_index].last_used_time = chrono::duration<long double, milli> (chrono::high_resolution_clock::now() - start_time).count();
    			hit = true;
    			way = i;
    			uway = -2;
    			main_memory_read = 0;
    			main_memory_write = 0;
    			if (command == 'w'){
    				cache_object[i][cache_index].dirty = true;
    				if (write_policy == 't' || write_policy == 'T') main_memory_write ++;
    				main_write++;
    			}

    		    break;
    		}
    	}

//Procedure for cache miss
    	if(!hit){

    		misses++;
    		way = -1;


    		if (command == 'W' && write_policy == 't'){
    			way = -1;
    			uway = -1;
    			main_memory_write++;
    			main_memory_read= 0;
    			main_write++;
    		} else {

//Looking for the right slot to replace
    			for(unsigned int i = 0; i < cache_object.size(); i++){
    				if(cache_object[i][cache_index].last_used_time < min){
    			    	min = cache_object[i][cache_index].last_used_time;
    			    	victim_index = i;
    			    	way = -1;
    			    	uway = victim_index;
    			    }
    			}

//replacing element at victim index block
    			cache_object[victim_index][cache_index].last_used_time = chrono::duration<long double, std::milli> (chrono::high_resolution_clock::now() - start_time).count();
    	    	dirty_flag = cache_object[victim_index][cache_index].dirty;

    	    	for(unsigned int i = 0; i < cache_object[victim_index][cache_index].cache_line_vector.size(); i++){
    	    	    cache_object[victim_index][cache_index].cache_line_vector[i] = base * block_size + i;

    	    	}


// In case of write command, update the dirty bit
    	    	if (command == 'W') cache_object[victim_index][cache_index].dirty = true;
    	    	else cache_object[victim_index][cache_index].dirty = false;

 //handling conditions for main memory readxwrite commands
	    		if (dirty_flag == true && write_policy == 'b'){
	    			main_memory_write++;
	    			main_write++;
	    		}

    	    	if(command == 'R') {
    	    	   main_memory_read++;
    	    	   main_read++;
    	    	}else {
    	    		if (write_policy == 'T'){
    	    	    main_memory_write++;
    	    	    main_write++;
    	    	    main_memory_read++;
    	    	    main_read++;
    	    		}
    	    		if (write_policy == 'b'){
    	    			main_memory_read++;
    	    			main_read++;
    	    		}
    	    	}

    		}
    }

// open file and type cache's specs
    		FILE * pFile;
    		pFile = fopen (file_name,"a");
    		if (pFile!=NULL)
    		{
    			string address_line = tag_bits+" "+ entry_bits+" "+offset_bits;
    			const char *caddress_line = address_line.c_str();
    			fprintf (pFile, "%6x%c %26s %8d %5d %3d %4d %4d %4d %4d\n",address,command, caddress_line, stoi(tag_bits,0,2), cache_index,cache_line_index,way,uway,main_memory_read,main_memory_write);
    		    fclose (pFile);
    		} else {
    			cout <<"Cannot open file for cache tracing outputs/n";
    			return 1;
    		}

    return 0;
}

int cache :: FIFO_cache_access( char cmd, long int addr){

	nref++;
    command = cmd;
    address = addr;


	bool hit = false;
	int way = -1;
	int uway = -1;
	bool dirty_flag = false;
	static vector<int> fifo_count (num_entries,-1);
	int victim_index = -1;
	int main_memory_read 		= 0;
	int main_memory_write 		= 0;



	long int cache_line_index, base, cache_index;
	base = address / block_size;
	cache_index = base % num_entries;
	cache_line_index = address % block_size;


	string r;
	r = bitset< 24 >( address ).to_string();
	offset_bits = r.substr (tag_bits_num+entry_bits_num,offset_bits_num);
	entry_bits = r.substr (tag_bits_num,entry_bits_num);
	tag_bits = r.substr (0,tag_bits_num);


    if (command == 'R') nread++;
    else nwrite++;

    struct cache_element cache_item;
    cache_item.last_used_time = -1;
    cache_item.cache_line_vector.resize(block_size, -1);
    cache_item.dirty = false;

    static vector<vector< struct cache_element > > cache_object (associativity,vector < struct cache_element> (num_entries,  cache_item));



    for(unsigned int i = 0; i < cache_object.size(); i++){

    	if(cache_object[i][cache_index].cache_line_vector[cache_line_index] == address){

    		hits++;
//Cache hit -- update last_used_time

    		cache_object[i][cache_index].last_used_time = chrono::duration<long double, milli> (chrono::high_resolution_clock::now() - start_time).count();
    		hit = true;
    		way = i;
    		uway = -1;
    		main_memory_read = 0;
    		main_memory_write = 0;
    		if (command == 'w'){
    			cache_object[i][cache_index].dirty = true;
    			if (write_policy == 't' || write_policy == 'T') main_memory_write ++;
    			main_write++;
    		}
    		break;
    	}
    }

//Procedure for cache miss --- cache hit took care immediately after realize it is a hit
    if(!hit){

    	misses++;
    	way =-1;

    	if (command == 'W' && write_policy == 't'){
    		way = -1;
    		uway = -1;
    		main_memory_write++;
    		main_memory_read= 0;
    		main_write++;
    	} else {

//Looking for the right slot to replace

    		fifo_count [cache_index] ++;
    		if (fifo_count [cache_index] == associativity) fifo_count [cache_index] = 0;
    		victim_index = fifo_count [cache_index];
    		uway = victim_index;

//replacing element at victim index

    		cache_object[victim_index][cache_index].last_used_time = chrono::duration<long double, std::milli> (chrono::high_resolution_clock::now() - start_time).count();
    		dirty_flag = cache_object[victim_index][cache_index].dirty;

    		if (command == 'W') cache_object[victim_index][cache_index].dirty = true;
    		else cache_object[victim_index][cache_index].dirty = false;

    		for(unsigned int i = 0; i < cache_object[victim_index][cache_index].cache_line_vector.size(); i++){
    			cache_object[victim_index][cache_index].cache_line_vector[i] = base * block_size + i;
    		}

//handling conditions for main memory readxwrite commands
 	    	if (dirty_flag == true && write_policy == 'b'){
 	    		main_memory_write++;
 	    		main_write++;
 	    	}

     	    if(command == 'R') {
     	    	main_memory_read++;
     	    	main_read++;
     	    }else {
     	    	if (write_policy == 'T'){
     	    		main_memory_write++;
     	    	    main_write++;
     	    	    main_memory_read++;
     	    	    main_read++;
     	    	}
     	    	if (write_policy == 'b'){
     	    		main_memory_read++;
     	    		main_read++;
     	    	}
     	    }
    	}
    }

 // open file and type cache's specs
    FILE * pFile;
    pFile = fopen (file_name,"a");
    if (pFile!=NULL){
    	string address_line = tag_bits+" "+ entry_bits+" "+offset_bits;
    	const char *caddress_line = address_line.c_str();
    	fprintf (pFile, "%6x%c %26s %8d %5d %3d %4d %4d %4d %4d\n",address,command, caddress_line, stoi(tag_bits,0,2), cache_index,cache_line_index,way,uway,main_memory_read,main_memory_write);
    	fclose (pFile);
    } else {
    	cout <<"Cannot open file for cache tracing outputs/n";
    	return 1;
    }

	return 0;
}

int cache :: Random_cache_access(char cmd, long int addr){

	nref++;
    command = cmd;
    address = addr;

    bool hit = false;
    int way = -1;
    int uway = -1;
    bool dirty_flag = false;
    int main_memory_read 		= 0;
    int main_memory_write 		= 0;


	long int cache_line_index, base, cache_index;
	base = address / block_size;
	cache_index = base % num_entries;
	cache_line_index = address % block_size;

	string r;
	r = bitset< 24 >( address ).to_string();
	offset_bits = r.substr (tag_bits_num+entry_bits_num,offset_bits_num);
	entry_bits = r.substr (tag_bits_num,entry_bits_num);
	tag_bits = r.substr (0,tag_bits_num);



    if (command == 'R') nread++;
    else nwrite++;


    struct cache_element cache_item;
    	cache_item.last_used_time = -1;
    	cache_item.cache_line_vector.resize(block_size, -1);
        cache_item.dirty = false;

    static vector<vector< struct cache_element > > cache_object (associativity,vector < struct cache_element> (num_entries,  cache_item));



    for(unsigned int i = 0; i < cache_object.size(); i++){

    	if(cache_object[i][cache_index].cache_line_vector[cache_line_index] == address){
    		hits++;
//Cache hit -- update last_used_time

    		cache_object[i][cache_index].last_used_time = chrono::duration<long double, milli> (chrono::high_resolution_clock::now() - start_time).count();
    		hit = true;
    		way = i;
    		uway = -2;
    		main_memory_read = 0;
    		main_memory_write = 0;
    		if (command == 'w'){
    			 cache_object[i][cache_index].dirty = true;
    			 if (write_policy == 't' || write_policy == 'T') main_memory_write ++;
    			 main_write++;
    		}

    		break;
    	}
    }

//Procedure for cache miss --- cache hit took care immediately after realize it is a hit
    if(!hit){

    	misses++;
    	way =-1;

    	if (command == 'W' && write_policy == 't'){
    		way = -1;
    		uway = -1;
    		main_memory_write++;
    		main_memory_read= 0;
    		main_write++;
    	} else {
    		int victim_index = -1;

//Looking for the right slot to replace

    		victim_index = 0 + (rand() * (int)(cache_object.size() -1) / RAND_MAX);


//replacing element at victim index
    		cache_object[victim_index][cache_index].last_used_time = chrono::duration<long double, std::milli> (chrono::high_resolution_clock::now() - start_time).count();
    		dirty_flag = cache_object[victim_index][cache_index].dirty;
    		if (command == 'W') cache_object[victim_index][cache_index].dirty = true;
    		else cache_object[victim_index][cache_index].dirty = false;
    		for(unsigned int i = 0; i < cache_object[victim_index][cache_index].cache_line_vector.size(); i++){
    			cache_object[victim_index][cache_index].cache_line_vector[i] = base * block_size + i;
    			uway = victim_index;
    		}

//handling conditions for main memory readxwrite commands
    		if (dirty_flag == true && write_policy == 'b'){
    		    main_memory_write++;
    		    main_write++;
    		}

    	    if(command == 'R') {
    	    	main_memory_read++;
    	    	main_read++;
    	    }else {
    	    	if (write_policy == 'T'){
    	    	   main_memory_write++;
    	    	   main_write++;
    	    	   main_memory_read++;
    	    	   main_read++;
    	    	}
    	    	if (write_policy == 'b'){
    	    	    main_memory_read++;
    	    	    main_read++;
    	    	 }
    	   }
    	}
    }

// open file and type cache's specs
    FILE * pFile;
    pFile = fopen (file_name,"a");
    if (pFile!=NULL){
        string address_line = tag_bits+" "+ entry_bits+" "+offset_bits;
        const char *caddress_line = address_line.c_str();
        fprintf (pFile, "%6x%c %26s %8d %5d %3d %4d %4d %4d %4d\n",address,command, caddress_line, stoi(tag_bits,0,2), cache_index,cache_line_index,way,uway,main_memory_read,main_memory_write);
        fclose (pFile);
      } else {
        cout <<"Cannot open file for cache tracing outputs/n";
        return 1;
     }

    return 0;
}






