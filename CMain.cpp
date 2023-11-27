#include "CMain.h"
#include "funct.h"

wxIMPLEMENT_APP(CMain);

CMain::~CMain()
{	
	delete[] dif1;
	delete[] dif2;
	delete list1;
	delete list2;
}

bool CMain::OnInit()
{
	//Pairs for Folder <number of files,filenames>
	std::pair<unsigned int, string>* p1 = new std::pair<unsigned int, string>;
	std::pair<unsigned int, string>* p2 = new std::pair<unsigned int, string>;

	//User selects first Folder
	std::wstring folder = choosefolder(&BrowseFolderCallback);
	//Analyse first folder in a seperate thread
	std::thread workerthread = std::thread(readcontent, folder, p1);

	//User selects second Folder
	folder = choosefolder(&BrowseFolderCallback);
	//Analyse second Folder in the main thread and wait for the end of thread thr1
	readcontent(folder, p2);
	workerthread.join();
	
	//Determines the maximum number of files based on lineseperator count
	const unsigned int max_rows = std::max<const unsigned int>(p1->first, p2->first);

	//Heap alloc for Filelists 
	list1 = new FILESET;
	list2 = new FILESET;

	//Fills the Sets and check the number of insertions against the nrow constant
	if(p1->first != fillset(list1, p1->second, duplicates1)) throw std::runtime_error("nrows1 is not aquivalent filln");
	if(p2->first != fillset(list2, p2->second, duplicates2)) throw std::runtime_error("nrows2 is not aquivalent filln");

	//Heap Alloc for Pointers to the Differences (nullpointer for iteration)
	dif1 = new Datei* [max_rows+1];
	dif2 = new Datei* [max_rows+1];

	//Number of Differences in Lists
	unsigned int ndif1, ndif2;

	//Determine the Differrences of the lists , Fills Pointers  in the intended arrays (multithreaded)
	workerthread = std::thread(listdiffwrap, std::ref(*list1), std::ref(*list2), dif1, &ndif1);
	ndif2 = listdiff(*list2, *list1, dif2);
	workerthread.join();
		
	/*				  ----------------------Build GUI----------------------				  */
	window_main = new MyFrame();

	//Copies Pointers to the diffarrays for passing them to the window_object
	window_main->dif1 = this->dif1;
	window_main->dif2 = this->dif2;
	
	//Fills Listboxes (Infoheader | Blank Line | Data)
	window_main->lbox->Append("From "+ std::to_string(p2->first)+" Files ( "+ std::to_string(duplicates2) + " Duplicates ). " +"These " + std::to_string(ndif2) + " Files are missed:");
	window_main->lbox->Append("");
	for (unsigned int i = 0; dif2[i] != nullptr; i++) window_main->lbox->Append(dif2[i]->getname());
	
	window_main->rbox->Append("From "+ std::to_string(p1->first)+" Files ( "+ std::to_string(duplicates1) + " Duplicates ). " +"These " + std::to_string(ndif1) + " Files are missed:");
	window_main->rbox->Append("");
	for (unsigned int i = 0; dif1[i] != nullptr; i++) window_main->rbox->Append(dif1[i]->getname());
	
	
	window_main->Show();
	
	delete p1, p2;

	return true;
}