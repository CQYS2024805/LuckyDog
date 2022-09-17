#include "../include/luckydog.h"
#include <conio.h>

int main(){
	App app;
	for(int i=0;i<10;i++){
		std::cout<<app.getRandomNum(51)<<std::endl;
	}
	getch();
}