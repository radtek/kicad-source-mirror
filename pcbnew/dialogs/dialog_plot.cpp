/**
 * @file dialog_plot.cpp
 */

/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 1992-2018 KiCad Developers, see AUTHORS.txt for contributors.
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


#include <kiface_i.h>
#include <plotter.h>
#include <confirm.h>
#include <pcb_edit_frame.h>
#include <pcbplot.h>
#include <gerber_jobfile_writer.h>
#include <base_units.h>
#include <macros.h>
#include <reporter.h>
#include <wildcards_and_files_ext.h>
#include <bitmaps.h>

#include <class_board.h>
#include <wx/ffile.h>
#include <dialog_plot.h>
#include <wx_html_report_panel.h>
#include <drc.h>


DIALOG_PLOT::DIALOG_PLOT( PCB_EDIT_FRAME* aParent ) :
    DIALOG_PLOT_BASE( aParent ), m_parent( aParent )
{
    SetName( DLG_WINDOW_NAME );
    m_userUnits = g_UserUnit;
    m_config = Kiface().KifaceSettings();
    m_plotOpts = aParent->GetPlotSettings();
    init_Dialog();

    // We use a sdbSizer here to get the order right, which is platform-dependent
    m_sdbSizer1OK->SetLabel( _( "Plot" ) );
    m_sdbSizer1Apply->SetLabel( _( "Generate Drill Files..." ) );
    m_sdbSizer1Cancel->SetLabel( _( "Close" ) );
    m_sizerButtons->Layout();

    m_sdbSizer1OK->SetDefault();

    GetSizer()->Fit( this );
    GetSizer()->SetSizeHints( this );
}


