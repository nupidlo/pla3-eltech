#include <stdio.h>
#include <string>
#include <stdlib.h>
#include <vector>
#include <cstdlib>
#include <io.h>
using namespace std;
typedef unsigned char uchar;
typedef vector<uchar> text;
enum act {ENCODE, DECODE};

void help() {
	printf("\n%s\n","crypt [options] <key> <source> [<dest>]");
	printf("%s\n","options:");
	printf("%s\n","-a, --alphabet=<alphabet>; by default contains the Latin alphabet's letters and digits from 0 to 9");
	printf("%s\n","-t, --type=<type>; can be encode (by default) or decode");
	printf("%s\n","-h, --help (you see it right now)");
	printf("%s\n","key - a key for encoding/decoding");
	printf("%s\n","source - a source file for encoding/decoding");
	printf("%s\n","dest - file that will contain the result of encoding/decoding (by default - source file)");
	return;
}

void incorInput() {
	printf("\n%s\n","Error! Incorrect data input");
	help();
	return;
}

void incorParams() {
	printf("\n%s\n","Error! Incorrect parameters");
	help();
	return;
}

//file existence
bool fileExists(const char *fname)
			{
			  return access(fname, 0) != -1;
			}

int main(int argc, char *argv[]) {
	
	//displays an error if the function doesn't contain any argument
	if(argc<2) {
		incorInput();
		return -1;
	}

	int alphaAdr=0; //number of input alphabet cell in argv[]
	bool alphaIn=false; //were the alphabet input?
	bool isAlphaInSeparate=false; //were the alphabet input in "-a <alphabet>" way?
	act action=ENCODE;
	int key=0; //key of encoding
	int inFileAdr=0; //number of input file cell in argv[]
	int outFileAdr=0; //number of output file cell in argv[]
	bool outFileIn=false; //were the output file input?

	for(int i=1;i<argc;i++) {
		if(argv[i][0]=='-') {
			switch(argv[i][1]) {
				case 'a': if(argv[i+1][0]=='-') { incorParams(); return -1; } else { alphaAdr=i+1; alphaIn=true; isAlphaInSeparate=true; i++; } break;
				case 't': if(argv[i+1][0]=='-') { incorParams(); return -1; } else switch(argv[i+1][0]) { 
																						case 'd': action=DECODE; i++; break; 
																						case 'e': i++; break; 
																						default: incorParams(); return -1; 
																					} break;
				case 'h': help(); return 0; break;
				case '-': switch(argv[i][2]) { 
								case 'a': alphaAdr=i; alphaIn=true; break; 
								case 't': switch(argv[i][7]) { 
												case 'd': action=DECODE; break; 
												case 'e': break; 
												default: incorParams(); return -1; 
											} break; 
								case 'h': help(); return 0; break; 
								default: incorParams(); return -1; 
							} break;
				default: incorParams(); return -1;
			}
		}
		else if(key==0) key=atoi(argv[i]); 
			 else if(inFileAdr==0) inFileAdr=i;
			 	  else if(outFileAdr==0) { outFileAdr=i; outFileIn=true; }
			 	  	   else { incorInput(); return -1; }
	}

	//displays an error if the key is negative
	if(key<0) {
		printf("\n%s\n","Error! Key must be positive");
		return -1;
	}

	//displays file existence
	if(!fileExists(argv[inFileAdr])) {
		printf("\n%s\n","Error! Input file not found");
		return -1;
	}

	//returns alphabet size
	int alphaInSize=0;
	int alphaSize=0;
	int i=0;
	if(alphaIn)
		if(isAlphaInSeparate) //for the alphabet input like "-a <alphabet>"
			while(argv[alphaAdr][i]) { 
				alphaInSize++;
				alphaSize++;
				if(argv[alphaAdr][i]=='"') alphaSize--; //don't count quotes
				i++;
			}
		else while(argv[alphaAdr][i+11]) { //for the alphabet input like "--alphabet=<alphabet>"
				alphaInSize++;
				alphaSize++;
				if(argv[alphaAdr][i+11]=='"') alphaSize--; //don't count quotes here, too
				i++;
			 }
	else alphaSize=62; //size of the default alphabet 

	while(key>alphaSize) key-=alphaSize; //makes key value lesser than alphabet size
	
	char alphaArr[alphaSize]; //creates an alphabet array
	
	//fills the alphabet array with symbols
	int j=0;
	if(alphaIn)
		if(isAlphaInSeparate) //for the alphabet input like "-a <alphabet>"
			for(i=0;i<alphaInSize;i++) {
				if(argv[alphaAdr][i]!='"') {
					alphaArr[j]=argv[alphaAdr][i];
					j++;
				}
			}		
		else for(i=0;i<alphaInSize;i++) { //for the alphabet input like "--alphabet=<alphabet>"
				if(argv[alphaAdr][i+11]!='"') {
					alphaArr[j]=argv[alphaAdr][i+11];
					j++;
				}
			 }
	else for(i=0;i<alphaSize;i++) { //for the default alphabet
			if(i<10) alphaArr[i]=(char)(i+48);
			else if(i<36) alphaArr[i]=(char)(i+55);
				 else alphaArr[i]=(char)(i+61);
		 } 

	//reads the input file and copies the text
	text *fileText=new text();
	FILE *in;
	int *buffer=new int();
	in=fopen(argv[inFileAdr],"r");
	while(*buffer!=EOF) {
		*buffer=fgetc(in);
		fileText->push_back(*buffer);
	}
	fclose(in);
	delete(buffer);

	//decoding is just a negative-key encoding
	int keyValue=key;
	if(action==DECODE) keyValue=-1*key;

	long inSize=fileText->size(); //the text symbols' quantity
	inSize--; //don't forget about \0 (that's not sieg heil!)

	//decoding/encoding itself
	for(i=0;i<inSize;i++) {
		for(j=0;j<alphaSize;j++) {
			if(fileText->at(i)==alphaArr[j]) { 
				fileText->at(i)=alphaArr[(j+keyValue+alphaSize)%alphaSize];
				j=alphaSize;
			}
		}
	}

	//output file output
	if(outFileIn) {
		FILE *out;
		out=fopen(argv[outFileAdr],"w");
		for(i=0;i<inSize;i++) {
			fputc(fileText->at(i),out);
		}
		fclose(out);
	}
	//input file output
	else {
		FILE *inRw;
		inRw=fopen(argv[inFileAdr],"w");
		for(i=0;i<inSize;i++) {
			fputc(fileText->at(i),inRw);
		}
		fclose(inRw);
	}

	printf("\n%s\n","Successfully!"); //gj
	return 0;
}
