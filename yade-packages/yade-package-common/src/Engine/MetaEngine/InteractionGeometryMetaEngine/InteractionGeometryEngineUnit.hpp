/***************************************************************************
 *   Copyright (C) 2004 by Janek Kozicki                                   *
 *   cosurgi@berlios.de                                                    *
 *   Copyright (C) 2004 by Olivier Galizzi                                 *
 *   olivier.galizzi@imag.fr                                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __INTERACTIONGEOMETRYFUNCTOR_H__
#define __INTERACTIONGEOMETRYFUNCTOR_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <yade/yade-core/InteractingGeometry.hpp>
#include <yade/yade-lib-wm3-math/Se3.hpp>
#include <yade/yade-core/Interaction.hpp>
#include <yade/yade-core/EngineUnit2D.hpp>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <boost/shared_ptr.hpp>
#include <string>

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/*! \brief Abstract interface for all interaction functor.

	Every functions that describe interaction between two InteractionGeometries must derive from InteractionGeometryEngineUnit.
*/

class InteractionGeometryEngineUnit : 	public EngineUnit2D
					<
		 				bool ,
		 				TYPELIST_5(	  const shared_ptr<InteractingGeometry>&
								, const shared_ptr<InteractingGeometry>&
								, const Se3r&
								, const Se3r&
								, const shared_ptr<Interaction>&
			  				  ) 
					>
{
	REGISTER_CLASS_NAME(InteractionGeometryEngineUnit);
	REGISTER_BASE_CLASS_NAME(EngineUnit2D);
};

REGISTER_SERIALIZABLE(InteractionGeometryEngineUnit,false);

#endif // __INTERACTIONGEOMETRYFUNCTOR_H__

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////