void DIALOG_PLOT::init_Dialog()
{
    BOARD*      board = m_parent->GetBoard();
    wxString    msg;
    wxFileName  fileName;

    m_config->Read( OPTKEY_PLOT_X_FINESCALE_ADJ, &m_XScaleAdjust );
    m_config->Read( OPTKEY_PLOT_Y_FINESCALE_ADJ, &m_YScaleAdjust );

    bool checkZones;
    m_config->Read( OPTKEY_PLOT_CHECK_ZONES, &checkZones, true );
    m_zoneFillCheck->SetValue( checkZones );

    m_browseButton->SetBitmap( KiBitmap( browse_files_xpm ) );

    // m_PSWidthAdjust is stored in mm in user config
    double dtmp;
    m_config->Read( CONFIG_PS_FINEWIDTH_ADJ, &dtmp, 0 );
    m_PSWidthAdjust = KiROUND( dtmp * IU_PER_MM );

    // The reasonable width correction value must be in a range of
    // [-(MinTrackWidth-1), +(MinClearanceValue-1)] decimils.
    m_widthAdjustMinValue   = -( board->GetDesignSettings().m_TrackMinWidth - 1 );
    m_widthAdjustMaxValue   = board->GetDesignSettings().GetSmallestClearanceValue() - 1;

    switch( m_plotOpts.GetFormat() )
    {
    default:
    case PLOT_FORMAT_GERBER:
        m_plotFormatOpt->SetSelection( 0 );
        break;

    case PLOT_FORMAT_POST:
        m_plotFormatOpt->SetSelection( 1 );
        break;

    case PLOT_FORMAT_SVG:
        m_plotFormatOpt->SetSelection( 2 );
        break;

    case PLOT_FORMAT_DXF:
        m_plotFormatOpt->SetSelection( 3 );
        break;

    case PLOT_FORMAT_HPGL:
        m_plotFormatOpt->SetSelection( 4 );
        break;

    case PLOT_FORMAT_PDF:
        m_plotFormatOpt->SetSelection( 5 );
        break;
    }

    msg = StringFromValue( m_userUnits, board->GetDesignSettings().m_SolderMaskMargin, true );
    m_SolderMaskMarginCurrValue->SetLabel( msg );
    msg = StringFromValue( m_userUnits, board->GetDesignSettings().m_SolderMaskMinWidth, true );
    m_SolderMaskMinWidthCurrValue->SetLabel( msg );

    // Set units and value for HPGL pen size (this param is stored in mils).
    AddUnitSymbol( *m_textPenSize, m_userUnits );

    msg = StringFromValue( m_userUnits,
                           m_plotOpts.GetHPGLPenDiameter() * IU_PER_MILS );
    m_HPGLPenSizeOpt->SetValue( msg );

    AddUnitSymbol( *m_textDefaultPenSize, m_userUnits );
    msg = StringFromValue( m_userUnits, m_plotOpts.GetLineWidth() );
    m_linesWidth->SetValue( msg );

    // Set units for PS global width correction.
    AddUnitSymbol( *m_textPSFineAdjustWidth, m_userUnits );

    // Test for a reasonable scale value. Set to 1 if problem
    if( m_XScaleAdjust < PLOT_MIN_SCALE || m_YScaleAdjust < PLOT_MIN_SCALE
        || m_XScaleAdjust > PLOT_MAX_SCALE || m_YScaleAdjust > PLOT_MAX_SCALE )
        m_XScaleAdjust = m_YScaleAdjust = 1.0;

    msg.Printf( wxT( "%f" ), m_XScaleAdjust );
    m_fineAdjustXscaleOpt->AppendText( msg );

    msg.Printf( wxT( "%f" ), m_YScaleAdjust );
    m_fineAdjustYscaleOpt->SetValue( msg );

    // Test for a reasonable PS width correction value. Set to 0 if problem.
    if( m_PSWidthAdjust < m_widthAdjustMinValue || m_PSWidthAdjust > m_widthAdjustMaxValue )
        m_PSWidthAdjust = 0.;

    msg.Printf( wxT( "%f" ), To_User_Unit( m_userUnits, m_PSWidthAdjust ) );
    m_PSFineAdjustWidthOpt->SetValue( msg );

    m_plotPSNegativeOpt->SetValue( m_plotOpts.GetNegative() );
    m_forcePSA4OutputOpt->SetValue( m_plotOpts.GetA4Output() );

    // Could devote a PlotOrder() function in place of UIOrder().
    m_layerList = board->GetEnabledLayers().UIOrder();

    // Populate the check list box by all enabled layers names
    for( LSEQ seq = m_layerList;  seq;  ++seq )
    {
        PCB_LAYER_ID layer = *seq;

        int checkIndex = m_layerCheckListBox->Append( board->GetLayerName( layer ) );

        if( m_plotOpts.GetLayerSelection()[layer] )
            m_layerCheckListBox->Check( checkIndex );
    }

    // Option for using proper Gerber extensions
    m_useGerberExtensions->SetValue( m_plotOpts.GetUseGerberProtelExtensions() );

    // Option for including Gerber attributes (from Gerber X2 format) in the output
    m_useGerberX2Attributes->SetValue( m_plotOpts.GetUseGerberAttributes() );

    // Option for including Gerber netlist info (from Gerber X2 format) in the output
    m_useGerberNetAttributes->SetValue( m_plotOpts.GetIncludeGerberNetlistInfo() );

    // Grey out if m_useGerberX2Attributes is not checked
    m_useGerberNetAttributes->Enable( m_useGerberX2Attributes->GetValue() );

    // Option to generate a Gerber job file
    m_generateGerberJobFile->SetValue( m_plotOpts.GetCreateGerberJobFile() );

    // Gerber precision for coordinates
    m_rbGerberFormat->SetSelection( m_plotOpts.GetGerberPrecision() == 5 ? 0 : 1 );

    // Option for excluding contents of "Edges Pcb" layer
    m_excludeEdgeLayerOpt->SetValue( m_plotOpts.GetExcludeEdgeLayer() );

    // Option to exclude pads from silkscreen layers
    m_excludePadsFromSilkscreen->SetValue( !m_plotOpts.GetPlotPadsOnSilkLayer() );

    m_subtractMaskFromSilk->SetValue( m_plotOpts.GetSubtractMaskFromSilk() );

    // Option to plot page references:
    m_plotSheetRef->SetValue( m_plotOpts.GetPlotFrameRef() );

    // Options to plot texts on footprints
    m_plotModuleValueOpt->SetValue( m_plotOpts.GetPlotValue() );
    m_plotModuleRefOpt->SetValue( m_plotOpts.GetPlotReference() );
    m_plotInvisibleText->SetValue( m_plotOpts.GetPlotInvisibleText() );

    // Options to plot pads and vias holes
    m_drillShapeOpt->SetSelection( m_plotOpts.GetDrillMarksType() );

    // Scale option
    m_scaleOpt->SetSelection( m_plotOpts.GetScaleSelection() );

    // Plot mode
    setPlotModeChoiceSelection( m_plotOpts.GetPlotMode() );

    // Plot outline mode
    m_DXF_plotModeOpt->SetValue( m_plotOpts.GetDXFPlotPolygonMode() );

    // Plot text mode
    m_DXF_plotTextStrokeFontOpt->SetValue( m_plotOpts.GetTextMode() == PLOTTEXTMODE_DEFAULT );

    // Plot mirror option
    m_plotMirrorOpt->SetValue( m_plotOpts.GetMirror() );

    // Put vias on mask layer
    m_plotNoViaOnMaskOpt->SetValue( m_plotOpts.GetPlotViaOnMaskLayer() );

    // Initialize a few other parameters, which can also be modified
    // from the drill dialog
    reInitDialog();

    // Update options values:
    wxCommandEvent cmd_event;
    SetPlotFormat( cmd_event );
    OnSetScaleOpt( cmd_event );
}


