#include <stdio.h>
#include <stdlib.h>
#include "BMPImage.h"


void printBMPFileHeader(BITMAPFILEHEADER *fileHeader) {
  printf("File header:\n");
  printf("  bfType: %x\n", fileHeader->bfType);
  printf("  bfSize: %d\n", fileHeader->bfSize);
  printf("  bfReserved1: %d\n", fileHeader->bfReserved1);
  printf("  bfReserved2: %d\n", fileHeader->bfReserved2);
  printf("  bfOffBits: %d\n", fileHeader->bfOffBits);
}

void printBMPInfoHeader(BITMAPINFOHEADER *infoHeader) {
  printf("Info header:\n");
  printf("  biSize: %d\n", infoHeader->biSize);
  printf("  biWidth: %d\n", infoHeader->biWidth);
  printf("  biHeight: %d\n", infoHeader->biHeight);
  printf("  biPlanes: %d\n", infoHeader->biPlanes);
  printf("  biBitCount: %d\n", infoHeader->biBitCount);
  printf("  biCompression: %d\n", infoHeader->biCompression);
  printf("  biSizeImage: %d\n", infoHeader->biSizeImage);
  printf("  biXPelsPerMeter: %d\n", infoHeader->biXPelsPerMeter);
  printf("  biYPelsPerMeter: %d\n", infoHeader->biYPelsPerMeter);
  printf("  biClrUsed: %d\n", infoHeader->biClrUsed);
  printf("  biClrImportant: %d\n", infoHeader->biClrImportant);
}


void createBMPFile(BMPImage *image) {
  // Open the file.
  FILE* file = fopen(image->filename,"wb");
  if (file == NULL) {
    printf("Could not open file: %s\n", image->filename);
    exit(EXIT_FAILURE);
  }
  
  // Write the file header.
  BITMAPFILEHEADER fileHeader;
  fileHeader.bfType = image->fileHeader.bfType;
  fileHeader.bfSize = image->fileHeader.bfSize;
  fileHeader.bfReserved1 = image->shadowNumber; //GUARDO el shadowNumber en el campo bfReserved1
  fileHeader.bfReserved2 = image->fileHeader.bfReserved2;
  fileHeader.bfOffBits = image->fileHeader.bfOffBits;//sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
  fwrite(&fileHeader, sizeof(fileHeader), 1, file);
  
  // Write the info header.
  BITMAPINFOHEADER infoHeader;
  infoHeader.biSize = sizeof(BITMAPINFOHEADER);
  infoHeader.biWidth = image->infoHeader.biWidth;
  infoHeader.biHeight = image->infoHeader.biHeight;
  infoHeader.biPlanes = image->infoHeader.biPlanes;
  infoHeader.biBitCount = image->bitsPerPixel;
  infoHeader.biCompression = image->infoHeader.biCompression;
  infoHeader.biSizeImage = image->infoHeader.biSizeImage;
  infoHeader.biXPelsPerMeter = image->infoHeader.biXPelsPerMeter;
  infoHeader.biYPelsPerMeter = image->infoHeader.biYPelsPerMeter;
  infoHeader.biClrUsed = image->infoHeader.biClrUsed; //0
  infoHeader.biClrImportant = image->infoHeader.biClrImportant; //0
  fwrite(&infoHeader, sizeof(infoHeader), 1, file);

  // Write the image data.
  fwrite(image->metadata, 1024, 1, file); //escribo los 1024 bytes de metadata (que son 0s

  fwrite(image->data, image->width * image->height, 1, file);

  // Close the file.
  fclose(file);
}

BMPImage *readBMP(const char* filename) {
  // Open the file.
  FILE* file = fopen(filename,"rb");
  if (file == NULL) {
    printf("Could not open file: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  
  // Read the file header.
  BITMAPFILEHEADER fileHeader;
  fread(&fileHeader, sizeof(fileHeader), 1, file);
  if (fileHeader.bfType != 0x4D42) {
    // Not a valid BMP file.
    printf("%s is not a valid BMP file\n", filename);
    fclose(file);
    exit(EXIT_FAILURE);
  }
  
  // Read the info header.
  BITMAPINFOHEADER infoHeader;
  fread(&infoHeader, sizeof(infoHeader), 1, file);

  // Read the image data.
  unsigned char * metadata = malloc(1024 * sizeof(unsigned char));
  fread(metadata, 1024 ,1,file);
  
  unsigned char* data = malloc(infoHeader.biSizeImage); 
  fread(data, infoHeader.biSizeImage, 1, file);
  fclose(file); 

  // Create the image structure.
  BMPImage *image = malloc(sizeof(BMPImage));
  image->width = infoHeader.biWidth;
  image->height = infoHeader.biHeight;
  image->bitsPerPixel = infoHeader.biBitCount;
  image->data = data;
  image->filename = (char *)filename;
  image->shadowNumber = fileHeader.bfReserved1;
  image->fileHeader = fileHeader;
  image->infoHeader = infoHeader;
  image->metadata= metadata;
  
  return image;
}


void printImageData(BMPImage * img){
  int imagePixels = img->width * img->height;
  for(int i = 0; i < imagePixels; i++){
    printf("%hhu-", img->data[i]);
  }
  printf("\n");
}



