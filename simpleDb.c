#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *name;
	char gender;
	int age;
}clientRecord;

void clientRecord_init_data(clientRecord *c, char *name, char gender, int age);
void clientRecord_destroy(clientRecord *c);

int clientRecord_read_text(char *fileName, clientRecord **array);
void clientRecord_write_text(char *fileName, clientRecord *array,int nRecords);

void clientRecord_write_binary(char *fileName,clientRecord *c,int nRecords);
int clientRecord_read_binary(char *fileName,clientRecord **c);

void clientRecord_sort(clientRecord *c,int nRecords);
int clientRecord_cmp(const void *a, const void *b);

int clientRecord_cmp(const void *a, const void *b){
	clientRecord *ca = (clientRecord*)a;
	clientRecord *cb = (clientRecord*)b;
	if(ca->age == cb->age)return strcmp(ca->name,cb->name);
	return ca->age -cb->age;
}	 

void clientRecord_sort(clientRecord *c,int nRecords){
	fprintf(stderr,"%d\n",nRecords);
	qsort(c,nRecords,sizeof(clientRecord),clientRecord_cmp);
}	

int clientRecord_read_text(char *fileName, clientRecord **array){
	FILE *fp=fopen(fileName,"r");
 clientRecord *myArray= (clientRecord *) malloc(sizeof(clientRecord));
 int maxElements=1;
 char line[512];
 int nLines=0;
	while(fgets(line,sizeof(line),fp)){
		while(nLines >= maxElements){
			maxElements *=2;
			myArray=realloc(myArray,maxElements*sizeof(clientRecord));
		}	
		int age;
		char gender;
		char name[512];
		sscanf(line,"%s %c %d",name,&gender,&age);
		fprintf(stderr,"%s %c %d\n",name,gender,age);
		clientRecord_init_data(myArray+nLines,name,gender,age);
		nLines++;
	}
	fclose(fp);
	*array=myArray;
	return nLines;
}
	
void clientRecord_write_text(char *fileName, clientRecord *array,int nRecords){
	FILE *fp=fopen(fileName,"w");
	for(int i=0;i<nRecords;i++){
	 fprintf(fp,"%s %c %d\n",array[i].name,array[i].gender,array[i].age);
	}
	fclose(fp);
}

void clientRecord_write_binary(char *fileName,clientRecord *c,int nRecords){
	FILE *fp=fopen(fileName,"w");
	fwrite(&nRecords,sizeof(int),1,fp);
 for(int i=0;i<nRecords;i++){
		unsigned char genage=0;
		char gender=(c[i].gender == 'M') ? 0 : 1;
		fwrite(c[i].name,strlen(c[i].name)+1,1,fp);
		genage=(gender << 7) | (unsigned char) c[i].age;
		fwrite(&genage,1,1,fp);
	}	
 fclose(fp);
}
int clientRecord_read_binary(char *fileName,clientRecord **c){
	FILE *fp=fopen(fileName,"r");
	if(!fp){
		 fprintf(stderr,"can't open %s\n",fileName); 
		exit(EXIT_FAILURE);
	}
	int nRecords=0;
	fread(&nRecords,sizeof(int),1,fp);
	clientRecord *myArray= (clientRecord *) malloc(sizeof(clientRecord)*nRecords);
 for(int i=0;i<nRecords;i++){
  char name[512];
  unsigned char genage=0;
  char input;
  char gender='M';
  int age;
  int k=0;
  while(fread(&input,1,1,fp)){
			name[k++]=input;
			if(!input){
				fread(&genage,1,1,fp);
				break;
			}	
	 }
  if(genage >> 7)gender='F';
  age=genage & 0x7F;
		clientRecord_init_data(myArray+i,name,gender,age);	
	}	
 *c=myArray;
 fclose(fp);
 return nRecords ;
}

void clientRecord_init_data(clientRecord *c, char *name, char gender, int age){
	c->name=malloc(strlen(name)+1);
	strcpy(c->name,name);
	c->age=age;
	c->gender=gender;
}
void clientRecord_destroy(clientRecord *c){
	if(c->name){ 
		free (c->name);
		c->name=0;
	}
	c->age=0;
	c->gender=0;
}
	
