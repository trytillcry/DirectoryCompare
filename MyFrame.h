#pragma once
#include "wx/wx.h"
#include "wx/taskbar.h"
#include "wx/bannerwindow.h"
#include "File.h"
class Datei;

typedef std::pair<wxString, wxString> INFOPAIR;

class MyFrame : public wxFrame {

	/*------------Eventhandlers------------*/
	
	//When the "End" Button or the "X" was clicked
	template<typename T>
	void closeapp(T&);
	
	//To get additional Fileinfo by mousclick on a listentry
	template <char side>
	void listclickhandler(wxCommandEvent&);
	
	//Takes a FILESET and a position and returns path and size of the File
	INFOPAIR getInfo(Datei**, unsigned int);

	//Listbox Position where the data begin (starts at 0)
	const unsigned char LB_FIRST_DATA = 2;

public:
	
	//Pointers copied from cmain instance for passing lists to tooltip info
	Datei** dif1 = nullptr;
	Datei** dif2 = nullptr;

	//Sizers
	wxBoxSizer* sizer = nullptr;
	wxBoxSizer* topsizer = nullptr;

	//Listboxen
	wxListBox* lbox = nullptr;
	wxListBox* rbox = nullptr;
	
	//Information needed to load the TTC Icon 
	const wxString bmpname = "ttcicon.ico";
	const wxIcon icon = wxIcon(bmpname,wxBITMAP_TYPE_ICO);
	wxTaskBarIcon* tbicon = nullptr;

	//Information needed to load the TTC Logo for the banner 
	const wxString imgname = "ttclogo.bmp";
	const wxBitmap img = wxBitmap(imgname,wxBITMAP_TYPE_ANY);
	const wxBitmapBundle logo = wxBitmapBundle(img);
	wxBannerWindow* banner = nullptr;

	//Button to end the prog
	wxButton* button1 = nullptr;

	//public constructor
	MyFrame();
	
	wxDECLARE_EVENT_TABLE();
};