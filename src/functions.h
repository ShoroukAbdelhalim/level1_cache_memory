#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>
#include <bitset>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <typeinfo>

using namespace std;

struct cache_element{
	double last_used_time;
	vector<long int> cache_line_vector;
	bool dirty;
};

void show_usage(string);

class cache {

public:
		int block_size = -1;
		int cache_size = -1;
		int num_entries = -1;
		int associativity = -1;
		char write_policy = ' ';
		char command = 'n';
		long int address = -1;
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
		string offset_bits = "N";
		string entry_bits = "N";
		string tag_bits = "N";
		const char* file_name = "../ex_output.out.txt";
		chrono::high_resolution_clock::time_point start_time;
		bool finish_trace = false;


	cache (int block, int cache, int associat, char write);
	int print_cache_info (void);
	int LRU_cache_access(char cmd, long int address);
	int FIFO_cache_access(char cmd, long int address);
	int Random_cache_access(char cmd, long int address);

};