void DIALOG_PLOT::reInitDialog()
{
    // after calling drill dialog, some parameters can be modified.
    // update them

    // Output directory
    m_outputDirectoryName->SetValue( m_plotOpts.GetOutputDirectory() );

    // Origin of coordinates:
    m_useAuxOriginCheckBox->SetValue( m_plotOpts.GetUseAuxOrigin() );
}


void DIALOG_PLOT::OnQuit( wxCommandEvent& event )
{
    event.Skip();

    Destroy();
}


void DIALOG_PLOT::OnClose( wxCloseEvent& event )
{
    Destroy();
}


// A helper function to show a popup menu, when the dialog is right clicked.
void DIALOG_PLOT::OnRightClick( wxMouseEvent& event )
{
    PopupMenu( m_popMenu );
}


// Select or deselect groups of layers in the layers list:
#include <layers_id_colors_and_visibility.h>
void DIALOG_PLOT::OnPopUpLayers( wxCommandEvent& event )
{
    unsigned int i;

    switch( event.GetId() )
    {
    case ID_LAYER_FAB: // Select layers usually needed to build a board
        for( i = 0; i < m_layerList.size(); i++ )
        {
            LSET layermask( m_layerList[ i ] );

            if( ( layermask & ( LSET::AllCuMask() | LSET::AllTechMask() ) ).any() )
                m_layerCheckListBox->Check( i, true );
            else
                m_layerCheckListBox->Check( i, false );
        }
        break;

    case ID_SELECT_COPPER_LAYERS:
        for( i = 0; i < m_layerList.size(); i++ )
        {
            if( IsCopperLayer( m_layerList[i] ) )
                m_layerCheckListBox->Check( i, true );
        }
        break;

    case ID_DESELECT_COPPER_LAYERS:
        for( i = 0; i < m_layerList.size(); i++ )
        {
            if( IsCopperLayer( m_layerList[i] ) )
                m_layerCheckListBox->Check( i, false );
        }
        break;

    case ID_SELECT_ALL_LAYERS:
        for( i = 0; i < m_layerList.size(); i++ )
            m_layerCheckListBox->Check( i, true );
        break;

    case ID_DESELECT_ALL_LAYERS:
        for( i = 0; i < m_layerList.size(); i++ )
            m_layerCheckListBox->Check( i, false );
        break;

    default:
        break;
    }
}


void DIALOG_PLOT::CreateDrillFile( wxCommandEvent& event )
{
    // Be sure drill file use the same settings (axis option, plot directory)
    // than plot files:
    applyPlotSettings();
    m_parent->InstallDrillFrame( event );

    // a few plot settings can be modified: take them in account
    m_plotOpts = m_parent->GetPlotSettings();
    reInitDialog();
}


void DIALOG_PLOT::OnChangeDXFPlotMode( wxCommandEvent& event )
{
    // m_DXF_plotTextStrokeFontOpt is disabled if m_DXF_plotModeOpt
    // is checked (plot in DXF polygon mode)
    m_DXF_plotTextStrokeFontOpt->Enable( !m_DXF_plotModeOpt->GetValue() );

    // if m_DXF_plotTextStrokeFontOpt option is disabled (plot DXF in polygon mode),
    // force m_DXF_plotTextStrokeFontOpt to true to use Pcbnew stroke font
    if( !m_DXF_plotTextStrokeFontOpt->IsEnabled() )
        m_DXF_plotTextStrokeFontOpt->SetValue( true );
}


void DIALOG_PLOT::OnSetScaleOpt( wxCommandEvent& event )
{
    /* Disable sheet reference for scale != 1:1 */
    bool scale1 = ( m_scaleOpt->GetSelection() == 1 );

    m_plotSheetRef->Enable( scale1 );

    if( !scale1 )
        m_plotSheetRef->SetValue( false );
}


void DIALOG_PLOT::OnOutputDirectoryBrowseClicked( wxCommandEvent& event )
{
    // Build the absolute path of current output plot directory
    // to preselect it when opening the dialog.
    wxFileName  fn( m_outputDirectoryName->GetValue() );
    wxString    path = Prj().AbsolutePath( m_outputDirectoryName->GetValue() );

    wxDirDialog dirDialog( this, _( "Select Output Directory" ), path );

    if( dirDialog.ShowModal() == wxID_CANCEL )
        return;

    wxFileName      dirName = wxFileName::DirName( dirDialog.GetPath() );

    fn = Prj().AbsolutePath( m_parent->GetBoard()->GetFileName() );
    wxString defaultPath = fn.GetPathWithSep();
    wxString msg;
    msg.Printf( _( "Do you want to use a path relative to\n\"%s\"" ),
                   GetChars( defaultPath ) );

    wxMessageDialog dialog( this, msg, _( "Plot Output Directory" ),
                            wxYES_NO | wxICON_QUESTION | wxYES_DEFAULT );

    if( dialog.ShowModal() == wxID_YES )
    {
        if( !dirName.MakeRelativeTo( defaultPath ) )
            wxMessageBox( _( "Cannot make path relative (target volume different from file volume)!" ),
                          _( "Plot Output Directory" ), wxOK | wxICON_ERROR );
    }

    m_outputDirectoryName->SetValue( dirName.GetFullPath() );
}


