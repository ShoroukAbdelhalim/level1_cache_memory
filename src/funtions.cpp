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
		num_entries = cache_size / block_size;
		num_sets = cache_size / associativity/block_size;
		offset_bits_num = log2(block_size);
		entry_bits_num = log2(num_entries);
		tag_bits_num = 26 - entry_bits_num - offset_bits_num;
		remove(file_name);

	}
int cache :: print_cache_info (void){

		cout << "Block size "<<block_size<<"\n";
		cout << "Cache size "<<cache_size<<"\n";
		cout << "Number of Entries "<<num_entries<<"\n";
		cout << "Write policy "<<write_policy<<"\n";
		cout << "Number of sets "<<num_sets<<"\n";
		return 0;
	}

int cache :: LRU_cache_access(char cmd, long int addr){
    command = cmd;
    address = addr;
    string r;

    r = bitset< 26 >( address ).to_string();
    offset_bits = r.substr (tag_bits_num+entry_bits_num,offset_bits_num);
    entry_bits = r.substr (tag_bits_num,entry_bits_num);
    tag_bits = r.substr (0,tag_bits_num);

    if (associativity == 1) direct_mapping ();
    else if (associativity == num_entries) full_associative_lru();
    else set_associative_lru();

    return 0;

}
int cache :: FIFO_cache_access( char cmd, long int address){
	return 0;
}
int cache :: Random_cache_access(char cmd, long int address){
	return 0;
}

int cache :: direct_mapping(void){

	ofstream myfile;
	myfile.open(file_name,myfile.out | myfile.app);


		//Initialize cache entries
		vector<vector<long int> > cache_simu (num_entries, vector<long int> (block_size, -1));

		int cache_line_index, base, cache_index;

			base = address / block_size;
			cache_index = base % num_entries;
			cache_line_index = address % block_size;



			//cache missed
			if(cache_simu[cache_index][cache_line_index] != address){
				cout << cache_simu[cache_index][cache_line_index] << " "<<address<<"\n";

				//populate the cache line
				for(int i = 0; i < block_size; i++){
					cache_simu[cache_index][i] = base*block_size+i;
					cout <<base*block_size+i<<"\n";
				}

if (command == 'R') {
	myfile << hex<< address<<""<<'R'<<" "<<tag_bits<<" "<<entry_bits<<" "<<offset_bits<<" "<<dec<<stoi(tag_bits,0,2)<<" "<<dec<<stoi(entry_bits,0,2)<<" "<<dec<<stoi(offset_bits,0,2)<<" "<<"-1"<<" "<<0<<" ";
	myfile << '1'<<" "<<0<<"\n";

}else{
	myfile << hex<< address<<""<<'W'<<" "<<tag_bits<<" "<<entry_bits<<" "<<offset_bits<<" "<<dec<<stoi(tag_bits,0,2)<<" "<<dec<<stoi(entry_bits,0,2)<<" "<<dec<<stoi(offset_bits,0,2)<<" "<<"-1"<<" "<<0<<" ";
	myfile << '0'<<" "<<'1'<<"\n";
}


			}else{
				//cache_hit++;

			}

		myfile.close();
		//return cache_hit;


	return 0;
}
int cache :: set_associative_lru(void){
	return 0;
}
int cache :: full_associative_lru(void){
	return 0;
}






