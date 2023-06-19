#include <stdio.h>
#include <stdlib.h>
#include "imageReconstruction.h" 
#include "shadowGeneration.h" 
#include "BMPImage.h"
#include <time.h>

//main.o operacion imagen.bmp k carpeta_de_imagenes
int main(int argc, char* argv[]){
    srand(time(NULL));   // Initialization, should only be called once.
    if(argc != 5){
        printf("Error: numero de argumentos invalido\n");
        return 1;
    }
    char operation = argv[1][0];
    char* image = argv[2];
    int k = atoi(argv[3]);
    char* imagesDirectory = argv[4];

    printf("operacion: %c\n", operation);
    printf("imagen: %s\n", image);
    printf("k: %d\n", k);
    printf("carpeta: %s\n", imagesDirectory);

    int n = 3;
    char * filenames[3] = {"../images/Johnshare.bmp", "../images/Jamesshare.bmp", "../images/Carlitosshare.bmp"};

    BMPImage ** participants = malloc(sizeof(BMPImage*)*n);
    int width, height;
    for(int i = 0; i < n; i++){
        participants[i] = readBMP(filenames[i]);
        if(i > 0 && (width != participants[i]->width || height != participants[i]->height)){
            printf("Error: las imagenes deben tener el mismo tamaño\n");
            return 1;
        }
        if(participants[i]->bitsPerPixel != 8){
            printf("Error: las imagenes deben tener 8 bits por pixel\n");
            return 1;
        }

        width = participants[i]->width;
        height = participants[i]->height;

        if (participants[i] == NULL) {
            
            printf("Failed to read the BMP image.\n");
            return 1;
        }
    }

    if(operation == 'd'){
        // Read the BMP image
        const char * imageToHide = image;
        BMPImage* hiddenImage = readBMP(imageToHide);
        if (hiddenImage == NULL) {
            printf("Failed to read the BMP image.\n");
            return 1;
        }

        Shadow * shadows = generateShadowsFromFile(hiddenImage, k, n);

        for(int i = 0; i < n; i++){
            hideShadowInImage(participants[i], shadows[i], k);
        }
    } else if(operation == 'r'){

        //BMPImage * reconstructedImage = reconstructImage(image, participants, k);
        reconstructImage(image, participants, k);
    } else {

        printf("Error: operacion invalida. Debería ser: \'d\' para esconder la file o \'r\' para recuperarla\n");
        return 1;
        // print error and do nothing
    }

    //printf("HIDDEN\n----------------\n");
    //printImageData(hiddenImage);

    // Shadow * shadows = generateShadowsFromFile(hiddenImage, k, n);

    // for(int i = 0; i < n; i++){
    //     hideShadowInImage(participants[i], shadows[i], k);
    // }
    
    //printf("\n-------------------\nRECONSTRUCTED\n----------------\n");
    // BMPImage * reconstructedImage = reconstructImage(participants, k);
    //printImageData(reconstructedImage);
    return 0;
}