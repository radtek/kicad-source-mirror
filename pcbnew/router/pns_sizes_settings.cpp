/*
 * KiRouter - a push-and-(sometimes-)shove PCB router
 *
 * Copyright (C) 2014 CERN
 * Copyright (C) 2016 KiCad Developers, see AUTHORS.txt for contributors.
 * Author: Tomasz Wlostowski <tomasz.wlostowski@cern.ch>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <class_board.h>

#include "pns_item.h"
#include "pns_via.h"
#include "pns_solid.h"
#include "pns_node.h"
#include "pns_sizes_settings.h"

namespace PNS {

int SIZES_SETTINGS::inheritTrackWidth( ITEM* aItem )
{
    VECTOR2I p;

    assert( aItem->Owner() != NULL );

    switch( aItem->Kind() )
    {
    case ITEM::VIA_T:
        p = static_cast<VIA*>( aItem )->Pos();
        break;

    case ITEM::SOLID_T:
        p = static_cast<SOLID*>( aItem )->Pos();
        break;

    case ITEM::SEGMENT_T:
        return static_cast<SEGMENT*>( aItem )->Width();

    default:
        return 0;
    }

    JOINT* jt = static_cast<NODE*>( aItem->Owner() )->FindJoint( p, aItem );

    assert( jt != NULL );

    int mval = INT_MAX;


    ITEM_SET linkedSegs = jt->Links();
    linkedSegs.ExcludeItem( aItem ).FilterKinds( ITEM::SEGMENT_T );

    for( ITEM* item : linkedSegs.Items() )
    {
        int w = static_cast<SEGMENT*>( item )->Width();
        mval = std::min( w, mval );
    }

    return ( mval == INT_MAX ? 0 : mval );
}


void SIZES_SETTINGS::Init( BOARD* aBoard, ITEM* aStartItem, int aNet )
{
    BOARD_DESIGN_SETTINGS &bds = aBoard->GetDesignSettings();

    NETCLASSPTR netClass;
    int net = aNet;

    if( aStartItem )
        net = aStartItem->Net();

    if( net >= 0 )
    {
        NETINFO_ITEM* ni = aBoard->FindNet( net );

        if( ni )
        {
            wxString netClassName = ni->GetClassName();
            netClass = bds.m_NetClasses.Find( netClassName );
        }
    }

    if( !netClass )
        netClass = bds.m_NetClasses.GetDefault();

    m_trackWidth = 0;

    if( bds.m_UseConnectedTrackWidth && aStartItem != NULL )
    {
        m_trackWidth = inheritTrackWidth( aStartItem );
    }

    if( !m_trackWidth && ( bds.UseNetClassTrack() && netClass != NULL ) ) // netclass value
    {
        m_trackWidth = netClass->GetTrackWidth();
    }

    if( !m_trackWidth )
    {
        m_trackWidth = bds.GetCurrentTrackWidth();
    }

    if( bds.UseNetClassVia() && netClass != NULL )   // netclass value
    {
        m_viaDiameter = netClass->GetViaDiameter();
        m_viaDrill    = netClass->GetViaDrill();
    }
    else
    {
        m_viaDiameter = bds.GetCurrentViaSize();
        m_viaDrill    = bds.GetCurrentViaDrill();
    }

    if( bds.UseNetClassDiffPair() && netClass != NULL )
    {
        m_diffPairWidth  = netClass->GetDiffPairWidth();
        m_diffPairGap    = netClass->GetDiffPairGap();
        m_diffPairViaGap = netClass->GetDiffPairViaGap();
    }
    else if( bds.UseCustomDiffPairDimensions() )
    {
        m_diffPairWidth  = bds.GetCustomDiffPairWidth();
        m_diffPairGap    = bds.GetCustomDiffPairGap();
        m_diffPairViaGap = bds.GetCustomDiffPairViaGap();
    }
    else
    {
        m_diffPairWidth  = bds.m_DiffPairDimensionsList[ bds.GetDiffPairIndex() ].m_Width;
        m_diffPairGap    = bds.m_DiffPairDimensionsList[ bds.GetDiffPairIndex() ].m_Gap;
        m_diffPairViaGap = bds.m_DiffPairDimensionsList[ bds.GetDiffPairIndex() ].m_ViaGap;
    }

    m_layerPairs.clear();
}


void SIZES_SETTINGS::ClearLayerPairs()
{
    m_layerPairs.clear();
}


void SIZES_SETTINGS::AddLayerPair( int aL1, int aL2 )
{
    int top = std::min( aL1, aL2 );
    int bottom = std::max( aL1, aL2 );

    m_layerPairs[bottom] = top;
    m_layerPairs[top] = bottom;
}


void SIZES_SETTINGS::ImportCurrent( BOARD_DESIGN_SETTINGS& aSettings )
{
    m_trackWidth = aSettings.GetCurrentTrackWidth();
    m_viaDiameter = aSettings.GetCurrentViaSize();
    m_viaDrill = aSettings.GetCurrentViaDrill();

    m_diffPairWidth = aSettings.GetCurrentDiffPairWidth();
    m_diffPairGap = aSettings.GetCurrentDiffPairGap();
    m_diffPairViaGap = aSettings.GetCurrentDiffPairViaGap();
}


int SIZES_SETTINGS::GetLayerTop() const
{
    if( m_layerPairs.empty() )
        return F_Cu;
    else
        return m_layerPairs.begin()->first;
}


int SIZES_SETTINGS::GetLayerBottom() const
{
    if( m_layerPairs.empty() )
        return B_Cu;
    else
        return m_layerPairs.begin()->second;
}

}
