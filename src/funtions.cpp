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
         << "\t-w,--writepolicy  \tt for write_through & b for write_back\n"
         << "\t-a,--associativity\tNumber of ways in each associative set\n"
         << "\t[-r,--replacement]  \tl for least recently used, r for random & f for first-in first-out\n"
         << "\n";
}

cache :: cache (int block, int cache, int associat, char write){
		block_size = block;
		cache_size = cache;
		associativity = associat;
		write_policy = write;
		num_entries = cache_size / block_size / associativity;
		offset_bits_num = log2(block_size);
		entry_bits_num = log2(num_entries);
		tag_bits_num = 26 - entry_bits_num - offset_bits_num;
		remove(file_name);
		start_time = std::chrono::high_resolution_clock::now();


	}
int cache :: print_cache_info (void){

	ofstream myfile;
    myfile.open(file_name,myfile.out | myfile.app);

    myfile<<cache_size/1024<<"KB "<<associativity<<"-way associative cache:\n";
    myfile <<"\tBlock size = "<<block_size<<" bytes\n";
    myfile<<"\tNumber of [sets,blocks] = ["<<cache_size/block_size/associativity<<","<<cache_size/block_size<<"]\n";
    myfile<<"\tExtra space for tag storage = "<<"\n";
    myfile<<"\tBits for [tag,index,offset] = ["<<tag_bits_num<<","<<entry_bits_num<<","<<offset_bits_num<<"] = 26\n";
    if(write_policy == 't') myfile<<"\tWrite policy = Write-through \n\n";
    else myfile<<"\tWrite policy = Write-back \n\n";

    myfile<<"Hex 	Binary Address                      Set   Blk 			Memory\n";
    myfile<<"Address (tag/index/offset) 		Tag Index off    Way UWay    Read Write\n";
    myfile<<"======= ========================== ======== ===== === ==== ==== ==== =========\n";

		cout << "Block size "<<block_size<<"\n";
		cout << "Cache size "<<cache_size<<"\n";
		cout << "Number of Entries "<<num_entries<<"\n";
		cout << "Write policy "<<write_policy<<"\n";
		cout << "Number of ways "<<associativity<<"\n";
		return 0;
	}

