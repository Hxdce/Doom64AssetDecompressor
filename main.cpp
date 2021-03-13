// C++ language libraries:
#include <iostream>
#include <string>
// C language libraries (possibly needed):
#include <windows.h>
#include <stdio.h>

// This is needed so Visual Studio will shut up about using fopen():
#pragma warning(disable : 4996)

// This project is going to make usage of a lot of code from SvKaiser and Erick194's work. Full credits to them!
// March 2021, 
// https://github.com/svkaiser
// https://github.com/Erick194

#define OVERFLOWCHECK       0x7FFFFFFF
#define TABLESIZE   1280

typedef struct {
    int var0;
    int var1;
    int var2;
    int var3;
    byte* write;
    byte* writePos;
    byte* read;
    byte* readPos;
    int null1;
    int null2;
} decoder_t;

static decoder_t g_Decoder;

byte g_DecodeTable[TABLESIZE * 4];

byte g_Array01[0xFFFFFF];     // 0x800B3660
byte g_Array05[0xFFFFFF];     // 0x8005D8A0
byte g_TableVar01[0xFFFFFF];  // 0x800B2250
int g_AllocPtr[0xFFFFFF];


void DeflateInitDecodeTable() {
    // All this code below is SvKaiser's work. How it works is a mystery to me, honestly.

    int v[2];
    int a[4];
    byte* a0p;
    byte* v1p;
    byte* a1p;
    byte* a2p;
    byte* a3p;
    byte* v0p;

    // With *(signed short*)g_Array05 = some_value, you are writing sizeof(signed short) bytes into memory,
    // starting at the address the g_Array05 variable contains...

    *(signed short*)g_Array05 = 0x04;
    *(signed short*)(g_Array05 + 2) = 0x06;
    *(signed short*)(g_Array05 + 4) = 0x08;
    *(signed short*)(g_Array05 + 6) = 0x0A;
    *(signed short*)(g_Array05 + 8) = 0x0C;
    *(signed short*)(g_Array05 + 10) = 0x0E;

    *(signed short*)(g_TableVar01 + 0x34) = 0x558F;

    *(int*)(g_TableVar01 + 0x3C) = 3;
    *(int*)(g_TableVar01 + 0x40) = 0;
    *(int*)(g_TableVar01 + 0x44) = 0;

    g_Decoder.var0 = 0;
    g_Decoder.var1 = 0;
    g_Decoder.var2 = 0;
    g_Decoder.var3 = 0;

    a0p = (g_Array01 + 4);
    v1p = (byte*)(g_DecodeTable + 0x9E4);

    v[0] = 2;

    do {
        if (v[0] < 0) {
            *(signed short*)v1p = (signed short)((v[0] + 1) << 1);
        }
        else {
            *(signed short*)v1p = (signed short)(v[0] >> 1);
        }

        *(signed short*)a0p = 1;

        v1p += 2;
        a0p += 2;

    }     while (++v[0] < 1258);

    a1p = (byte*)(g_DecodeTable + 0x4F2);
    a0p = (byte*)(g_DecodeTable + 2);

    v[1] = 2;
    a[2] = 3;

    do {
        *(signed short*)a1p = a[2];
        a[2] += 2;

        *(signed short*)a0p = v[1];
        v[1] += 2;

        a0p += 2;
        a1p += 2;

    }     while (a[2] < 1259);

    *(int*)g_TableVar01 = 0;
    v[1] = (1 << *(signed short*)(g_Array05));
    *(int*)(g_TableVar01 + 0x18) = (v[1] - 1);

    *(int*)(g_TableVar01 + 4) = v[1];
    v[1] += (1 << *(signed short*)(g_Array05 + 2));
    *(int*)(g_TableVar01 + 0x1C) = (v[1] - 1);

    v[0] = 2;
    a2p = (g_Array05 + (v[0] << 1));

    a[0] = (v[0] << 2);
    a1p = (g_TableVar01 + a[0]);

    *(signed short*)a1p = v[1];

    v[1] += (1 << *(signed short*)a2p);
    *(int*)(a1p + 4) = v[1];

    v[1] += (1 << *(signed short*)(a2p + 2));
    *(int*)(a1p + 8) = v[1];

    a3p = ((byte*)(g_TableVar01 + 0x18) + a[0]);

    *(int*)a3p = (v[1] - 1);

    v[1] += (1 << *(signed short*)(a2p + 4));
    *(int*)(a1p + 12) = v[1];

    v[1] += (1 << *(signed short*)(a2p + 6));
    *(int*)(a3p + 4) = (v[1] - 1);
    *(int*)(a3p + 8) = (v[1] - 1);
    *(int*)(a3p + 0xc) = (v[1] - 1);

    v0p = (byte*)(g_TableVar01 + 0x30);

    *(int*)v0p = (v[1] - 1);
    *(int*)(v0p + 4) = ((v[1] - 1) + 64);
}


void DeflateDecompress(FILE* inputFile, FILE* outputFile) {
    // It appears that this decompression algorithm works by using the decoder (g_Decoder) to read the contents of the input file,
    // byte-by-byte, duplicating(?) and modifying the bytes, then writing the modified bytes to the output file.
    int v[2];
    int a[4];
    int s[10];
    int t[10];
    int at;
    int div;
    int mul;
    int incrBit;

    byte* tablePtr1;
    byte* a1p;
    byte* s4p;
    byte* t8p;
    byte* t9p;
    byte* t1p;
    byte* v0p;
    byte* t2p;
    byte* t4p;

}


int main(int argc, char* argv[]) {

	int numberOfFiles = argc - 1;
	std::string outLocation = "";    

	if (argc > 1) {
		std::string s = argv[1];
		std::cout << "First file is " << s << '\n';
		std::cout << "Number of files to process: " << numberOfFiles << '\n';
        std::cout << "Processing...\n";

        for (int i = 1; i < argc; i++) {
            FILE* inputFile = fopen(argv[i], "r");

            std::string outputFileName = "Test" + std::to_string(i) + ".bin";
            FILE* outputFile = fopen(outputFileName.c_str(), "w");
            
            //DeflateDecompress(inputFile, outputFile);
        }
	}
	else {
        std::cout << "No files selected\n";
	}

	//
	//
	system("pause");
	return 0;
}