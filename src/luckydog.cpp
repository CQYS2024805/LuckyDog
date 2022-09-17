#include "../include/luckydog.h"

int App::getRandomNum(int count) {
	return rand() % (count);
}

void App::getPersonList() {
    char buffer[256];
    while (!person_file.eof())
    {
        person_file.getline(buffer, 100);
        std::cout << buffer << std::endl;
    }
}