int cache :: LRU_cache_access(char cmd, long int addr){
	nref++;
    command = cmd;
    address = addr;

    if (command == 'R') nread++;
    else nwrite++;
    string r;
    bool hit = false;
    int way = -1;
    int uway = -1;
    bool dirty_flag = false;

    r = bitset< 26 >( address ).to_string();
    offset_bits = r.substr (tag_bits_num+entry_bits_num,offset_bits_num);
    entry_bits = r.substr (tag_bits_num,entry_bits_num);
    tag_bits = r.substr (0,tag_bits_num);

    ofstream myfile;
    myfile.open(file_name,myfile.out | myfile.app);
    myfile << setw(5)<<hex<< address<<""<<command<<" "<<tag_bits<<" "<<entry_bits<<" "<<offset_bits<<" "<<setw(5)<<dec<<stoi(tag_bits,0,2)<<" "<<setw(5)<<dec<<stoi(entry_bits,0,2)<<" "<<setw(5)<<dec<<stoi(offset_bits,0,2)<<" ";

    struct cache_element cache_item;
    	cache_item.last_used_time = -1;
    	cache_item.cache_line_vector.resize(block_size, -1);

    	static vector<vector< struct cache_element > > cache_object (associativity,vector < struct cache_element> (num_entries,  cache_item));
    	static vector<vector< bool > > dirty (associativity,vector < bool> (num_entries,false));


    	long int cache_line_index, base, cache_index;
    	base = address / block_size;
    	cache_index = base % num_entries;
    	cache_line_index = address % block_size;

    	for(unsigned int i = 0; i < cache_object.size(); i++){

    		if(cache_object[i][cache_index].cache_line_vector[cache_line_index] == address){
    			hits++;
    			//Cache hit -- update last_used_time

    			cache_object[i][cache_index].last_used_time = chrono::duration<long double, milli> (chrono::high_resolution_clock::now() - start_time).count();
    				hit = true;
    				way = i;
    				if (command == 'W') dirty[i][cache_index] = true;

    				break;
    		}
    	}

    	//Procedure for cache miss --- cache hit took care immediately after realize it is a hit
    	if(!hit){

    		misses++;


    		double min = 99999999999999;
    		int victim_index = -1;

    		//Looking for the right slot to replace

    		for(unsigned int i = 0; i < cache_object.size(); i++){
    			if(cache_object[i][cache_index].last_used_time < min){
    				min = cache_object[i][cache_index].last_used_time;
    				victim_index = i;
    			}
    		}


    		//replacing element at victim index
    		cache_object[victim_index][cache_index].last_used_time = chrono::duration<long double, std::milli> (chrono::high_resolution_clock::now() - start_time).count();
    		dirty_flag = dirty[victim_index][cache_index];
    		if (command == 'W') dirty[victim_index][cache_index] = true;
    		else dirty[victim_index][cache_index] = false;
    		for(unsigned int i = 0; i < cache_object[victim_index][cache_index].cache_line_vector.size(); i++){
    			cache_object[victim_index][cache_index].cache_line_vector[i] = base * block_size + i;
    			uway = victim_index;
    		}

    	}

    	if (!hit) myfile<< setw(5)<<"-1"<< " "<<setw(5)<<uway<<" ";
    	else myfile<<setw(5)<<way<<" "<<setw(5)<<"-2"<<" ";

    	if (write_policy == 't'){
    		if (command == 'R'){
    			if (hit) {myfile <<setw(5)<<'0' <<" "<<setw(5)<<'0'<<"\n";}
    			else {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'0'<<"\n"; main_read++;}
    		}else if (command == 'W'){
    			if(hit) {myfile <<setw(5)<<'0'<<" "<<setw(5)<<'1'<<"\n"; main_write++;}
    			else {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'1'<<"\n"; main_write++; main_read++;}
    		}

    	}else if (write_policy == 'b'){
    		if (command == 'R'){
    			if (hit) {myfile <<setw(5)<<'0'<<" "<<setw(5)<<'0'<<"\n";}
    			else if (dirty_flag) {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'1'<<"\n"; main_read++;}
    			else {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'0'<<"\n"; main_read++;}
    		} else if (command == 'W'){
    			if (hit) {myfile <<setw(5)<<'0'<<" "<<setw(5)<<'0'<<"\n";}
    			else if (dirty_flag) {myfile<<setw(5)<<'1'<<" "<<setw(5)<<'1'<<"\n"; main_write++;}
    			else {myfile<<setw(5)<<'1'<<" "<<setw(5)<<'0'<<"\n";}
    		}
    	}

    	myfile.close();


    return 0;
}

