#include <stdlib.h>
#include "img.h"

#define MAXLINE 64
#define SPACE_CHAR ' '

void pbm_image_free(PbmImage* img) {
	free(img->data);
	free(img);
}

PbmImage* pbm_image_load_from_stream(FILE* stream, int* error) {
	// Magic byte
	char magicByte[3];
	
	// Image height
	int imgHeight;
	
	// Image width
	int imgWidth;
	
	// current line in the file
	char line[MAXLINE];
	
	// are we in meta-data mode?
	int metaData = 1;
	
	// image data
	char *imgData;
	
	// line count
	int lineCount = 1;
	
	
	// Read file per line
    while (!feof (stream)) {
        if (metaData) {
		     if (fgets(line, sizeof (line), stream)) {
		         #ifdef DEBUG
        			printf("Line: %s", line);
        		 #endif
        		 
        		 if (lineCount == 1) {
    				 magicByte[0] = line[0];
    				 magicByte[1] = line[1];
    				 magicByte[2] = '\0';
    				 
    				 // check if magic byte equals our supported file format
    				 if (strcmp(PBM_TYPE_P5, magicByte) != 0) {
    					 *error = RET_UNSUPPORTED_FILE_FORMAT;
    				 }
    				 
    				 lineCount++;
    			 } else {
    				 // ignore line if it starts with comment character
    				 if (line[0] == PBM_COMMENT_CHAR) {
    					 #ifdef DEBUG
    						printf("Comment line!\n");
    					#endif
    				 } else {
    					 // get height and width
    					 if (lineCount == 2) {
    						 int parseHeight = 1;
    						 int parseWidth = 1;
    						 int charCount = 0;
    						 int offs = 0;
    						 
    						 char charHeight[MAXLINE];
    						 char charWidth[MAXLINE];
    						 
    						 while (parseWidth) {
    							 if ((line[charCount] == SPACE_CHAR) || (line[charCount] == '\n')) {
    								 parseWidth = 0;
    								 
    								 charWidth[charCount] = '\0';
    								 
    								 offs = charCount + 1;
    								 charCount = 0;
    							 } else {
    								 charWidth[charCount] = line[charCount];
    								 charCount++;
    							 }
    						 }
    						 
    						 while (parseHeight) {
    							 if ((line[charCount + offs] == SPACE_CHAR) || (line[charCount + offs] == '\n')) {
    								 parseHeight = 0;
    								 
    								 charHeight[charCount + offs] = '\0';
    							 } else {
    								 charHeight[charCount] = line[charCount + offs];
    								 charCount++;
    							 }
    						 }
    						 
    						 imgWidth = atoi(charWidth);
    						 imgHeight = atoi(charHeight);
    						 
    						 // initialize img data array with proper size
    						 imgData = malloc(sizeof(char)*(imgWidth * imgHeight));
    						 
    						 if (imgData == NULL) {
    							 *error = RET_OUT_OF_MEMORY;
    						 }
    						 
    						 #ifdef DEBUG
    							printf("Height: %i, Width: %i\n", imgHeight, imgWidth);
    						 #endif
    					 } else if (lineCount == 3) {
    						 // we can ignore this line since bit depth has to be 255 in file type P5
    						 // from now on, we receive the image data
    						 metaData = 0;
    					 }
    					 
    					 lineCount++;
    				 }
    			 }
		     }
		 } else {
			 // image data			 
			 size_t bytesRead = fread(imgData, sizeof(char), imgWidth * imgHeight, stream);
			 
			 #ifdef DEBUG
				printf("%zu bytes read", bytesRead);
			 #endif
			 
			 // we have read everything we want to know, we can exit now
			 break;
		 }
    }
    
    // Add new line for pretty printing
    #ifdef DEBUG
		printf("\n");
	 #endif
	 
	 PbmImage *img;
	 img = (PbmImage*)malloc(sizeof(PbmImage));
	 
	 if (img == NULL) {
		 *error = RET_OUT_OF_MEMORY;
	 }
	 
	 strcpy(img->type, magicByte);
	 img->width = imgWidth;
	 img->height = imgHeight;
	 img->data = imgData;
	
	return img; 
}

int pbm_image_write_to_stream(PbmImage* img, FILE* targetStream) {
	// put magic byte into the file
	if (fputs(img->type, targetStream) == EOF)
		return RET_PBM_ERROR;
	
	if (fputc('\n', targetStream) == EOF)
		return RET_PBM_ERROR;
	
	// put a comment in there
	if (fputs("# Flipped by ITM13 :-)", targetStream) == EOF)
		return RET_PBM_ERROR;
	
	if (fputc('\n', targetStream) == EOF)
		return RET_PBM_ERROR;
	
	// put width and height
	if (fprintf(targetStream, "%d %d", img->width, img->height) == EOF)
		return RET_PBM_ERROR;
	
	if (fputc('\n', targetStream) == EOF)
		return RET_PBM_ERROR;
	
	// put bit-depth
	if (fprintf(targetStream, "%d", PBM_BIT_DEPTH) == EOF)
		return RET_PBM_ERROR;
	
	if (fputc('\n', targetStream) == EOF)
		return RET_PBM_ERROR;
	
	// put data
	for (int i = 0 ; i < (img->width * img->height); i++) {
		if (fputc(img->data[i], targetStream) == EOF)
			return RET_PBM_ERROR;
	}
	
	return RET_PBM_OK;
}