PlotFormat DIALOG_PLOT::getPlotFormat()
{
    // plot format id's are ordered like displayed in m_plotFormatOpt
    static const PlotFormat plotFmt[] =
    {
        PLOT_FORMAT_GERBER,
        PLOT_FORMAT_POST,
        PLOT_FORMAT_SVG,
        PLOT_FORMAT_DXF,
        PLOT_FORMAT_HPGL,
        PLOT_FORMAT_PDF
    };

    return plotFmt[ m_plotFormatOpt->GetSelection() ];
}


// Enable or disable widgets according to the plot format selected
// and clear also some optional values
void DIALOG_PLOT::SetPlotFormat( wxCommandEvent& event )
{
    // this option exist only in DXF format:
    m_DXF_plotModeOpt->Enable( getPlotFormat() == PLOT_FORMAT_DXF );

    switch( getPlotFormat() )
    {
    case PLOT_FORMAT_PDF:
    case PLOT_FORMAT_SVG:
        m_drillShapeOpt->Enable( true );
        m_plotModeOpt->Enable( false );
        setPlotModeChoiceSelection( FILLED );
        m_plotMirrorOpt->Enable( true );
        m_useAuxOriginCheckBox->Enable( false );
        m_useAuxOriginCheckBox->SetValue( false );
        m_linesWidth->Enable( true );
        m_HPGLPenSizeOpt->Enable( false );
        m_excludeEdgeLayerOpt->Enable( true );
        m_scaleOpt->Enable( false );
        m_scaleOpt->SetSelection( 1 );
        m_fineAdjustXscaleOpt->Enable( false );
        m_fineAdjustYscaleOpt->Enable( false );
        m_PSFineAdjustWidthOpt->Enable( false );
        m_plotPSNegativeOpt->Enable( true );
        m_forcePSA4OutputOpt->Enable( false );
        m_forcePSA4OutputOpt->SetValue( false );

        m_PlotOptionsSizer->Hide( m_GerberOptionsSizer );
        m_PlotOptionsSizer->Hide( m_HPGLOptionsSizer );
        m_PlotOptionsSizer->Hide( m_PSOptionsSizer );
        m_PlotOptionsSizer->Hide( m_SizerDXF_options );
        break;

    case PLOT_FORMAT_POST:
        m_drillShapeOpt->Enable( true );
        m_plotModeOpt->Enable( true );
        m_plotMirrorOpt->Enable( true );
        m_useAuxOriginCheckBox->Enable( false );
        m_useAuxOriginCheckBox->SetValue( false );
        m_linesWidth->Enable( true );
        m_HPGLPenSizeOpt->Enable( false );
        m_excludeEdgeLayerOpt->Enable( true );
        m_scaleOpt->Enable( true );
        m_fineAdjustXscaleOpt->Enable( true );
        m_fineAdjustYscaleOpt->Enable( true );
        m_PSFineAdjustWidthOpt->Enable( true );
        m_plotPSNegativeOpt->Enable( true );
        m_forcePSA4OutputOpt->Enable( true );

        m_PlotOptionsSizer->Hide( m_GerberOptionsSizer );
        m_PlotOptionsSizer->Hide( m_HPGLOptionsSizer );
        m_PlotOptionsSizer->Show( m_PSOptionsSizer );
        m_PlotOptionsSizer->Hide( m_SizerDXF_options );
        break;

    case PLOT_FORMAT_GERBER:
        m_drillShapeOpt->Enable( false );
        m_drillShapeOpt->SetSelection( 0 );
        m_plotModeOpt->Enable( false );
        setPlotModeChoiceSelection( FILLED );
        m_plotMirrorOpt->Enable( false );
        m_plotMirrorOpt->SetValue( false );
        m_useAuxOriginCheckBox->Enable( true );
        m_linesWidth->Enable( true );
        m_HPGLPenSizeOpt->Enable( false );
        m_excludeEdgeLayerOpt->Enable( true );
        m_scaleOpt->Enable( false );
        m_scaleOpt->SetSelection( 1 );
        m_fineAdjustXscaleOpt->Enable( false );
        m_fineAdjustYscaleOpt->Enable( false );
        m_PSFineAdjustWidthOpt->Enable( false );
        m_plotPSNegativeOpt->Enable( false );
        m_plotPSNegativeOpt->SetValue( false );
        m_forcePSA4OutputOpt->Enable( false );
        m_forcePSA4OutputOpt->SetValue( false );

        m_PlotOptionsSizer->Show( m_GerberOptionsSizer );
        m_PlotOptionsSizer->Hide( m_HPGLOptionsSizer );
        m_PlotOptionsSizer->Hide( m_PSOptionsSizer );
        m_PlotOptionsSizer->Hide( m_SizerDXF_options );
        break;

    case PLOT_FORMAT_HPGL:
        m_drillShapeOpt->Enable( true );
        m_plotModeOpt->Enable( true );
        m_plotMirrorOpt->Enable( true );
        m_useAuxOriginCheckBox->Enable( false );
        m_useAuxOriginCheckBox->SetValue( false );
        m_linesWidth->Enable( false );
        m_HPGLPenSizeOpt->Enable( true );
        m_excludeEdgeLayerOpt->Enable( true );
        m_scaleOpt->Enable( true );
        m_fineAdjustXscaleOpt->Enable( false );
        m_fineAdjustYscaleOpt->Enable( false );
        m_PSFineAdjustWidthOpt->Enable( false );
        m_plotPSNegativeOpt->SetValue( false );
        m_plotPSNegativeOpt->Enable( false );
        m_forcePSA4OutputOpt->Enable( true );

        m_PlotOptionsSizer->Hide( m_GerberOptionsSizer );
        m_PlotOptionsSizer->Show( m_HPGLOptionsSizer );
        m_PlotOptionsSizer->Hide( m_PSOptionsSizer );
        m_PlotOptionsSizer->Hide( m_SizerDXF_options );
        break;

    case PLOT_FORMAT_DXF:
        m_drillShapeOpt->Enable( true );
        m_plotModeOpt->Enable( false );
        setPlotModeChoiceSelection( FILLED );
        m_plotMirrorOpt->Enable( false );
        m_plotMirrorOpt->SetValue( false );
        m_useAuxOriginCheckBox->Enable( true );
        m_linesWidth->Enable( false );
        m_HPGLPenSizeOpt->Enable( false );
        m_excludeEdgeLayerOpt->Enable( true );
        m_scaleOpt->Enable( false );
        m_scaleOpt->SetSelection( 1 );
        m_fineAdjustXscaleOpt->Enable( false );
        m_fineAdjustYscaleOpt->Enable( false );
        m_PSFineAdjustWidthOpt->Enable( false );
        m_plotPSNegativeOpt->Enable( false );
        m_plotPSNegativeOpt->SetValue( false );
        m_forcePSA4OutputOpt->Enable( false );
        m_forcePSA4OutputOpt->SetValue( false );

        m_PlotOptionsSizer->Hide( m_GerberOptionsSizer );
        m_PlotOptionsSizer->Hide( m_HPGLOptionsSizer );
        m_PlotOptionsSizer->Hide( m_PSOptionsSizer );
        m_PlotOptionsSizer->Show( m_SizerDXF_options );

        OnChangeDXFPlotMode( event );
        break;
    }

    /* Update the interlock between scale and frame reference
     * (scaling would mess up the frame border...) */
    OnSetScaleOpt( event );

    Layout();
    m_MainSizer->SetSizeHints( this );
}