int cache :: FIFO_cache_access( char cmd, long int addr){

	nref++;
    command = cmd;
    address = addr;

	long int cache_line_index, base, cache_index;
	base = address / block_size;
	cache_index = base % num_entries;
	cache_line_index = address % block_size;

    if (command == 'R') nread++;
    else nwrite++;
    string r;
    bool hit = false;
    int way = -1;
    int uway = -1;
    bool dirty_flag = false;
    static vector<int> fifo_count (num_entries,-1);
	int victim_index = -1;

    r = bitset< 26 >( address ).to_string();
    offset_bits = r.substr (tag_bits_num+entry_bits_num,offset_bits_num);
    entry_bits = r.substr (tag_bits_num,entry_bits_num);
    tag_bits = r.substr (0,tag_bits_num);

    ofstream myfile;
    myfile.open(file_name,myfile.out | myfile.app);
    myfile << setw(5)<<hex<< address<<""<<command<<" "<<tag_bits<<" "<<entry_bits<<" "<<offset_bits<<" "<<setw(5)<<dec<<stoi(tag_bits,0,2)<<" "<<setw(5)<<dec<<stoi(entry_bits,0,2)<<" "<<setw(5)<<dec<<stoi(offset_bits,0,2)<<" ";

    struct cache_element cache_item;
    	cache_item.last_used_time = -1;
    	cache_item.cache_line_vector.resize(block_size, -1);

    	static vector<vector< struct cache_element > > cache_object (associativity,vector < struct cache_element> (num_entries,  cache_item));
    	static vector<vector< bool > > dirty (associativity,vector < bool> (num_entries,false));



    	for(unsigned int i = 0; i < cache_object.size(); i++){

    		if(cache_object[i][cache_index].cache_line_vector[cache_line_index] == address){
    			hits++;
    			//Cache hit -- update last_used_time

    			cache_object[i][cache_index].last_used_time = chrono::duration<long double, milli> (chrono::high_resolution_clock::now() - start_time).count();
    				hit = true;
    				way = i;
    				if (command == 'W') dirty[i][cache_index] = true;

    				break;
    		}
    	}

    	//Procedure for cache miss --- cache hit took care immediately after realize it is a hit
    	if(!hit){

    		misses++;

    		//Looking for the right slot to replace

    		fifo_count [cache_index] ++;
    		if (fifo_count [cache_index] == associativity) fifo_count [cache_index] = 0;
    		victim_index = fifo_count [cache_index];
    		cout<< "replaced way"<<victim_index<<"\n";




    		//replacing element at victim index

    		cache_object[victim_index][cache_index].last_used_time = chrono::duration<long double, std::milli> (chrono::high_resolution_clock::now() - start_time).count();
    		dirty_flag = dirty[victim_index][cache_index];
    		if (command == 'W') dirty[victim_index][cache_index] = true;
    		else dirty[victim_index][cache_index] = false;

    		for(unsigned int i = 0; i < cache_object[victim_index][cache_index].cache_line_vector.size(); i++){
    			cache_object[victim_index][cache_index].cache_line_vector[i] = base * block_size + i;
    			uway = victim_index;
    		}

    	}

    	if (!hit) myfile<< setw(5)<<"-1"<< " "<<setw(5)<<uway<<" ";
    	else myfile<<setw(5)<<way<<" "<<setw(5)<<"-1"<<" ";

    	if (write_policy == 't'){
    		if (command == 'R'){
    			if (hit) {myfile <<setw(5)<<'0' <<" "<<setw(5)<<'0'<<"\n";}
    			else {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'0'<<"\n"; main_read++;}
    		}else if (command == 'W'){
    			if(hit) {myfile <<setw(5)<<'0'<<" "<<setw(5)<<'1'<<"\n"; main_write++;}
    			else {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'1'<<"\n"; main_write++; main_read++;}
    		}

    	}else if (write_policy == 'b'){
    		if (command == 'R'){
    			if (hit) {myfile <<setw(5)<<'0'<<" "<<setw(5)<<'0'<<"\n";}
    			else if (dirty_flag) {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'1'<<"\n"; main_read++;}
    			else {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'0'<<"\n"; main_read++;}
    		} else if (command == 'W'){
    			if (hit) {myfile <<setw(5)<<'0'<<" "<<setw(5)<<'0'<<"\n";}
    			else if (dirty_flag) {myfile<<setw(5)<<'1'<<" "<<setw(5)<<'1'<<"\n"; main_write++;}
    			else {myfile<<setw(5)<<'1'<<" "<<setw(5)<<'0'<<"\n";}
    		}
    	}

    	myfile.close();

	return 0;
}
int cache :: Random_cache_access(char cmd, long int addr){

	nref++;
    command = cmd;
    address = addr;

    if (command == 'R') nread++;
    else nwrite++;
    string r;
    bool hit = false;
    int way = -1;
    int uway = -1;
    bool dirty_flag = false;

    r = bitset< 26 >( address ).to_string();
    offset_bits = r.substr (tag_bits_num+entry_bits_num,offset_bits_num);
    entry_bits = r.substr (tag_bits_num,entry_bits_num);
    tag_bits = r.substr (0,tag_bits_num);

    ofstream myfile;
    myfile.open(file_name,myfile.out | myfile.app);
    myfile << setw(5)<<hex<< address<<""<<command<<" "<<tag_bits<<" "<<entry_bits<<" "<<offset_bits<<" "<<setw(5)<<dec<<stoi(tag_bits,0,2)<<" "<<setw(5)<<dec<<stoi(entry_bits,0,2)<<" "<<setw(5)<<dec<<stoi(offset_bits,0,2)<<" ";

    struct cache_element cache_item;
    	cache_item.last_used_time = -1;
    	cache_item.cache_line_vector.resize(block_size, -1);

    	static vector<vector< struct cache_element > > cache_object (associativity,vector < struct cache_element> (num_entries,  cache_item));
    	static vector<vector< bool > > dirty (associativity,vector < bool> (num_entries,false));


    	long int cache_line_index, base, cache_index;
    	base = address / block_size;
    	cache_index = base % num_entries;
    	cache_line_index = address % block_size;

    	for(unsigned int i = 0; i < cache_object.size(); i++){

    		if(cache_object[i][cache_index].cache_line_vector[cache_line_index] == address){
    			hits++;
    			//Cache hit -- update last_used_time

    			cache_object[i][cache_index].last_used_time = chrono::duration<long double, milli> (chrono::high_resolution_clock::now() - start_time).count();
    				hit = true;
    				way = i;
    				if (command == 'W') dirty[i][cache_index] = true;

    				break;
    		}
    	}

    	//Procedure for cache miss --- cache hit took care immediately after realize it is a hit
    	if(!hit){

    		misses++;


    		int victim_index = -1;

    		//Looking for the right slot to replace
/*
    		for(unsigned int i = 0; i < cache_object.size(); i++){
    			if(cache_object[i][cache_index].last_used_time < min){
    				min = cache_object[i][cache_index].last_used_time;
    				victim_index = i;
    			}
    		}*/

    		victim_index = 0 + (rand() * (int)(cache_object.size() -1) / RAND_MAX);
    		cout << "uway "<<victim_index <<"cache entries "<<cache_object.size()<<"\n";


    		//replacing element at victim index
    		cache_object[victim_index][cache_index].last_used_time = chrono::duration<long double, std::milli> (chrono::high_resolution_clock::now() - start_time).count();
    		dirty_flag = dirty[victim_index][cache_index];
    		if (command == 'W') dirty[victim_index][cache_index] = true;
    		else dirty[victim_index][cache_index] = false;
    		for(unsigned int i = 0; i < cache_object[victim_index][cache_index].cache_line_vector.size(); i++){
    			cache_object[victim_index][cache_index].cache_line_vector[i] = base * block_size + i;
    			uway = victim_index;
    		}

    	}

    	if (!hit) myfile<< setw(5)<<"-1"<< " "<<setw(5)<<uway<<" ";
    	else myfile<<setw(5)<<way<<" "<<setw(5)<<"-1"<<" ";

    	if (write_policy == 't'){
    		if (command == 'R'){
    			if (hit) {myfile <<setw(5)<<'0' <<" "<<setw(5)<<'0'<<"\n";}
    			else {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'0'<<"\n"; main_read++;}
    		}else if (command == 'W'){
    			if(hit) {myfile <<setw(5)<<'0'<<" "<<setw(5)<<'1'<<"\n"; main_write++;}
    			else {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'1'<<"\n"; main_write++; main_read++;}
    		}

    	}else if (write_policy == 'b'){
    		if (command == 'R'){
    			if (hit) {myfile <<setw(5)<<'0'<<" "<<setw(5)<<'0'<<"\n";}
    			else if (dirty_flag) {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'1'<<"\n"; main_read++;}
    			else {myfile <<setw(5)<<'1'<<" "<<setw(5)<<'0'<<"\n"; main_read++;}
    		} else if (command == 'W'){
    			if (hit) {myfile <<setw(5)<<'0'<<" "<<setw(5)<<'0'<<"\n";}
    			else if (dirty_flag) {myfile<<setw(5)<<'1'<<" "<<setw(5)<<'1'<<"\n"; main_write++;}
    			else {myfile<<setw(5)<<'1'<<" "<<setw(5)<<'0'<<"\n";}
    		}
    	}

    	myfile.close();


    return 0;
}






