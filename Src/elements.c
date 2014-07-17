/*
 * elements.c
 * 
 * Copyright 2013 Michael Davenport <Davenport.physics@gmail.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include "elements.h"

#define HYDROGEN 0
#define HELIUM 1

void initialize_elements(int element, int num) {
	
	int x;
	switch ( element ) {
		
		case HYDROGEN: hydrogen = ( struct attributes *)malloc( num * sizeof( struct attributes ) ); break;
	
	}
	
	switch ( element ) {
	
		case HYDROGEN:
		
			for ( x = 0; x < num;x++ ) {
			
				hydrogen[x].numElectrons = 1;
				hydrogen[x].numProtons = 1;
				hydrogen[x].numNeutron = 0;
				
			}
		break;
		
	}
	
	
}
