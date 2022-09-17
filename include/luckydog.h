#ifndef LUCKY_DOG_H
#define LUCKY_DOG_H

#include "ui.h"

#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>

class App {
private:
	struct Config {
		int time;
	}Config;

	std::fstream cfg_file;
	std::fstream person_file;
	int person_count = 0;

	char** person_list;
private:
	void getPersonList();
public:
	App() {
		srand(time(NULL));
		cfg_file.open("./config/config.txt", std::ios::in | std::ios::out);
		person_file.open("./config/PersonList.txt", std::ios::in | std::ios::out);
		getPersonList();
	}
	~App() {
		cfg_file.close();
		person_file.close();
	}
	int getRandomNum(int count);
};

#endif //LUCKY_DOG_H