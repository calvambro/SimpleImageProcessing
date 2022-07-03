#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>

#define HEADER_SIZE 54
#define CHAR_NULL '\0'
#define COLOR_PALLET_SIZE 1024
#define BMP_SIGNATURE 0x4d42
#define RGB_SIZE 24
#define CHAR_RANGE 256
#define BYTE_PER_PIXEL 3
#pragma pack ( push , 1 )


typedef struct header
{
   unsigned short int   signature;      // type of file 4D42 hex for bmp
   unsigned int         bmp_file_size;
   unsigned short int   reserved1;      // reserved
   unsigned short int   reserved2;      // reserved
   unsigned int         offset;         // offset to start image data in bytes

}HEADER;

typedef struct info_header
{
   unsigned int         info_header_size;
   int                  image_width , image_height;     // in pixels
   unsigned short int   no_of_planes;                   // must be 1
   unsigned short int   bits_per_pixel;                 // can be 1 , 4 , 8 , 24
   unsigned int         compression_type;               //   0 -none , 1 - RLE-8 , 2 - RLE-4
   unsigned int         image_data_size;                // including padding
   int                  horizontal_resolution, vertical_resolution;
   unsigned int         no_of_colors;
   unsigned int         no_of_imp_colors;

}INFO_HEADER;

typedef struct metadata{  
   HEADER bmp_header;
   INFO_HEADER bmp_info_header;
   char fileName[100];
}METADATA;

METADATA metadata[1000];
int size = 0;

#pragma pack(pop)

int parent(int curr){
   return curr/2;
}

void swap(METADATA *heap, int curr, int parent){
   METADATA temp = heap[curr];
   heap[curr] = heap[parent];
   heap[parent] = temp;
}

bool isLeaf(int curr, int size){
   if(curr > (size/2) && curr <= size){
      return true;
   } else{
      return false;
   }
}

int rightChildPos(int curr){
   return curr*2+1;
}

int leftChildPos(int curr){
   return curr*2;
}

void minHeapify(METADATA *heap, int *size, int curr){
   if(isLeaf(curr, *size) == true)return;
   // printf("%d-", *size);

   unsigned int rightChildSize = heap[rightChildPos(curr)].bmp_header.bmp_file_size ;
   unsigned int leftChildSize = heap[leftChildPos(curr)].bmp_header.bmp_file_size;

   if(heap[curr].bmp_header.bmp_file_size > rightChildSize || heap[curr].bmp_header.bmp_file_size > leftChildSize){
      if(leftChildSize < rightChildSize){
         swap(heap, curr, leftChildPos(curr));
         minHeapify(heap, size, leftChildPos(curr));
      }else{
         swap(heap, curr, rightChildPos(curr));
         minHeapify(heap, size, rightChildPos(curr));
      }
   }
}

METADATA extractMin(METADATA *heap, int *size){
   // get max value or root
   METADATA maxValue = heap[1];
   heap[1] = heap[*size];
   *size = *size-1;

   minHeapify(heap, size, 1);
   return maxValue;
}

void print_header(METADATA metadata );
int insertHeap(int size, char *filename);
void readAllBMP();
void printAll();

int main ()
{
   int choose;

   while (true)
   {
      printf("\tWELCOME\n");
      printf("==================================\n");
      printf("1. Read All BMP file.\n");
      printf("2. Get Smallest BMP file data.\n");
      printf("3. Print All BMP data.\n");
      printf("0. Exit.\n");
      printf("Enter the number : ");
      scanf("%d", &choose);
      // getchar();

      switch (choose)
      {
      case 1:
         readAllBMP();
         break;
      case 2: ;
         METADATA tempExtrack;
         tempExtrack = extractMin(metadata, &size);
         print_header(tempExtrack);
         break;
      case 3:
         printAll();
         break;
      case 0:
         exit(1);
         break;
      default:
         break;
      }
   }
   

   readAllBMP();
   printAll();

   return 0;
}

void printAll(){
   for(int i = 1; i<=size; i++){
      print_header(metadata[i]);
   }
}

