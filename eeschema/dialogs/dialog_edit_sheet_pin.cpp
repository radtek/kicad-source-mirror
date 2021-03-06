/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2010 Wayne Stambaugh <stambaughw@verizon.net>
 * Copyright (C) 2018-2019 KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <sch_edit_frame.h>
#include <sch_sheet.h>
#include <sch_validators.h>
#include <dialog_edit_sheet_pin.h>


static wxString sheetPinTypes[] =
{
    _( "Input" ),
    _( "Output" ),
    _( "Bidirectional" ),
    _( "Tri-state" ),
    _( "Passive" )
};


DIALOG_EDIT_SHEET_PIN::DIALOG_EDIT_SHEET_PIN( SCH_EDIT_FRAME* parent, SCH_SHEET_PIN* aPin ) :
    DIALOG_EDIT_SHEET_PIN_BASE( parent ),
    m_frame( parent ),
    m_sheetPin( aPin ),
    m_textSize( parent, m_textSizeLabel, m_textSizeCtrl, m_textSizeUnits, true )
{
    for( const wxString& sheetPinType : sheetPinTypes )
        m_choiceConnectionType->Append( sheetPinType );

    m_choiceConnectionType->SetSelection( 0 );
    SetInitialFocus( m_comboName );
    m_sdbSizerOK->SetDefault();

    // Set invalid label characters list:
    SCH_NETNAME_VALIDATOR validator;
    m_comboName->SetValidator( validator );

    // Now all widgets have the size fixed, call FinishDialogSettings
    FinishDialogSettings();

    /* This ugly hack fixes a bug in wxWidgets 2.8.7 and likely earlier versions for
     * the flex grid sizer in wxGTK that prevents the last column from being sized
     * correctly.  It doesn't cause any problems on win32 so it doesn't need to wrapped
     * in ugly #ifdef __WXGTK__ #endif.
     */
    Layout();
    Fit();
    SetMinSize( GetSize() );

    // On some windows manager (Unity, XFCE), this dialog is
    // not always raised, depending on this dialog is run.
    // Force it to be raised
    Raise();
}


bool DIALOG_EDIT_SHEET_PIN::TransferDataToWindow()
{
    SCH_SCREEN* screen = m_sheetPin->GetParent()->GetScreen();

    for( SCH_ITEM* item : screen->Items().OfType( SCH_HIER_LABEL_T ) )
    {
        m_comboName->Append( static_cast<SCH_HIERLABEL*>( item )->GetText() );
    }

    m_comboName->SetValue( UnescapeString( m_sheetPin->GetText() ) );
    m_comboName->SelectAll();
    // Currently, eeschema uses only the text width as text size
    // (only the text width is saved in files), and expects text width = text height
    m_textSize.SetValue( m_sheetPin->GetTextWidth() );
    m_choiceConnectionType->SetSelection( static_cast<int>( m_sheetPin->GetShape() ) );

    return true;
}


bool DIALOG_EDIT_SHEET_PIN::TransferDataFromWindow()
{
    if( !m_sheetPin->IsNew() )
        m_frame->SaveCopyInUndoList( (SCH_ITEM*) m_sheetPin->GetParent(), UR_CHANGED );

    m_sheetPin->SetText( EscapeString( m_comboName->GetValue(), CTX_NETNAME ) );
    // Currently, eeschema uses only the text width as text size,
    // and expects text width = text height
    m_sheetPin->SetTextSize( wxSize( m_textSize.GetValue(), m_textSize.GetValue() ) );

    auto shape = static_cast<PINSHEETLABEL_SHAPE>( m_choiceConnectionType->GetCurrentSelection() );
    m_sheetPin->SetShape( shape );

    m_frame->RefreshItem( m_sheetPin );
    m_frame->GetCanvas()->Refresh();
    m_frame->OnModify();

    return true;
}


void DIALOG_EDIT_SHEET_PIN::onOKButton( wxCommandEvent& event )
{
    event.Skip();
}


void DIALOG_EDIT_SHEET_PIN::OnSyntaxHelp( wxHyperlinkEvent& aEvent )
{
    SCH_TEXT::ShowSyntaxHelp( this );
}


void DIALOG_EDIT_SHEET_PIN::onComboBox( wxCommandEvent& aEvent )
{
    SCH_SCREEN* screen = m_sheetPin->GetParent()->GetScreen();

    for( SCH_ITEM* item : screen->Items().OfType( SCH_HIER_LABEL_T ) )
    {
        auto hierLabelItem = static_cast<SCH_HIERLABEL*>( item );

        if( m_comboName->GetValue().CmpNoCase( hierLabelItem->GetText() ) == 0 )
        {
            m_choiceConnectionType->SetSelection( static_cast<int>( hierLabelItem->GetShape() ) );
            break;
        }
    }
}
