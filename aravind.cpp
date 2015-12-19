#include <iostream>
#include <string>
#include <sys/stat.h>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <map>


using namespace std;
int asc;
vector<string> comp_cols;
map<string,int> metadata;
map<string,int> map_comp_cols;
map<int,int> map_bytes_comp_cols;



vector<string> get_comp_cols(int argc, char *argv[])
{
	vector<string> vec;
	string temp;
	for(int i=5;i<argc;i++)
	{	
		temp = argv[i];
		vec.push_back(temp);
	}
	return vec;
}

inline bool is_file (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

int check_for_input_errors(int argc, char *argv[])
{
	int err = 0;
	if(argc < 6)
		err = 1;//no of arguments is less
	else if(!is_file(argv[1]))
		err = 2;//input.txt missing
	else if(!is_file("metadata.txt"))
		err = 3;//metadata.txt missing

	//check for all columns given in input whether they actually present
	return err;
}


int get_total_num_of_records(int argc, char* argv[])
{
	ifstream file;
	file.open(argv[1]);
	string a;
	int n = 0;
	while(getline(file, a))
		n++;
	return n;
}

int get_record_size()
{
	ifstream file;
	file.open("metadata.txt");
	char a[20];
	int b, sum=0;
	char * token = NULL;
	while(file >> a)
	{
		token = strtok(a, ",");
		token = strtok(NULL, ",");
		b = atoi(token);
		sum = sum + b;
	}

	sum = sum + (metadata.size()-1)*2 ;
	return sum;
}

int get_no_of_cols()
{
	int n = 0;
	ifstream file;
	file.open("metadata.txt");
	string line;
	while(file >> line)
	{
		n++;
	}
	return n;
}

map<string, int> get_map_of_metadata()
{
	map<string,int> m;

	ifstream file;
	file.open("metadata.txt");
	string line, token, key, value;
	int val;

	while(file >> line)
	{
		stringstream ss(line);
		getline(ss, key, ',');
		getline(ss, value, ',');
		val = atoi(value.c_str());
		m.insert(make_pair(key, val));
	}
	file.close();

	return m;
}

bool comp(const string &a, const string &b)
{
	vector<string> record_a;
	vector<string> record_b;

	string x, y;
	x = "";
	y = "";

	map<int,int>::iterator mp;
	for(mp=map_bytes_comp_cols.begin();mp!=map_bytes_comp_cols.end();mp++)
	{
		x = x + a.substr(mp->first, mp->second);
		y = y + b.substr(mp->first, mp->second);		
	}

	if(asc == 1)
	{
		return (x < y);
	}
	else
		return (x > y);

}

string min(string a, string b)
{
	vector<string> record_a;
	vector<string> record_b;

	string x, y;
	x = "";
	y = "";

	map<int,int>::iterator mp;
	for(mp=map_bytes_comp_cols.begin();mp!=map_bytes_comp_cols.end();mp++)
	{
		if(a.size() > mp->first)
			x = x + a.substr(mp->first, mp->second);
		if(b.size() > mp->first)
			y = y + b.substr(mp->first, mp->second);		
	}

	if(asc == 1)
	{
		if(x < y)
			return a;
		else
			return b;
	}
	else
	{
		if(x < y)
			return b;
		else
			return a;
	}
}

int main(int argc, char *argv[])
{
	int err = check_for_input_errors(argc, argv);
	if(err == 1)
	{
		cout << "ERROR: invalid input" << endl;
		return 0;
	}
	else if(err == 2)
	{
		cout << "ERROR: The input file is missing" << endl;
		return 0;
	}
	else if(err == 3)
	{
		cout << "ERROR: metadata.txt is missing" << endl;
		return 0;
	}
	else 
	{	
		//checking for the asc or desc
		if(strcmp(argv[4], "asc") == 0)
		{
			asc = 1;
		}
		else if(strcmp(argv[4], "desc") == 0)
		{
			asc = 0;
		}
		else
		{
			cout << "ERROR: 'asc' for ascending and 'desc' for descending" << endl;
			return 0;
		}

		
		metadata = get_map_of_metadata();

		string temp;
		for(int i=5;i<argc;i++)
		{	
			temp = argv[i];
			if(metadata.find(temp) == metadata.end())
			{
				cout << "ERROR: check once ,the given columns" << endl;
				return 0;
			}
		}
		comp_cols = get_comp_cols(argc, argv);
		//cout << comp_cols[1] << endl;

		/*for(int i=0;i<comp_cols.size();i++)
		{
			cout << comp_cols.at(i) << endl;
		}*/

		for(int i=0;i<comp_cols.size();i++)
		{
			map_comp_cols.insert(make_pair(comp_cols[i], metadata.at(comp_cols[i])));
		}
		map<string,int>::iterator it;

		int cnt = 0, flag=0;
		for(int i=0;i<comp_cols.size();i++)
		{
			for(it=metadata.begin();it!=metadata.end();it++)
			{	
				//cout << it->first << endl;
				if(strcmp((it->first).c_str(), comp_cols[i].c_str())==0)
				{	
					/*if(flag == 0)
						flag = 1;
					else if(flag != 0)*/
					if(cnt != 0)
						cnt = cnt + 2;
					map_bytes_comp_cols.insert(make_pair(cnt, it->second));
					break;
				}
				cnt = cnt + it->second;
			}

			cnt = 0;
		}

		map<int,int>::iterator mit;
		/*for(mit = map_bytes_comp_cols.begin();mit != map_bytes_comp_cols.end();mit++)
		{
			cout << mit->first << " " << mit->second << endl;
		}*/

		float mem_size = atoi(argv[3]);
		mem_size = mem_size * 1024 * 1024 * 0.8;
		//memory size given as input

		int total_num_of_records = get_total_num_of_records(argc, argv);
		//cout << total_num_of_records << endl;
		//these are the total number of records in input.txt

		int rec_size = get_record_size();
		//cout << rec_size << endl;
		int no_of_records = (mem_size/rec_size);
		//cout << no_of_records << endl;
		//these are the no of records that can fit into main memory

		if(mem_size >= (rec_size+1)*total_num_of_records )
		{
			cout << "ERROR: Two way merge sort not required!!!" << endl;
			return 0;
		}

		int num_of_files = (total_num_of_records/no_of_records) + 1;
		//cout << num_of_files << endl;
		//these are the num of files which we use intermediarily

		int no_of_cols = get_no_of_cols();
		//these are the no of cols metadata.txt contains
		
		vector<string> record;

		string line;
		ifstream ff;
		//ff.open("input.txt");
		ff.open(argv[1]);		
		int m, n;

		int d = 0;
		string name;

		while(ff != NULL)
		{
			getline(ff, line);
			//cout << line << endl;
			record.push_back(line);
			if(record.size() == no_of_records)
			{
				sort(record.begin(), record.end(), comp);

				d++;
				ostringstream conv;
				conv << d;
				name = conv.str() + ".txt";

				ofstream file(name.c_str(), ios_base::app | ios_base::out);
				for(int i=0;i<record.size();i++)
				{
					file << record.at(i) << endl;
				}
				file.close();

				//cout << name << endl;
				//if(d==10)
				//	break;
				record.clear();
			}
		}
		//cout << no_of_records << " " <<  rec_size <<  endl;
		record.pop_back();

		if(!record.empty())
		{
			sort(record.begin(), record.end(), comp);
			d++;
			ostringstream conv;
			conv << d;
			name = conv.str() + ".txt";
			ofstream file(name.c_str(), ios_base::app | ios_base::out);

			for(int i=0;i<record.size();i++)
			{
				file << record.at(i) << endl;
			}
			file.close();
			record.clear();
		}

		/*d++;
		ostringstream conv;
		conv << d;
		name = conv.str() + ".txt";
		ofstream file(name.c_str(), ios_base::app | ios_base::out);
		//file.open(name.c_str());

		for(int i=0;i<record.size()-1;i++)
		{
			file << record.at(i) << endl;
		}
		file.close();
		record.clear();*/

		int block_size = no_of_records/(num_of_files + 1);
		//this is the number of records from each file to take into vector
		//(num_of_files + 1) implies 1 block for output

		vector< vector<string> > mem_sized_vec;
		vector<string> temp_block;
		vector<int> file_line_pointer;
		file_line_pointer.push_back(-1);
		string dummy, dummy_line;

		for(int i=1;i<=num_of_files;i++)
		{
			file_line_pointer.push_back(0);
			ostringstream abc;
			abc << i;
			dummy = abc.str() + ".txt";

			ifstream dummy_pointer;
			dummy_pointer.open(dummy.c_str());


			while(getline(dummy_pointer, dummy_line))
			{
				temp_block.push_back(dummy_line);
				file_line_pointer[i]++;

				if(temp_block.size() == block_size)
				{
					break;
				}
			}
			mem_sized_vec.push_back(temp_block);
			temp_block.clear();
			dummy_pointer.close();
		}
		//cout << num_of_files << " " << file_line_pointer[num_of_files] << endl;

		/*for(int i=0;i<file_line_pointer.size();i++)
		{
			cout << file_line_pointer.at(i) << endl;
		}*/

		//cout << mem_sized_vec.size() << endl;

		//mem_sized_vec[0] ---->  1.txt
		int line_cnt;
		string min_record;
		int min_index;
		vector<string> output;
		/*fstream out_file_pointer;
		out_file_pointer.open("output.txt", ios_base::app);*/

		int cnnt = 0;

		if(is_file(argv[2]))
		{
			remove(argv[2]);
		}

		while(1)
		{
			//this loop fills the completely emptied block with corresponding file records incase file isn't null
			for(int i=0;i<num_of_files;i++)
			{
				if(mem_sized_vec[i].size() == 0)
				{
					ostringstream abc;
					abc << (i+1);
					dummy = abc.str() + ".txt";
					ifstream dummy_pointer;
					dummy_pointer.open(dummy.c_str());

					line_cnt = 0;
					while(getline(dummy_pointer, dummy_line))
					{
						if(temp_block.size() == block_size || file_line_pointer[i+1] == no_of_records)
						{
							break;
						}
						line_cnt++;
						if(line_cnt >= file_line_pointer[i+1] + 1)
						{
							temp_block.push_back(dummy_line);
							//file_line_pointer[i+1]++;
						}
						/*if(file_line_pointer[i+1] == no_of_records && (i+1) != num_of_files )
						{
							file_line_pointer[i+1] = -1;
						}
						else if(i+1 == num_of_files)
						{
							if(total_num_of_records % no_of_records == file_line_pointer[i+1])
							{
								file_line_pointer[i+1] == -1;
							}
						}*/

					}

					file_line_pointer[i+1] = file_line_pointer[i+1] + temp_block.size();
					if(temp_block.size() != 0)
					{

						mem_sized_vec[i] = temp_block;
					}
					temp_block.clear();
					dummy_pointer.close();
				}
				//cout << mem_sized_vec.size() << endl;
			}

			/*for(int i=1;i<=num_of_files;i++)
			{
				cout << file_line_pointer[i] << " ";
			}
			cout << endl;*/

			//if a block is still then (i+1).txt was all read.
			line_cnt = 0;
			min_record = "";
			for(int i=0;i<num_of_files;i++)
			{
				if(mem_sized_vec[i].size() != 0)
				{
					if(line_cnt == 0)
					{
						min_record = mem_sized_vec[i].at(0);
						//cout << min_record << endl;
						min_index = i;
						line_cnt++;
					}
					else
					{
						min_record = min(min_record, mem_sized_vec[i].at(0));
						if(strcmp(mem_sized_vec[min_index].at(0).c_str(), min_record.c_str() ) != 0)
						{
							min_index = i;
						}
					}
				}
			}
			//cout << min_record << endl;

			//if all files were read, then sorting is completed.

			if(strcmp(min_record.c_str(), "")==0)
			{
				break;
			}
			/*int loop_break_flag = 0;
			for(int i=1; i<= num_of_files; i++)
			{
				if(file_line_pointer[i] != no_of_records)
				{
					loop_break_flag = 1;
					break;
				}
			}
			if(loop_break_flag == 0)
			{
				break;
			}*/

			output.push_back(min_record);
			mem_sized_vec[min_index].erase(mem_sized_vec[min_index].begin());
			//file_line_pointer[min_index+1] = file_line_pointer[min_index+1] - 1;

			ofstream out_file_pointer;
			//out_file_pointer.open("output.txt", ios_base::app);


			out_file_pointer.open(argv[2], ios_base::app);
			//cout << output.size() << endl;
			//line_cnt = 0;
			if(output.size() == block_size)
			{
				//cout << block_size << " " << cnnt++ << endl;
				for(int i=0;i<output.size();i++)
				{
					out_file_pointer << output.at(i) << endl;
				}
				output.clear();
			}
			out_file_pointer.close();


		}

		ofstream out_file_pointer;
		//out_file_pointer.open("output.txt", ios_base::app);
		out_file_pointer.open(argv[2], ios_base::app);
		for(int i=0;i<output.size();i++)
		{
			out_file_pointer << output.at(i) << endl;
		}
		out_file_pointer.close();

		string nm;
		for(int i=1;i<=num_of_files;i++)
		{
			ostringstream cv;
			cv << i;
			nm = cv.str() + ".txt";
			if(remove(nm.c_str()) != 0)
			{
				cout << "ERROR: Files created for intermediary use are not deleted!!!" << endl;
			}
		}
	}

	return 0;
}