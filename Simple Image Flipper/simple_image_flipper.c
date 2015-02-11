#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "img.h"
#include "flip.h"

int main(int argc, char* argv[]){

	if(argc != 3){
		printf("Usage: simple_image_flipper <string: path to input file> <string: path to output file>\n");
		return 1;
	}
	
	printf("Starting the Simple Image Flipper\n");
	
	// Read filenames into memory
	char infilename[60];
	char outfilename[60];
	
	strcpy(infilename, argv[1]);
	strcpy(outfilename, argv[2]);
	
	#ifdef DEBUG
		printf("Input file: %s\n", infilename);
		printf("Output file: %s\n", outfilename);
	#endif
	
	// declare file pointers for input and output file
	FILE *imgfile;
	FILE *flippedfile;
	
	// Our PbmImage
	PbmImage* pbmimg;
	int readErr = RET_PBM_OK;
	int* rep = &readErr;
	
	// open the input file
	imgfile = fopen(infilename, "r");
	
	// load image from file stream
	pbmimg = pbm_image_load_from_stream(imgfile, rep);
	
	if (readErr != RET_PBM_OK) {
		return readErr;
	}
	
	// close input file
	fclose(imgfile);
	
	// rotate image
	pbm_image_flip(pbmimg);
	
	// open output file
	flippedfile = fopen(outfilename, "w+");
	
	// write output file
	pbm_image_write_to_stream(pbmimg, flippedfile);
	
	// close output file
	fclose(flippedfile);
	
	// free memory space allocated for pbm image
	pbm_image_free(pbmimg);
	
	// finish-message
	printf("File flipping finished!\n");
	
	return RET_PBM_OK;
}
