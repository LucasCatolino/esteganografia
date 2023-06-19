#include "./imageReconstruction.h"
#include "./shadowGeneration.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Gauss.h"

int extractLSB4(unsigned char * img){
    unsigned char mask = 0x0F;  // Mask to extract the 4 least significant bits
    unsigned char * A = img;
    unsigned char * B = img + 1;
    unsigned char firstBits = *A & mask; 
    unsigned char lastBits = *B & mask;  
    return (firstBits << 4) | lastBits;
}

int extractLSB2(unsigned char * img){
    unsigned char mask = 0x03; // 00000011

    unsigned char * A = img;
    unsigned char * B = img+1;
    unsigned char * C = img+2;
    unsigned char * D = img+3;

    unsigned char bitsA = *A & mask;
    unsigned char bitsB = *B & mask;
    unsigned char bitsC = *C & mask;
    unsigned char bitsD = *D & mask;

    return (bitsA << 6) | (bitsB << 4) | (bitsC << 2) | bitsD;
}


Shadow extractShadowFromImage(BMPImage * img,  int k){
    unsigned char * image = img->data;
    Shadow shadow;
    shadow.shadowNumber = img->shadowNumber;
    int t = (img->width * img->height)/(2*k-2);
    shadow.t = t;
    shadow.shadow = malloc(t * sizeof(V));
    for(int i = 0; i < t; i++){
        if(k < 5) {
            shadow.shadow[i].m = extractLSB4(image);
            image = image+2;
            shadow.shadow[i].d = extractLSB4(image);
            image = image+2;
        } else {
            shadow.shadow[i].m = extractLSB2(image);
            image = image+4;
            shadow.shadow[i].d = extractLSB2(image);
            image = image+4;
        }
    }
    return shadow;
}


void printBlock(ImageBlock block, int k){
    printf("BLOCK %d: \n", block.blockNumber);
    for(int i = 0; i < k; i++){
        printf("%d ", block.f[i]);

    }
    for(int i = 0; i < k; i++){
        // if(i>1){
            printf("%d ", block.g[i]);
        // }
    }
    printf("\n");
}


ImageBlock * reconstructBlocks(Shadow * shadows, int k){
    int t = shadows[0].t;
    ImageBlock * blocks = malloc(t * sizeof(ImageBlock));
    for(int i = 0; i < t; i++){
        blocks[i].blockNumber = i;
        blocks[i].f = malloc(k * sizeof(int));
        blocks[i].g = malloc(k * sizeof(int));
        Point * fpoints = malloc(k * sizeof(Point));
        Point * gpoints = malloc(k * sizeof(Point));
        for(int j = 0; j < k; j++){
            fpoints[j].x = shadows[j].shadowNumber;
            gpoints[j].x = shadows[j].shadowNumber;
            fpoints[j].y = shadows[j].shadow[i].m;
            gpoints[j].y = shadows[j].shadow[i].d;
        }

        polynomialCoefficients(fpoints,blocks[i].f,k-1); 
        polynomialCoefficients(gpoints,blocks[i].g,k-1);
        
        // printBlock(blocks[i],k);

        free(fpoints);
        free(gpoints);
    }
    return blocks;
}


unsigned char * buildImageFromBlocks(ImageBlock * blocks, int t, int k){
    unsigned char * image = malloc(t * (2*k-2) *sizeof(unsigned char));
    int index = 0;
    for(int i = 0; i < t; i++){
        for(int j=0;j < k; j++){
            image[index++] = (unsigned char) blocks[i].f[j]; 
        }
        for(int j=0; j < k; j++){
            if(j>=2){
                image[index++] = (unsigned char) blocks[i].g[j];
            }
        }
    }
    return image;
}

//BMPImage * reconstructImage(char * reconstructdImage, BMPImage ** images, int k){
void reconstructImage(char * reconstructdImage, BMPImage ** images, int k){

    Shadow * shadows = malloc(k * sizeof(Shadow));
    for(int i = 0; i < k; i++){
        shadows[i] = extractShadowFromImage(images[i],k);
    }
    
    ImageBlock * blocks = reconstructBlocks(shadows,k);

    unsigned char * image = buildImageFromBlocks(blocks, shadows[0].t, k);

    BMPImage * img = malloc(sizeof(BMPImage));
    
    // img->fileHeader = images[0]->fileHeader;
    img->bitsPerPixel = images[0]->bitsPerPixel;
    img->height = images[0]->height;
    img->width = images[0]->width;
    img->data = malloc(images[0]->width * images[0]->height );
    img->shadowNumber = images[0]->shadowNumber;
    img->filename = reconstructdImage;
    img->fileHeader = images[0]->fileHeader;
    img->infoHeader = images[0]->infoHeader;
    img->metadata = images[0]->metadata;

    // printf("Image header: %s\n", img->fileHeader);
    // printf("Image width: %d\n", img->width);
    // printf("Image height: %d\n", img->height);
    // printf("Image bits per pixel: %d\n", img->bitsPerPixel);

    memcpy(img->data, image, images[0]->width * images[0]->height);

    createBMPFile(img);

    //return img;
}