// A helper function to "clip" aValue between aMin and aMax
// and write result in * aResult
// return false if clipped, true if aValue is just copied into * aResult
static bool setDouble( double* aResult, double aValue, double aMin, double aMax )
{
    if( aValue < aMin )
    {
        *aResult = aMin;
        return false;
    }
    else if( aValue > aMax )
    {
        *aResult = aMax;
        return false;
    }

    *aResult = aValue;
    return true;
}


static bool setInt( int* aResult, int aValue, int aMin, int aMax )
{
    if( aValue < aMin )
    {
        *aResult = aMin;
        return false;
    }
    else if( aValue > aMax )
    {
        *aResult = aMax;
        return false;
    }

    *aResult = aValue;
    return true;
}


void DIALOG_PLOT::applyPlotSettings()
{
    REPORTER&   reporter = m_messagesPanel->Reporter();

    PCB_PLOT_PARAMS tempOptions;

    tempOptions.SetExcludeEdgeLayer( m_excludeEdgeLayerOpt->GetValue() );
    tempOptions.SetSubtractMaskFromSilk( m_subtractMaskFromSilk->GetValue() );
    tempOptions.SetPlotFrameRef( m_plotSheetRef->GetValue() );
    tempOptions.SetPlotPadsOnSilkLayer( !m_excludePadsFromSilkscreen->GetValue() );
    tempOptions.SetUseAuxOrigin( m_useAuxOriginCheckBox->GetValue() );
    tempOptions.SetPlotValue( m_plotModuleValueOpt->GetValue() );
    tempOptions.SetPlotReference( m_plotModuleRefOpt->GetValue() );
    tempOptions.SetPlotInvisibleText( m_plotInvisibleText->GetValue() );
    tempOptions.SetScaleSelection( m_scaleOpt->GetSelection() );
    tempOptions.SetDrillMarksType( static_cast<PCB_PLOT_PARAMS::DrillMarksType>
                                   ( m_drillShapeOpt->GetSelection() ) );
    tempOptions.SetMirror( m_plotMirrorOpt->GetValue() );
    tempOptions.SetPlotMode( m_plotModeOpt->GetSelection() == 1 ? SKETCH : FILLED );
    tempOptions.SetDXFPlotPolygonMode( m_DXF_plotModeOpt->GetValue() );
    tempOptions.SetPlotViaOnMaskLayer( m_plotNoViaOnMaskOpt->GetValue() );

    if( !m_DXF_plotTextStrokeFontOpt->IsEnabled() )     // Currently, only DXF supports this option
        tempOptions.SetTextMode( PLOTTEXTMODE_DEFAULT  );
    else
        tempOptions.SetTextMode( m_DXF_plotTextStrokeFontOpt->GetValue() ?
                                 PLOTTEXTMODE_DEFAULT : PLOTTEXTMODE_NATIVE );

    // Update settings from text fields. Rewrite values back to the fields,
    // since the values may have been constrained by the setters.

    // read HPLG pen size (this param is stored in mils)
    wxString    msg = m_HPGLPenSizeOpt->GetValue();
    double      tmp = DoubleValueFromString( m_userUnits, msg ) / IU_PER_MILS;

    if( !tempOptions.SetHPGLPenDiameter( tmp ) )
    {
        msg = StringFromValue( m_userUnits, tempOptions.GetHPGLPenDiameter() * IU_PER_MILS );
        m_HPGLPenSizeOpt->SetValue( msg );
        msg.Printf( _( "HPGL pen size constrained." ) );
        reporter.Report( msg, REPORTER::RPT_INFO );
    }

    // Default linewidth
    msg = m_linesWidth->GetValue();
    tmp = ValueFromString( m_userUnits, msg );

    if( !tempOptions.SetLineWidth( tmp ) )
    {
        msg = StringFromValue( m_userUnits, tempOptions.GetLineWidth() );
        m_linesWidth->SetValue( msg );
        msg.Printf( _( "Default line width constrained." ) );
        reporter.Report( msg, REPORTER::RPT_INFO );
    }

    // X scale
    double tmpDouble;
    msg = m_fineAdjustXscaleOpt->GetValue();
    msg.ToDouble( &tmpDouble );

    if( !setDouble( &m_XScaleAdjust, tmpDouble, PLOT_MIN_SCALE, PLOT_MAX_SCALE ) )
    {
        msg.Printf( wxT( "%f" ), m_XScaleAdjust );
        m_fineAdjustXscaleOpt->SetValue( msg );
        msg.Printf( _( "X scale constrained." ) );
        reporter.Report( msg, REPORTER::RPT_INFO );
    }

    ConfigBaseWriteDouble( m_config, OPTKEY_PLOT_X_FINESCALE_ADJ, m_XScaleAdjust );

    // Y scale
    msg = m_fineAdjustYscaleOpt->GetValue();
    msg.ToDouble( &tmpDouble );

    if( !setDouble( &m_YScaleAdjust, tmpDouble, PLOT_MIN_SCALE, PLOT_MAX_SCALE ) )
    {
        msg.Printf( wxT( "%f" ), m_YScaleAdjust );
        m_fineAdjustYscaleOpt->SetValue( msg );
        msg.Printf( _( "Y scale constrained." ) );
        reporter.Report( msg, REPORTER::RPT_INFO );
    }

    ConfigBaseWriteDouble( m_config, OPTKEY_PLOT_Y_FINESCALE_ADJ, m_YScaleAdjust );

    m_config->Write( OPTKEY_PLOT_CHECK_ZONES, m_zoneFillCheck->GetValue() );

    // PS Width correction
    msg = m_PSFineAdjustWidthOpt->GetValue();
    int itmp = ValueFromString( m_userUnits, msg );

    if( !setInt( &m_PSWidthAdjust, itmp, m_widthAdjustMinValue, m_widthAdjustMaxValue ) )
    {
        msg = StringFromValue( m_userUnits, m_PSWidthAdjust );
        m_PSFineAdjustWidthOpt->SetValue( msg );
        msg.Printf( _( "Width correction constrained. "
                       "The reasonable width correction value must be in a range of "
                       " [%+f; %+f] (%s) for current design rules." ),
                    To_User_Unit( m_userUnits, m_widthAdjustMinValue ),
                    To_User_Unit( m_userUnits, m_widthAdjustMaxValue ),
                    ( m_userUnits == INCHES ) ? wxT( "\"" ) : wxT( "mm" ) );
        reporter.Report( msg, REPORTER::RPT_WARNING );
    }

    // Store m_PSWidthAdjust in mm in user config
    ConfigBaseWriteDouble( m_config, CONFIG_PS_FINEWIDTH_ADJ,
                           (double)m_PSWidthAdjust / IU_PER_MM );

    tempOptions.SetFormat( getPlotFormat() );

    tempOptions.SetUseGerberProtelExtensions( m_useGerberExtensions->GetValue() );
    tempOptions.SetUseGerberAttributes( m_useGerberX2Attributes->GetValue() );
    tempOptions.SetIncludeGerberNetlistInfo( m_useGerberNetAttributes->GetValue() );
    tempOptions.SetCreateGerberJobFile( m_generateGerberJobFile->GetValue() );

    tempOptions.SetGerberPrecision( m_rbGerberFormat->GetSelection() == 0 ? 5 : 6 );

    LSET selectedLayers;
    for( unsigned i = 0; i < m_layerList.size(); i++ )
    {
        if( m_layerCheckListBox->IsChecked( i ) )
            selectedLayers.set( m_layerList[i] );
    }
    // Get a list of copper layers that aren't being used by inverting enabled layers.
    LSET disabledCopperLayers = LSET::AllCuMask() & ~m_parent->GetBoard()->GetEnabledLayers();
    // Enable all of the disabled copper layers.
    // If someone enables more copper layers they will be selected by default.
    selectedLayers = selectedLayers | disabledCopperLayers;
    tempOptions.SetLayerSelection( selectedLayers );

    tempOptions.SetNegative( m_plotPSNegativeOpt->GetValue() );
    tempOptions.SetA4Output( m_forcePSA4OutputOpt->GetValue() );

    // Set output directory and replace backslashes with forward ones
    wxString dirStr;
    dirStr = m_outputDirectoryName->GetValue();
    dirStr.Replace( wxT( "\\" ), wxT( "/" ) );
    tempOptions.SetOutputDirectory( dirStr );

    if( !m_plotOpts.IsSameAs( tempOptions, false ) )
    {
        // First, mark board as modified only for parameters saved in file
        if( !m_plotOpts.IsSameAs( tempOptions, true ) )
            m_parent->OnModify();

        // Now, save any change, for the session
        m_parent->SetPlotSettings( tempOptions );
        m_plotOpts = tempOptions;
    }
}