int main(int argc, char *argv[]){
	char *inputFile=0, *outputFile=0, rb=0, wb=0,s=0;
	int i=1;
	while(i<argc){
		if(strcmp(argv[i],"-i") == 0){
			inputFile=argv[++i];
		}	
		else if(strcmp(argv[i],"-o") == 0){
			outputFile=argv[++i];
		}
		else if(strcmp(argv[i],"-wb") == 0){
			wb=1;
		}				
		else if(strcmp(argv[i],"-rb") == 0){
			rb=1;
		}
		else if(strcmp(argv[i],"-s") == 0){
			s=1;
		}
		else{
			fprintf(stderr,"simpleDb given wrong parameters\n");
			exit(EXIT_FAILURE);
		}	
		i++;
	}
	//read in data
	clientRecord *c;
	int nRecords=0;
	if(rb==0) nRecords=clientRecord_read_text(inputFile,&c);	
	else nRecords=clientRecord_read_binary(inputFile,&c);	

 //do something
	if(s){
		clientRecord_sort(c,nRecords);
	}
	
	//write the data	
	if(wb==0) clientRecord_write_text(outputFile,c,nRecords);	
	else clientRecord_write_binary(outputFile,c,nRecords);
	
	//cleanup	
 for (int i=0;i<nRecords;i++)
  clientRecord_destroy(c+i);
 free(c); 

	return 1;
}

//Practice exercise

//**Assume that lines are never longer than 500 characters and that people are younger than 127

//********Part 1 declaring a struct clientRecord

//make a struct that contains a string for name, a character for gender (i.e. M or F), an integer for age
//call it clientRecord

//********Part 2 Bookkeeping stuff for clientRecord

//write a function clientRecord_init_read that takes a pointer to clientRecord, a name, a gender and an age as parameters
//it allocates memory for the name field in clientRecord and copies the name,gender and age into the appropriate fields of clientRecord

//write a function clientRecord_destroy that takes as a pointer to clientRecord and releases all the malloced memory - in this case just the name field

//*******Part 3 Read/Write text

//write a function to read in name, gender and age from the file sample.txt
//the function will take as parameters the filename and and a pointer to an array of clientRecords to be allocated and return an integer indicating the number of records read in
//use malloc to allocate enough memory for 1 arrayRecord to the array - keep track of the numberofElements allocated and the number of lines read in - and use realloc to allocate more memory if necessary
//read the data one line at a time - extract the information using sscanf - put the information into the array element using clientRecord_init_read

//Write a function that takes as parameters a filename, an array of clientRecords and the number of Elements in the array
//It will write the array to a text file like sample.txt

//******Part 4 Sort

//write a function that takes an array of clientRecords and the number of Elements
// It sorts by age as the first key - if age is the same it sorts alphabetically 
// USE qsort - this involves writing a cmp function as well

//******Part 5 Binary bit pushing and reading writing

//The age and gender can be encoded into one byte by using the first bit to indicate gender male=0 female=1 and the other 7 bits to indicate age (0-127)
//write a function that takes a filename, an array of clientRecords, and the number of elements in the array
//first the function writes an integer with the number of elements
//for each record it writes to the file the name(with \0) and a unsigned char genderage where the first bit indicates gender and the next 7 bits indicate age
//This can be done entirely with fwrite

//write a function that take as parameters the filename and and a pointer to an array of clientRecords to be allocated and return an integer indicating the number of records read in from a binary file in the above format

//use fread to read in the number of records and allocate an array of the correct size
//use fread to read in characters into a name string until '\0' is found
//then use fread to read the genderage byte
//decode it to get the gender and age
//use clientRecord_init_read to enter the information into an element of the array of clientRecords
//repeat until all records are read in

//******Part 6 put it all together

//put it all together with a main that parses the flags to support
//simpleDb -i <inputfile> -o <outputfile> -s -rb -wb

//where -s is a flag to indicate sorting
//where -rb is a flag that indicates that a binary file will be read in - otherwise a text file will be read in
//where -wb is a flag that indicates that a binary file will be written - otherwise a text file will be written out

//declare a pointer to clientRecord
//read in the file binary or text
//decide whether to sort it
//write out the file 
//clean up clientRecord

//Congratulations now you've written a simple extensible database
