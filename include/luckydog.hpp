#pragma once
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

class App {
public:
	std::vector<std::wstring> result_list;
	std::vector<std::string> result_lista;
public:
	App() {
		srand(time(NULL));
		cfg_file.open("./config/config.txt", std::ios::in | std::ios::out);
		person_file.open("./config/PersonList.txt", std::ios::in | std::ios::out);
		readPersonList();
	}
	~App() {
		cfg_file.close();
		person_file.close();
	}
	void get_result();
		int getRandomNum(int count);
	
private:
	void readPersonList();
	int person_count = 0;
private:
	struct Config {
		int time;
	}Config;

	std::fstream cfg_file;
	std::fstream person_file;
	

	std::vector<std::string> person_list;
};

int App::getRandomNum(int num) {
	return rand() % (num);
	return 0;
}

void App::readPersonList() {
	std::string buffer;
	int count = 0;
	while (!person_file.eof())
	{
		person_file >> buffer;
		person_list.push_back(buffer);
		count++;
	}
	person_count = count;
}

void App::get_result() {
	for (int i = 0; i < 10; i++) {
		std::string str = person_list[getRandomNum(person_count)];
		std::wstring result;

		int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
		wchar_t* buffer = new wchar_t[len + 1];

		MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
		buffer[len] = '\0';          

		result.append(buffer);
		delete[] buffer;

		result_list.push_back(result);
		result_lista.push_back(str.c_str());
		//MessageBoxW(NULL, result.c_str(), L"", MB_OK);
	}
}