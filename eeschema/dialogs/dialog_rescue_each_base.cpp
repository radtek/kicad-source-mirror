///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Dec 30 2017)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dialog_rescue_each_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_RESCUE_EACH_BASE::DIALOG_RESCUE_EACH_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );
	
	m_htmlPrompt = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	m_htmlPrompt->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE ) );
	m_htmlPrompt->SetMinSize( wxSize( -1,50 ) );
	
	bSizerMain->Add( m_htmlPrompt, 0, wxEXPAND|wxTOP|wxBOTTOM, 5 );
	
	m_titleSymbols = new wxStaticText( this, wxID_ANY, _("Symbols to update:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_titleSymbols->Wrap( -1 );
	m_titleSymbols->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	
	bSizerMain->Add( m_titleSymbols, 0, wxLEFT|wxRIGHT, 10 );
	
	m_ListOfConflicts = new wxDataViewListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( m_ListOfConflicts, 3, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 10 );
	
	m_titleInstances = new wxStaticText( this, wxID_ANY, _("Instances of this symbol:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_titleInstances->Wrap( -1 );
	m_titleInstances->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	
	bSizerMain->Add( m_titleInstances, 0, wxRIGHT|wxLEFT, 10 );
	
	m_ListOfInstances = new wxDataViewListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	bSizerMain->Add( m_ListOfInstances, 2, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 10 );
	
	wxBoxSizer* bSizerPreviews;
	bSizerPreviews = new wxBoxSizer( wxHORIZONTAL );
	
	wxBoxSizer* bSizerLeftPreview;
	bSizerLeftPreview = new wxBoxSizer( wxVERTICAL );
	
	m_previewOldLabel = new wxStaticText( this, wxID_ANY, _("Cached Symbol:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_previewOldLabel->Wrap( -1 );
	m_previewOldLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	
	bSizerLeftPreview->Add( m_previewOldLabel, 0, 0, 5 );
	
	m_previewOldPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerOldPanel;
	bSizerOldPanel = new wxBoxSizer( wxVERTICAL );
	
	m_previewOldWidget = new SYMBOL_PREVIEW_WIDGET( m_previewOldPanel, Kiway() );
	bSizerOldPanel->Add( m_previewOldWidget, 1, wxEXPAND, 5 );
	
	
	m_previewOldPanel->SetSizer( bSizerOldPanel );
	m_previewOldPanel->Layout();
	bSizerOldPanel->Fit( m_previewOldPanel );
	bSizerLeftPreview->Add( m_previewOldPanel, 1, wxEXPAND | wxALL, 5 );
	
	
	bSizerPreviews->Add( bSizerLeftPreview, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizerRightPreview;
	bSizerRightPreview = new wxBoxSizer( wxVERTICAL );
	
	m_previewNewLabel = new wxStaticText( this, wxID_ANY, _("Library Symbol:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_previewNewLabel->Wrap( -1 );
	m_previewNewLabel->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );
	
	bSizerRightPreview->Add( m_previewNewLabel, 0, 0, 5 );
	
	m_previewNewPanel = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizerNewPanel;
	bSizerNewPanel = new wxBoxSizer( wxVERTICAL );
	
	m_previewNewWidget = new SYMBOL_PREVIEW_WIDGET( m_previewNewPanel, Kiway() );
	bSizerNewPanel->Add( m_previewNewWidget, 1, wxEXPAND, 5 );
	
	
	m_previewNewPanel->SetSizer( bSizerNewPanel );
	m_previewNewPanel->Layout();
	bSizerNewPanel->Fit( m_previewNewPanel );
	bSizerRightPreview->Add( m_previewNewPanel, 1, wxEXPAND | wxALL, 5 );
	
	
	bSizerPreviews->Add( bSizerRightPreview, 1, wxEXPAND, 5 );
	
	
	bSizerMain->Add( bSizerPreviews, 3, wxEXPAND|wxRIGHT|wxLEFT, 10 );
	
	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );
	
	m_btnNeverShowAgain = new wxButton( this, wxID_ANY, _("Never Show Again"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer5->Add( m_btnNeverShowAgain, 0, wxALIGN_CENTER_VERTICAL|wxRIGHT|wxLEFT, 10 );
	
	
	bSizer5->Add( 0, 0, 1, wxEXPAND, 5 );
	
	m_stdButtons = new wxStdDialogButtonSizer();
	m_stdButtonsOK = new wxButton( this, wxID_OK );
	m_stdButtons->AddButton( m_stdButtonsOK );
	m_stdButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_stdButtons->AddButton( m_stdButtonsCancel );
	m_stdButtons->Realize();
	
	bSizer5->Add( m_stdButtons, 0, wxALL|wxEXPAND, 5 );
	
	
	bSizerMain->Add( bSizer5, 0, wxEXPAND, 5 );
	
	
	this->SetSizer( bSizerMain );
	this->Layout();
	bSizerMain->Fit( this );
	
	this->Centre( wxBOTH );
	
	// Connect Events
	this->Connect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_RESCUE_EACH_BASE::OnDialogResize ) );
	m_ListOfConflicts->Connect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( DIALOG_RESCUE_EACH_BASE::OnConflictSelect ), NULL, this );
	m_btnNeverShowAgain->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_RESCUE_EACH_BASE::OnNeverShowClick ), NULL, this );
	m_stdButtonsCancel->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_RESCUE_EACH_BASE::OnCancelClick ), NULL, this );
}

DIALOG_RESCUE_EACH_BASE::~DIALOG_RESCUE_EACH_BASE()
{
	// Disconnect Events
	this->Disconnect( wxEVT_SIZE, wxSizeEventHandler( DIALOG_RESCUE_EACH_BASE::OnDialogResize ) );
	m_ListOfConflicts->Disconnect( wxEVT_COMMAND_DATAVIEW_SELECTION_CHANGED, wxDataViewEventHandler( DIALOG_RESCUE_EACH_BASE::OnConflictSelect ), NULL, this );
	m_btnNeverShowAgain->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_RESCUE_EACH_BASE::OnNeverShowClick ), NULL, this );
	m_stdButtonsCancel->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_RESCUE_EACH_BASE::OnCancelClick ), NULL, this );
	
}