void DIALOG_PLOT::OnGerberX2Checked( wxCommandEvent& event )
{
    // m_useGerberNetAttributes is useless if m_useGerberX2Attributes
    // is not checked. So disabled (greyed out) when Gerber X2 gets unchecked
    // to make it clear to the user.
    if( m_useGerberX2Attributes->GetValue() )
    {
        m_useGerberNetAttributes->Enable( true );
    }
    else
    {
        m_useGerberNetAttributes->Enable( false );
        m_useGerberNetAttributes->SetValue( false );
    }
}


void DIALOG_PLOT::Plot( wxCommandEvent& event )
{
    BOARD* board = m_parent->GetBoard();

    applyPlotSettings();

    // If no layer selected, we have nothing plotted.
    // Prompt user if it happens because he could think there is a bug in Pcbnew.
    if( !m_plotOpts.GetLayerSelection().any() )
    {
        DisplayError( this, _( "No layer selected, Nothing to plot" ) );
        return;
    }

    // Create output directory if it does not exist (also transform it in
    // absolute form). Bail if it fails
    wxFileName  outputDir = wxFileName::DirName( m_plotOpts.GetOutputDirectory() );
    wxString    boardFilename = m_parent->GetBoard()->GetFileName();
    REPORTER&   reporter = m_messagesPanel->Reporter();

    if( !EnsureFileDirectoryExists( &outputDir, boardFilename, &reporter ) )
    {
        wxString msg;
        msg.Printf( _( "Could not write plot files to folder \"%s\"." ),
                    GetChars( outputDir.GetPath() ) );
        DisplayError( this, msg );
        return;
    }

    if( m_zoneFillCheck->GetValue() )
        m_parent->Check_All_Zones( this );

    m_plotOpts.SetAutoScale( false );
    m_plotOpts.SetScale( 1 );

    switch( m_plotOpts.GetScaleSelection() )
    {
    default:
        break;

    case 0:     // Autoscale option
        m_plotOpts.SetAutoScale( true );
        break;

    case 2:     // 3:2 option
        m_plotOpts.SetScale( 1.5 );
        break;

    case 3:     // 2:1 option
        m_plotOpts.SetScale( 2 );
        break;

    case 4:     // 3:1 option
        m_plotOpts.SetScale( 3 );
        break;
    }

    /* If the scale factor edit controls are disabled or the scale value
     * is 0, don't adjust the base scale factor. This fixes a bug when
     * the default scale adjust is initialized to 0 and saved in program
     * settings resulting in a divide by zero fault.
     */
    if( m_fineAdjustXscaleOpt->IsEnabled()  && m_XScaleAdjust != 0.0 )
        m_plotOpts.SetFineScaleAdjustX( m_XScaleAdjust );

    if( m_fineAdjustYscaleOpt->IsEnabled() && m_YScaleAdjust != 0.0 )
        m_plotOpts.SetFineScaleAdjustY( m_YScaleAdjust );

    if( m_PSFineAdjustWidthOpt->IsEnabled() )
        m_plotOpts.SetWidthAdjust( m_PSWidthAdjust );

    wxString file_ext( GetDefaultPlotExtension( m_plotOpts.GetFormat() ) );

    // Test for a reasonable scale value
    // XXX could this actually happen? isn't it constrained in the apply
    // function?
    if( m_plotOpts.GetScale() < PLOT_MIN_SCALE )
        DisplayInfoMessage( this,
                            _( "Warning: Scale option set to a very small value" ) );

    if( m_plotOpts.GetScale() > PLOT_MAX_SCALE )
        DisplayInfoMessage( this,
                            _( "Warning: Scale option set to a very large value" ) );

    GERBER_JOBFILE_WRITER jobfile_writer( board, &reporter );

    // Save the current plot options in the board
    m_parent->SetPlotSettings( m_plotOpts );

    wxBusyCursor dummy;

    for( LSEQ seq = m_plotOpts.GetLayerSelection().UIOrder();  seq;  ++seq )
    {
        PCB_LAYER_ID layer = *seq;

        // All copper layers that are disabled are actually selected
        // This is due to wonkyness in automatically selecting copper layers
        // for plotting when adding more than two layers to a board.
        // If plot options become accessible to the layers setup dialog
        // please move this functionality there!
        // This skips a copper layer if it is actually disabled on the board.
        if( ( LSET::AllCuMask() & ~board->GetEnabledLayers() )[layer] )
            continue;

        // Pick the basename from the board file
        wxFileName fn( boardFilename );

        // Use Gerber Extensions based on layer number
        // (See http://en.wikipedia.org/wiki/Gerber_File)
        if( m_plotOpts.GetFormat() == PLOT_FORMAT_GERBER && m_useGerberExtensions->GetValue() )
            file_ext = GetGerberProtelExtension( layer );

        BuildPlotFileName( &fn, outputDir.GetPath(), board->GetLayerName( layer ), file_ext );
        wxString fullname = fn.GetFullName();
        jobfile_writer.AddGbrFile( layer, fullname );

        LOCALE_IO toggle;

        PLOTTER*    plotter = StartPlotBoard( board, &m_plotOpts, layer, fn.GetFullPath(), wxEmptyString );

        // Print diags in messages box:
        wxString msg;

        if( plotter )
        {
            PlotOneBoardLayer( board, plotter, layer, m_plotOpts );
            plotter->EndPlot();
            delete plotter;

            msg.Printf( _( "Plot file \"%s\" created." ), GetChars( fn.GetFullPath() ) );
            reporter.Report( msg, REPORTER::RPT_ACTION );
        }
        else
        {
            msg.Printf( _( "Unable to create file \"%s\"." ), GetChars( fn.GetFullPath() ) );
            reporter.Report( msg, REPORTER::RPT_ERROR );
        }
    }

    if( m_plotOpts.GetFormat() == PLOT_FORMAT_GERBER && m_plotOpts.GetCreateGerberJobFile() )
    {
        // Pick the basename from the board file
        wxFileName fn( boardFilename );
        // Build gerber job file from basename
        BuildPlotFileName( &fn, outputDir.GetPath(), "job", GerberJobFileExtension );
        jobfile_writer.CreateJobFile( fn.GetFullPath() );
    }
}


void DIALOG_PLOT::onRunDRC( wxCommandEvent& event )
{
    PCB_EDIT_FRAME* parent = dynamic_cast<PCB_EDIT_FRAME*>( GetParent() );

    if( parent )
    {
        // First close an existing dialog if open
        // (low probability, but can happen)
        parent->GetDrcController()->DestroyDRCDialog( wxID_OK );

        // Open a new drc dialod, with the right parent frame, and in Modal Mode
        parent->GetDrcController()->ShowDRCDialog( this );
    }
}

