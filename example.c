/* Example use of Reed-Solomon library 
 * (modified for embedded devices)
 * Copyright Henry Minsky (hqm@alum.mit.edu) 1991-2009
 *
 * This software library is licensed under terms of the GNU GENERAL
 * PUBLIC LICENSE
 *
 * RSCODE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * RSCODE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Rscode.  If not, see <http://www.gnu.org/licenses/>.

 * Commercial licensing is available under a separate license, please
 * contact author for details.
 *
 * This same code demonstrates the use of the encodier and 
 * decoder/error-correction routines. 
 *
 * We are assuming we have at least four bytes of parity (NPAR >= 4).
 * 
 * This gives us the ability to correct up to two errors, or 
 * four erasures. 
 *
 * In general, with E errors, and K erasures, you will need
 * 2E + K bytes of parity to be able to correct the codeword
 * back to recover the original message data.
 *
 * You could say that each error 'consumes' two bytes of the parity,
 * whereas each erasure 'consumes' one byte.
 *
 * Thus, as demonstrated below, we can inject one error (location unknown)
 * and two erasures (with their locations specified) and the 
 * error-correction routine will be able to correct the codeword
 * back to the original message.
 * */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rscombo.h"
 
unsigned char msg[] = "Nervously I loaded the twin ducks aboard the revolving platform.";

#define ML (sizeof (msg) + NPAR)

unsigned char codeword[ML];
 
/* Some debugging routines to introduce errors or erasures
   into a codeword. 
   */

/* Introduce a byte error at LOC */
void
byte_err (int err, int loc, unsigned char *dst)
{
	printf("Adding Error at loc %d, data %#x\n", loc, dst[loc-1]);
	dst[loc-1] ^= err;
}

/* Pass in location of error (first byte position is
	labeled starting at 1, not 0), and the codeword.
*/

void insert_erasure(unsigned char codeword[], int csize, int eloc,  unsigned char eraselist[], int *numerase)
{
	codeword[ eloc - 1 ] = '~';
	eraselist[*numerase] =  csize - eloc;
	(*numerase)++;
}



void main (void)
{

	unsigned char erasures[NPAR];
	int nerasures,i;

	/* Initialization the RS library */
	rs_init();

	/* ************** */

	/* Encode data into codeword, adding NPAR parity bytes */
	rs_encode_data(msg, sizeof(msg), codeword);

	printf("Encoded data is: \"%s\"\n", codeword);

	/* We need to indicate the position of the erasures.  Eraseure
	  positions are indexed (1 based) from the end of the message... */

	for(nerasures = 0, i=1; i <= NPAR; i++)
	{
		insert_erasure(codeword, ML, i*2, erasures, &nerasures);
	}
	printf("with erasures: \"%s\"\n", codeword);

	/* Now decode -- encoded codeword size must be passed */
	/* check if syndrome is all zeros */
	if ( rs_decode_data(codeword, ML) ) {
		rs_correct_errors_erasures (codeword,
				  ML,
				  nerasures,
				  erasures);

	 printf("Corrected codeword: \"%s\"\n", codeword);
	}
	else
	{
		printf("No Errors\n");
	}
	if( strcmp(msg,codeword) )
				printf("Error in corrected compare\n");


	/* Encode data into codeword, adding NPAR parity bytes */
	rs_encode_data(msg, sizeof(msg), codeword);

	printf("Encoded data is: \"%s\"\n", codeword);

	for(nerasures = 0, i=1; i <=NPAR/2; i++)
		byte_err(0x35, i+20, codeword);

	printf("with some errors: \"%s\"\n", codeword);

	/* Now decode -- encoded codeword size must be passed */
	/* check if syndrome is all zeros */
	if ( rs_decode_data(codeword, ML) ) {
		rs_correct_errors_erasures (codeword,
				  ML,
				  nerasures,
				  erasures);

		printf("Corrected codeword: \"%s\"\n", codeword);
	}
	else
	{
		printf("No Errors\n");
	}
	if( strcmp(msg,codeword) )
				printf("Error in corrected compare\n");
	exit(0);
}

