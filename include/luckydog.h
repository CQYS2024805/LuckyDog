#ifndef LUCKY_DOG_H
#define LUCKY_DOG_H

#include "ui.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

class App {
public:
	std::vector<std::string> result_list;
public:
	App() {
		srand(time(NULL));
		cfg_file.open("./config/config.txt", std::ios::in | std::ios::out);
		person_file.open("./config/PersonList.txt", std::ios::in | std::ios::out);
	}
	~App() {
		cfg_file.close();
		person_file.close();
	}
	void get_result();int getRandomNum(int count);
private:
	
	void readPersonList();
private:
	struct Config {
		int time;
	}Config;

	std::fstream cfg_file;
	std::fstream person_file;
	int person_count = 0;

	std::vector<std::string> person_list;
};

#endif //LUCKY_DOG_H