/*
 * jpeg.h
 *
 *  Created on: 28. pro 2017.
 *      Author: Renchy
 */

#ifndef SRC_JPEG_H_
#define SRC_JPEG_H_



#endif /* SRC_JPEG_H_ */

void toRGB(unsigned char *slika1 , unsigned char *slika2){

	int i,j;
	j=0;
	for(i=0;i<(480*640*3);i+=3){
			 slika2[i]	=	(slika1[j] & 0xF8);
			 slika2[i+1]	=	(slika1[j]<<5 | slika1[i+1]>>3) & 0xFC;
			 slika2[i+2]	=	(slika1[j+1]<<3) & 0xF8;
		     j+=2;


	}

}


void criptImage(unsigned char *slika, int brojac){

	    int i;
		for(i=0;i<=brojac;i++){
			slika[i] ^= 0xCC;
		}

}


