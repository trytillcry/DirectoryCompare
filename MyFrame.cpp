#include "MyFrame.h"

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
EVT_BUTTON(10001, MyFrame::closeapp<wxCommandEvent>)
EVT_LISTBOX(10002,MyFrame::listclickhandler<'l'>)
EVT_LISTBOX(10003, MyFrame::listclickhandler<'r'>)
EVT_CLOSE(closeapp<wxCloseEvent>)
wxEND_EVENT_TABLE()


MyFrame::MyFrame() : wxFrame(nullptr,10000,"DirectoryCompare",wxPoint(10,10),wxSize(1000,900)) {

	//Disable resize borders / disable maximize button
	SetWindowStyle(wxDEFAULT_FRAME_STYLE & ~wxRESIZE_BORDER & ~wxMAXIMIZE_BOX);
	
	//Background
	SetBackgroundColour(*wxWHITE);

	//Icon
	this->SetIcon(icon);
	tbicon = new wxTaskBarIcon();

	//Sizers
	sizer = new wxBoxSizer(wxHORIZONTAL);
	topsizer = new wxBoxSizer(wxVERTICAL);

	//Logo
	banner = new wxBannerWindow(this, wxBottom);
	banner->SetBitmap(logo);

	//Listboxes
	lbox = new wxListBox(this, 10002, wxDefaultPosition, wxSize(490, 560), NULL, wxLB_NEEDED_SB);
	rbox = new wxListBox(this, 10003, wxDefaultPosition, wxSize(490, 560), NULL, wxLB_NEEDED_SB);

	//Button
	button1 = new wxButton(this,10001, "END",wxPoint(10,10), wxSize(120, 25));
	button1->SetBackgroundColour(*wxWHITE);

	//Layout the frame
	sizer->Add(lbox, wxSizerFlags().Expand());
	sizer->AddSpacer(1);
	sizer->Add(rbox, wxSizerFlags().Expand());
	topsizer->Add(banner, wxSizerFlags().Center());
	topsizer->Add(sizer, wxSizerFlags().Expand());
	topsizer->Add(button1, wxSizerFlags().Center());
	topsizer->Fit(this);
	SetSizer(topsizer);
};

template <typename T>
void MyFrame::closeapp(T& closeevt){
	
	delete tbicon;
	delete button1;
	delete lbox;
	delete rbox;
		
	DestroyChildren();
	Destroy();
}

INFOPAIR MyFrame::getInfo(Datei** difp, unsigned int row){

	return std::make_pair(wxString(difp[row]->getpath()), wxString(std::to_string(difp[row]->getsize())));
}

template <char site> 
void MyFrame::listclickhandler(wxCommandEvent& cevt) {

	unsigned int rownumber = cevt.GetSelection();
	if (!(rownumber >= LB_FIRST_DATA )) return; //Abort the Event if a row in the offset to the data was clicked 
	std::pair<wxString, wxString> infopair;
	if(site == 'l'){ infopair = MyFrame::getInfo(this->dif2, rownumber - LB_FIRST_DATA); }
	else if (site == 'r') { infopair = MyFrame::getInfo(this->dif1, rownumber - LB_FIRST_DATA); }
	const wxString info = infopair.first +"\n"+ infopair.second + " Bytes";
	wxToolTip* tip = new wxToolTip(info);
	if (site == 'l') {this->lbox->SetToolTip(tip);}
	else if (site == 'r') { this->rbox->SetToolTip(tip);}
	wxPoint mousePos = wxGetMousePosition();
	tip->SetDelay(100);
	tip->SetAutoPop(3000);
	tip->Enable(true);
	cevt.Skip();
	return;
}