#pragma once
#include "MyFrame.h"
#include <thread>

class CMain : public wxApp {

	//Frame for (Main)Window
	MyFrame* window_main = nullptr;
	
	//Pointers to Filelists
	FILESET* list1 = nullptr;
	FILESET* list2 = nullptr;
	
	//Pointers to Files which not found in both folders
	Datei** dif1 = nullptr;
	Datei** dif2 = nullptr;

public:
		
	//main
	bool OnInit();

	~CMain();
};

