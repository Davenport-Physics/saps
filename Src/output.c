/*
*    Semi-accurate physics simulator (SAPS)
*
*    Copyright (C) 2014 Michael Davenport <Davenport.physics@gmail.com>
*
*
*   Permission is hereby granted, free of charge, to any person 
*   obtaining a copy of this software and associated documentation 
*   files (the "Software"), to deal in the Software without 
*   restriction, including without limitation the rights to use, 
*   copy, modify, merge, publish, distribute, sublicense, and/or 
*   sell copies of the Software, and to permit persons to whom 
*   the Software is furnished to do so, subject to the following 
*   conditions:
*
*   The above copyright notice and this permission notice shall be 
*   included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
*   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
*   OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
*   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
*   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
*   FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR 
*   OTHER DEALINGS IN THE SOFTWARE.
*
*/

#include "output.h"

char *filename;

void file_init() {
	
	filename = ( char * )malloc( 100 * sizeof( char ) );
	
}

/*
 * 
 * Saves data to a flatfile for the future possibility of just
 * focusing on rendering as opposed to both rendering and the calculations
 * themselves.
 * 
 * */
int data_save( struct particle_attributes *this, int length ) {
	
	int x;
	
	if ( filename == NULL ) {
	
		return -1;
		
	}
	
	FILE *fp = fopen( filename , "w+" );
	
	for ( x = 0; x < length; x++ ) {
	
		fprintf(fp, "[%d,%d,%lf,%lf]" , this[x].type , this[x].index , this[x].mass, this[x].charge);
	
	}
	
	fclose(fp);
	
	return 1;
	
}

//fscanf(fp, "%s %s %s %d", str1, str2, str3, &year);

void file_quit() {
	
	free( filename );
	
	filename = NULL;
	
}

