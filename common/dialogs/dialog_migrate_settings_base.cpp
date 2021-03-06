///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Oct 26 2018)
// http://www.wxformbuilder.org/
//
// PLEASE DO *NOT* EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#include "dialog_migrate_settings_base.h"

///////////////////////////////////////////////////////////////////////////

DIALOG_MIGRATE_SETTINGS_BASE::DIALOG_MIGRATE_SETTINGS_BASE( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : DIALOG_SHIM( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

	m_sizer = new wxBoxSizer( wxVERTICAL );

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer( wxVERTICAL );

	m_lblWelcome = new wxStaticText( this, wxID_ANY, wxT("Welcome to KiCad %s!"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblWelcome->Wrap( -1 );
	m_lblWelcome->SetFont( wxFont( wxNORMAL_FONT->GetPointSize(), wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD, false, wxEmptyString ) );

	bSizer6->Add( m_lblWelcome, 0, wxALL|wxEXPAND, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("How would you like to configure KiCad?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer6->Add( m_staticText2, 0, wxALL|wxEXPAND, 5 );

	m_btnPrevVer = new wxRadioButton( this, wxID_ANY, wxT("Import settings from a previous version at:"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_btnPrevVer, 0, wxALL|wxEXPAND, 5 );

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_cbPath = new wxComboBox( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, NULL, 0 );
	m_cbPath->SetMinSize( wxSize( 480,-1 ) );

	bSizer5->Add( m_cbPath, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5 );

	m_btnCustomPath = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW|0 );
	m_btnCustomPath->SetToolTip( wxT("Choose a different path") );
	m_btnCustomPath->SetMinSize( wxSize( 40,30 ) );

	bSizer5->Add( m_btnCustomPath, 0, wxALL, 5 );


	bSizer6->Add( bSizer5, 1, wxEXPAND, 5 );

	m_lblPathError = new wxStaticText( this, wxID_ANY, wxT("The selected path does not contain valid KiCad settings!"), wxDefaultPosition, wxDefaultSize, 0 );
	m_lblPathError->Wrap( -1 );
	m_lblPathError->SetForegroundColour( wxColour( 255, 43, 0 ) );
	m_lblPathError->Hide();

	bSizer6->Add( m_lblPathError, 0, wxALIGN_CENTER_HORIZONTAL|wxALL, 5 );

	m_btnUseDefaults = new wxRadioButton( this, wxID_ANY, wxT("Start with default settings"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer6->Add( m_btnUseDefaults, 0, wxALL|wxEXPAND, 5 );


	m_sizer->Add( bSizer6, 0, wxEXPAND|wxLEFT|wxRIGHT|wxTOP, 5 );


	m_sizer->Add( 0, 0, 1, wxEXPAND, 5 );

	m_standardButtons = new wxStdDialogButtonSizer();
	m_standardButtonsOK = new wxButton( this, wxID_OK );
	m_standardButtons->AddButton( m_standardButtonsOK );
	m_standardButtonsCancel = new wxButton( this, wxID_CANCEL );
	m_standardButtons->AddButton( m_standardButtonsCancel );
	m_standardButtons->Realize();
	m_standardButtons->SetMinSize( wxSize( -1,50 ) );

	m_sizer->Add( m_standardButtons, 0, wxBOTTOM|wxEXPAND|wxLEFT|wxRIGHT, 10 );


	this->SetSizer( m_sizer );
	this->Layout();
	m_sizer->Fit( this );

	this->Centre( wxBOTH );

	// Connect Events
	m_btnPrevVer->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnPrevVerSelected ), NULL, this );
	m_cbPath->Connect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnPathChanged ), NULL, this );
	m_cbPath->Connect( wxEVT_KILL_FOCUS, wxFocusEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnPathDefocused ), NULL, this );
	m_cbPath->Connect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnPathChanged ), NULL, this );
	m_btnCustomPath->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnChoosePath ), NULL, this );
	m_btnUseDefaults->Connect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnDefaultSelected ), NULL, this );
}

DIALOG_MIGRATE_SETTINGS_BASE::~DIALOG_MIGRATE_SETTINGS_BASE()
{
	// Disconnect Events
	m_btnPrevVer->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnPrevVerSelected ), NULL, this );
	m_cbPath->Disconnect( wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnPathChanged ), NULL, this );
	m_cbPath->Disconnect( wxEVT_KILL_FOCUS, wxFocusEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnPathDefocused ), NULL, this );
	m_cbPath->Disconnect( wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnPathChanged ), NULL, this );
	m_btnCustomPath->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnChoosePath ), NULL, this );
	m_btnUseDefaults->Disconnect( wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler( DIALOG_MIGRATE_SETTINGS_BASE::OnDefaultSelected ), NULL, this );

}