void readAllBMP(){
   DIR *d;
   struct dirent *dir;
   FILE *fptr;

   // Read All bmp file in a folder
   d = opendir("E:/Kuliah/Semester 2/Simple Image Processing/testing");
   if (d)
   {
      while ((dir = readdir(d)) != NULL)
      {
      
      fptr = fopen(dir->d_name, "r+");
      if (fptr != NULL){
         size = insertHeap(size, dir->d_name);
         fclose(fptr);
      }
   
      }
      closedir(d);
   }
}

/*
prints the header information
*/
void print_header(METADATA metadata )
{
   printf("\n\nfilename\t\t%s\n", metadata.fileName);
   printf("signature\t\t%02x\n", metadata.bmp_header.signature);
   printf("size of bmp\t\t%d\n", metadata.bmp_header.bmp_file_size);
   printf("offset\t\t\t%d\n",metadata.bmp_header.offset );

   printf("header size\t\t%d\n", metadata.bmp_info_header.info_header_size);
   printf("image_width\t\t%d\n",metadata.bmp_info_header.image_width );
   printf("image_height\t\t%d\n", metadata.bmp_info_header.image_height);
   printf("no_of_planes\t\t%d\n", metadata.bmp_info_header.no_of_planes);
   printf("bits_per_pixel\t\t%d\n", metadata.bmp_info_header.bits_per_pixel );
   printf("compression_type\t%d\n",metadata.bmp_info_header.compression_type );
   printf("size of image\t\t%d\n", metadata.bmp_info_header.image_data_size);
   printf("horizontal_resolution\t%d\n", metadata.bmp_info_header.horizontal_resolution );
   printf("vertical_resolution\t%d\n", metadata.bmp_info_header.vertical_resolution);
   printf("no_of_colors\t\t%d\n",metadata.bmp_info_header.no_of_colors );
   printf("no_of_imp_colors\t%d\n", metadata.bmp_info_header.no_of_imp_colors);

}

int insertHeap(int size, char *file_name){
   long long unsigned int file_size = 10;
   METADATA tempMetadata;
   unsigned char *data_buffer;
   strcpy(tempMetadata.fileName, file_name);

   FILE *in_file = NULL;

   in_file =  fopen ( file_name , "rb" );
   if (in_file == NULL)
   {
      printf("file opening failed\n");
      return size;
   }

   if ( fread ( &tempMetadata.bmp_header , sizeof ( HEADER ) , 1 , in_file) < 1 )
   {
      printf("Some issue in reading file\n");
      return size;
   }

   if ( fread ( &tempMetadata.bmp_info_header , sizeof ( INFO_HEADER ) , 1 , in_file) < 1 )
   {
      printf("Some issue in reading file\n");
      return size;
   }

   //checking if the image is bmp or not
   if ( tempMetadata.bmp_header.signature != BMP_SIGNATURE )
   {
      printf("%s no a bmp\n", file_name);
   }

   if ( fseek ( in_file , tempMetadata.bmp_header.offset , SEEK_SET ) == -1 )
   {
      printf("file operation failed\n");
      // return 1;
   }

   file_size = tempMetadata.bmp_info_header.image_data_size ;
   //based on file_size allocate memory
   data_buffer = malloc (sizeof (char) * file_size + 1 );
   if (data_buffer == NULL)
   {
     printf("malloc Failed\n");
     return size;
   }
   *(data_buffer  + file_size) = CHAR_NULL;
   //read the data into buffer
   if ( fread ( data_buffer , sizeof(char) , file_size , in_file ) < file_size )
   {
     printf("Some issue in reading file\n");
     fclose (in_file);
     free (data_buffer);
     return size;
   }
   // printf("!");

   size++;
   metadata[size] = tempMetadata;
   // swapping
   int curr = size;
   while(metadata[curr].bmp_header.bmp_file_size < metadata[parent(curr)].bmp_header.bmp_file_size){
      swap(metadata, curr, parent(curr));
      curr = parent(curr);
   }


   // size++;
   printf("%s read\n", file_name);
   return size;
}
