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
	//Benutzer waehlt Ordner per GUI
	std::wstring folder1 = choosefolder(&BrowseFolderCallback);
	std::wstring folder2 = choosefolder(&BrowseFolderCallback);
	
	//Arrays fuer konvertierte Ordnernamen
	char s1[MAX_PATH], s2[MAX_PATH];

	//Konvertierung der Ordnernamen + aufheben der warnungsignoranz
	wcstombs(s1, folder1.data(), MAX_PATH);
	wcstombs(s2, folder2.data(), MAX_PATH);
	#undef _CRT_SECURE_NO_WARNINGS

	//Erstellen des CMD Befehls fuer Ordneriteration
	const string command1 = batchpath + " " + " \"" + s1 + "\"";
	const string command2 = batchpath + " " + " \"" + s2 + "\"";

	//Child Batch holt Dateinamen und schreibt sie in einen String
	const string cmdout1 = exec(command1.data());
	const string cmdout2 = exec(command2.data());
	
	//Ermittelt anhand Zeilenseperatoren die maximale Anzahl an Dateien
	const unsigned int
	nrows1 = counteach(cmdout1.data(), '\n'),
	nrows2 = counteach(cmdout2.data(), '\n'),
	max_rows = std::max<const unsigned int>(nrows1, nrows2);

	//Heap Speicher fuer Listeneintraege reservieren
	list1 = new FILESET;
	list2 = new FILESET;

	//Befüllen der Listenarrays + Kontrolle der anzahl der extrahierten filenames
	if(nrows1 != fillset(list1, cmdout1, duplicates1)) throw std::runtime_error("nrows1 is not aquivalent filln");
	if(nrows2 != fillset(list2, cmdout2, duplicates2)) throw std::runtime_error("nrows2 is not aquivalent filln");

	//Heap Reservierung für Pointer der Differenzeintraege + nullpointer
	dif1 = new Datei* [max_rows+1];
	dif2 = new Datei* [max_rows+1];

	//Differrenz der Listen ermitteln + Befuellen der vorgesehenen Arrays
	unsigned int ndif1 = (listdiff(*list1, *list2, dif1));
	unsigned int ndif2 = (listdiff(*list2, *list1, dif2));
	
	/*				  ----------------------Build GUI----------------------				  */

	window_main = new MyFrame();

	//Pointerkopie fuer Uebergabe an die Fensterinstanz (window_main) 
	window_main->dif1 = this->dif1;
	window_main->dif2 = this->dif2;
	
	window_main->lbox->Append("From "+ std::to_string(nrows2)+" Files ( "+ std::to_string(duplicates2) + " Duplicates ). " +"These " + std::to_string(ndif2) + " Files are missed:");
	window_main->lbox->Append("");
	for (unsigned int i = 0; dif2[i] != nullptr; i++) window_main->lbox->Append(dif2[i]->getname());
	
	window_main->rbox->Append("From "+ std::to_string(nrows1)+" Files ( "+ std::to_string(duplicates1) + " Duplicates ). " +"These " + std::to_string(ndif1) + " Files are missed:");
	window_main->rbox->Append("");
	for (unsigned int i = 0; dif1[i] != nullptr; i++) window_main->rbox->Append(dif1[i]->getname());
	
	window_main->Show();
	
	return true;
}