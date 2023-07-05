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
	//User selects Folders
	std::wstring folder1 = choosefolder(&BrowseFolderCallback);
	std::wstring folder2 = choosefolder(&BrowseFolderCallback);
	
	//Arrays for converted folderpaths
	char s1[MAX_PATH], s2[MAX_PATH];
	
	//Conversion of the folder names wstring -> cstring 
	#ifdef _MSC_VER
	#define _CRT_SECURE_NO_WARNINGS
	#endif
	wcstombs(s1, folder1.data(), MAX_PATH);
	wcstombs(s2, folder2.data(), MAX_PATH);
	#undef _CRT_SECURE_NO_WARNINGS

	//Building the command for exec() the folder iteration
	const string command1 = batchpath + "  \"" + s1 + "\"";
	const string command2 = batchpath + "  \"" + s2 + "\"";

	//Child Batch retrieves Filenames and writes it in a String
	const string cmdout1 = exec(command1.data());
	const string cmdout2 = exec(command2.data());
	
	//Determines the maximum number of files based on lineseperator count
	const unsigned int nrows1 = counteach(cmdout1.data(), '\n');
	const unsigned int nrows2 = counteach(cmdout2.data(), '\n');
	const unsigned int max_rows = std::max<const unsigned int>(nrows1, nrows2);

	//Heap alloc for Filelists 
	list1 = new FILESET;
	list2 = new FILESET;

	//Fills the Sets and check the number of insertions against the nrow constant
	if(nrows1 != fillset(list1, cmdout1, duplicates1)) throw std::runtime_error("nrows1 is not aquivalent filln");
	if(nrows2 != fillset(list2, cmdout2, duplicates2)) throw std::runtime_error("nrows2 is not aquivalent filln");

	//Heap Alloc for Pointers to the Differences (nullpointer for iteration)
	dif1 = new Datei* [max_rows+1];
	dif2 = new Datei* [max_rows+1];

	//Determine the Differrences of the lists , Fills Pointers  in the intended arrays
	unsigned int ndif1 = (listdiff(*list1, *list2, dif1));
	unsigned int ndif2 = (listdiff(*list2, *list1, dif2));
	
	/*				  ----------------------Build GUI----------------------				  */

	window_main = new MyFrame();

	//Copies Pointers to the diffarrays for passing them to the window_object
	window_main->dif1 = this->dif1;
	window_main->dif2 = this->dif2;
	
	//Fills Listboxes (Infoheader | Blank Line | Data)
	window_main->lbox->Append("From "+ std::to_string(nrows2)+" Files ( "+ std::to_string(duplicates2) + " Duplicates ). " +"These " + std::to_string(ndif2) + " Files are missed:");
	window_main->lbox->Append("");
	for (unsigned int i = 0; dif2[i] != nullptr; i++) window_main->lbox->Append(dif2[i]->getname());
	
	window_main->rbox->Append("From "+ std::to_string(nrows1)+" Files ( "+ std::to_string(duplicates1) + " Duplicates ). " +"These " + std::to_string(ndif1) + " Files are missed:");
	window_main->rbox->Append("");
	for (unsigned int i = 0; dif1[i] != nullptr; i++) window_main->rbox->Append(dif1[i]->getname());
	
	
	window_main->Show();
	
	return true;
}