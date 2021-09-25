#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>
#include <bitset>

using namespace std;

void show_usage(string);

class cache {
protected:
	int block_size = -1;
	int cache_size = -1;
	int num_entries = -1;
	int associativity = -1;
	int num_sets = -1;
	char write_policy = ' ';
	char command;
	long int address;
	int nref = 0;
	int nread = 0;
	int nwrite = 0;
	int hits = 0;
	int misses = 0;
	int main_read = 0;
	int main_write = 0;
	int offset_bits_num = 0;
	int entry_bits_num = 0;
	int tag_bits_num = 0;
	string offset_bits;
	string entry_bits;
	string tag_bits;
	const char* file_name = "../output.txt";


public:
	cache (int block, int cache, int associat, char write);
	int print_cache_info (void);
	int LRU_cache_access(char cmd, long int address);
	int FIFO_cache_access(char cmd, long int address);
	int Random_cache_access(char cmd, long int address);

private:
	int direct_mapping(void);
	int set_associative_lru(void);
	int full_associative_lru(void);

};
