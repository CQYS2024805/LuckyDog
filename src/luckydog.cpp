#include "../include/luckydog.h"

int App::getRandomNum(int num) {
	return rand() % (num);
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
    readPersonList();
    for (int i = 0; i < 10; i++) {
        result_list.push_back(person_list[getRandomNum(person_count)]);
    }
}