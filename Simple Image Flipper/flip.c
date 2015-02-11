#include "flip.h"

int pbm_image_flip(PbmImage* image) {
	// check supported file format
	if (strcmp(image->type, PBM_TYPE_P5) > 0) {
		return RET_UNSUPPORTED_FILE_FORMAT;
	}
	
	// check if width and height are set
	if (image->width <= 0 || image->height <= 0) {
		return RET_PBM_ERROR;
	}
	
	int n = image->width * image->height;
	int end = n - 1;
	char temp;
	
	for (int c = 0; c < (n / 2); c++) {
		temp = image->data[c];
		image->data[c] = image->data[end];
		image->data[end] = temp;
		
		end--;
	}
	
	return RET_PBM_OK;
}
