// C++ language libraries:
#include <iostream>
#include <string>
#include <fstream>
// C language libraries (possibly needed):
#include <windows.h>
#include <stdio.h>
#include <dirent.h> 

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

FILE* g_InputFile;
FILE* g_OutputFile;

//**************************************************************
//**************************************************************
//      Deflate_InitDecodeTable
//**************************************************************
//**************************************************************

void Deflate_InitDecodeTable() {
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

//**************************************************************
//**************************************************************
//      Deflate_GetDecodeByte
//**************************************************************
//**************************************************************

byte Deflate_GetDecodeByte(void) {
	if (!((g_Decoder.readPos - g_Decoder.read) < OVERFLOWCHECK))
		return -1;

	int readIntStart = fgetc(g_InputFile);
	if (readIntStart != EOF) {
		byte readByteStart = (byte)readIntStart;
		g_Decoder.readPos = &readByteStart;
		//std::cout << "Getting a byte for decoding!\n";
		return *g_Decoder.readPos;
	}
	return -1;
}

//**************************************************************
//**************************************************************
//      Deflate_DecodeScan
//**************************************************************
//**************************************************************

int Deflate_DecodeScan(void) {
	int resultbyte;

	resultbyte = g_Decoder.var0;

	g_Decoder.var0 = (resultbyte - 1);
	if ((resultbyte < 1)) {
		resultbyte = Deflate_GetDecodeByte();

		g_Decoder.var1 = resultbyte;
		g_Decoder.var0 = 7;
	}

	resultbyte = (0 < (g_Decoder.var1 & 0x80));
	g_Decoder.var1 = (g_Decoder.var1 << 1);

	return resultbyte;
}

//**************************************************************
//**************************************************************
//      Deflate_CheckTable
//**************************************************************
//**************************************************************

void Deflate_CheckTable(int a0, int a1, int a2) {
	int i = 0;
	byte* t7p;
	byte* v0p;
	int idByte1;
	int idByte2;
	byte* tablePtr = (byte*)(g_DecodeTable + 0x9E0);

	idByte1 = (a0 << 1);

	do {
		idByte2 = *(signed short*)(tablePtr + idByte1);

		t7p = (g_Array01 + (idByte2 << 1));
		*(signed short*)t7p =
			(*(signed short*)(g_Array01 + (a1 << 1)) +
				*(signed short*)(g_Array01 + idByte1));

		a0 = idByte2;

		if (idByte2 != 1) {
			idByte1 = *(signed short*)(tablePtr + (idByte2 << 1));
			idByte2 =
				*(signed short*)(g_DecodeTable + (idByte1 << 1));

			a1 = idByte2;

			if (a0 == idByte2)
				a1 = *(signed short*)((g_DecodeTable + 0x4F0) +
					(idByte1 << 1));
		}

		idByte1 = (a0 << 1);

	} while (a0 != 1);

	if (*(signed short*)(g_Array01 + 2) != 0x7D0)
		return;

	*(signed short*)(g_Array01 + 2) >>= 1;

	v0p = (byte*)(g_Array01 + 4);

	do {
		*(signed short*)(v0p + 6) >>= 1;
		*(signed short*)(v0p + 4) >>= 1;
		*(signed short*)(v0p + 2) >>= 1;
		*(signed short*)(v0p) >>= 1;

		v0p += 8;
		i += 8;

	} while (i != 2512);
}

//**************************************************************
//**************************************************************
//      Deflate_DecodeByte
//**************************************************************
//**************************************************************

void Deflate_DecodeByte(int a0) {
	int v[2];
	int a[4];
	int s[10];
	byte* s4p;
	byte* s2p;
	byte* v1p;
	byte* s1p;
	byte* s6p;
	byte* s3p;
	byte* a1p;

	s4p = g_Array01;
	v[0] = (a0 << 1);

	s2p = (byte*)(g_DecodeTable + 0x9E0);

	v1p = (s4p + v[0]);
	s[5] = 1;

	a[2] = (a0 + 0x275);
	*(signed short*)(v1p + 0x4EA) = (*(signed short*)(v1p + 0x4EA) + 1);

	if (s[5] == *(signed short*)((s2p + v[0]) + 0x4EA))
		return;

	v[1] = (a[2] << 1);

	s1p = (s2p + v[1]);

	s6p = (byte*)g_DecodeTable;

	a[3] = (*(signed short*)s1p << 1);
	a[1] = *(signed short*)(s6p + a[3]);
	s3p = (byte*)(g_DecodeTable + 0x4F0);

	if (a[2] == a[1]) {
		a[1] = *(signed short*)(s3p + a[3]);
		a[0] = a[2];
		Deflate_CheckTable(a[0], a[1], a[2]);
		a[3] = (*(signed short*)s1p << 1);
	}
	else {
		a[0] = a[2];
		Deflate_CheckTable(a[0], a[1], a[2]);
		s3p = (byte*)(g_DecodeTable + 0x4F0);
		a[3] = (*(signed short*)s1p << 1);
	}

	do {
		a[0] = (*(signed short*)(s2p + a[3]) << 1);

		a1p = (s6p + a[0]);
		v[0] = *(signed short*)a1p;
		s[0] = v[0];

		if (*(signed short*)s1p == v[0]) {
			s[0] = *(signed short*)(s3p + a[0]);
		}

		v1p = (s6p + a[3]);

		if (*(signed short*)(s4p + (s[0] << 1)) <
			*(signed short*)(s4p + v[1])) {
			if (*(signed short*)s1p == v[0]) {
				*(signed short*)(s3p + a[0]) = a[2];
			}
			else
				*(signed short*)a1p = a[2];

			a[1] = *(signed short*)v1p;
			if (a[2] == a[1]) {
				a[2] = *(signed short*)(s3p + a[3]);
				*(signed short*)v1p = s[0];
			}
			else {
				*(signed short*)(s3p + a[3]) = s[0];
				a[2] = a[1];
			}

			*(signed short*)(s2p + (s[0] << 1)) =
				*(signed short*)s1p;
			*(signed short*)s1p = *(signed short*)(s2p + a[3]);
			a[0] = s[0];
			a[1] = a[2];

			Deflate_CheckTable(a[0], a[1], a[2]);
			s1p = (s2p + (s[0] << 1));
		}

		a[2] = *(signed short*)s1p;
		v[1] = (a[2] << 1);

		s1p = (s2p + v[1]);
		a[3] = (*(signed short*)s1p << 1);

	} while (*(signed short*)s1p != s[5]);
}

//**************************************************************
//**************************************************************
//      Deflate_StartDecodeByte
//**************************************************************
//**************************************************************

int Deflate_StartDecodeByte(void) {
	int lookup = 1;		// $s0
	byte* tablePtr1 = g_DecodeTable;	// $s2
	byte* tablePtr2 = (byte*)(g_DecodeTable + 0x4F0);	// $s1

	while (lookup < 0x275) {
		if (Deflate_DecodeScan() == 0)
			lookup = *(signed short*)(tablePtr1 + (lookup << 1));
		else
			lookup = *(signed short*)(tablePtr2 + (lookup << 1));
	}

	lookup = (lookup + (signed short)0xFD8B);
	Deflate_DecodeByte(lookup);

	return lookup;
}

//**************************************************************
//**************************************************************
//      Deflate_RescanByte
//**************************************************************
//**************************************************************

int Deflate_RescanByte(int byte) {
	int i = 0;		// $s1
	int shift = 1;		// $s0
	int resultbyte = 0;	// $s2

	if (byte <= 0)
		return resultbyte;

	do {
		if (!(Deflate_DecodeScan() == 0))
			resultbyte |= shift;

		i++;
		shift = (shift << 1);
	} while (i != byte);

	return resultbyte;
}

//**************************************************************
//**************************************************************
//      Deflate_WriteOutput
//**************************************************************
//**************************************************************

void Deflate_WriteOutput(byte outByte) {
	putc(outByte, g_OutputFile);
	/*if (!((g_Decoder.writePos - g_Decoder.write) < OVERFLOWCHECK)) {
		std::cout << "Overflowed output buffer\n";
		return;
	}

	*g_Decoder.writePos++ = outByte;*/
}

//**************************************************************
//**************************************************************
//      Deflate_Decompress
//**************************************************************
//**************************************************************

void Deflate_Decompress() {
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

    Deflate_InitDecodeTable();

    incrBit = 0;

	byte readByteStart = (byte)fgetc(g_InputFile);
	if (readByteStart != EOF) {
		g_Decoder.read = &readByteStart;
		g_Decoder.readPos = &readByteStart;
	}
	else {
		std::cout << "How did this happen?\n";
		return;
	}

    g_Decoder.null1 = 0x7FFFFFFF;

    //byte writeByteStart = (byte)fgetc(g_OutputFile);
    //g_Decoder.write = &writeByteStart;
    //g_Decoder.writePos = &writeByteStart;

    tablePtr1 = (byte*)(g_TableVar01 + 0x34);

    g_Decoder.null2 = 0x7FFFFFFF;

    //	a1p = tablePtr1;
    a[2] = 1;
    a[3] = 0;
    // Z_Alloc(a[0], a1p, a[2], a[3]);

    s4p = (byte*)g_AllocPtr;

    v[0] = Deflate_StartDecodeByte();

    at = 256;
    s[0] = v[0];

	// GhostlyDeath <May 14, 2010> -- loc_8002E058 is part of a while loop
	while (v[0] != at) {
		at = (v[0] < 256);
		v[0] = 62;

		// GhostlyDeath <May 15, 2010> -- loc_8002E094 is an if statement
		if (at != 0) {
			a[0] = (s[0] & 0xff);
			Deflate_WriteOutput((byte)a[0]);

			t8p = s4p;
			t9p = (t8p + incrBit);
			*t9p = s[0];

			t[1] = *(int*)tablePtr1;

			incrBit += 1;
			if (incrBit == t[1])
				incrBit = 0;
		}
		// GhostlyDeath <May 15, 2010> -- Since then old shots point to loc_8002E19C the remainder of
		// loc_8002E094 until loc_8002E19C is an else.
		else {
			t[2] = (s[0] + (signed short)0xFEFF);
			div = t[2] / v[0];	// (62)

			// GhostlyDeath <May 15, 2010> -- loc_8002E0AC is an adjacent jump (wastes cpu cycles for fun!)
			at = -1;

			// GhostlyDeath <May 15, 2010> -- loc_8002E0C4 is an if
			if (v[0] == at)
				at = 0x8000;

			s[2] = 0;
			s[5] = div;
			t[4] = (s[5] << 1);

			mul = s[5] * v[0];

			a[0] = *(signed short*)(g_Array05 + t[4]);

			t[3] = mul;

			s[8] = (s[0] - t[3]);	// subu    $fp, $s0, $t3
			s[8] += (signed short)0xFF02;	// addiu   $fp, 0xFF02
			s[3] = s[8];	// move    $s3, $fp

			v[0] = Deflate_RescanByte(a[0]);

			t[5] = (s[5] << 2);
			t[6] = *(int*)(g_TableVar01 + t[5]);
			s[1] = incrBit;

			t[7] = (t[6] + v[0]);

			v[1] = (t[7] + s[8]);	// addu $v1, $t7, $fp
			a[0] = (incrBit - v[1]);	// subu input, incrBit, $v1
			s[0] = a[0];	// move $s0, input

			// GhostlyDeath <May 15, 2010> -- loc_8002E124 is an if
			if (a[0] < 0)	// bgez input, loc_8002E124
			{
				t[8] = *(int*)tablePtr1;
				s[0] = (a[0] + t[8]);
			}
			// GhostlyDeath <May 15, 2010> -- loc_8002E184 is an if
			if (s[8] > 0)
				// GhostlyDeath <May 15, 2010> -- loc_8002E12C is a while loop (jump back from end)
				while (s[2] != s[3]) {
					t9p = s4p;
					t1p = (t9p + s[0]);
					a[0] = *(byte*)t1p;	// lbu  input, 0($t1)
					Deflate_WriteOutput((byte)a[0]);

					v0p = s4p;
					s[2] += 1;

					t2p = (v0p + s[0]);	// addu $t2, $s0, $v0
					t[3] = *(byte*)t2p;

					t4p = (v0p + s[1]);
					*(byte*)t4p = t[3];

					v[1] = *(int*)tablePtr1;

					s[1]++;
					s[0]++;

					// GhostlyDeath <May 15, 2010> -- loc_8002E170 is an if
					if (s[1] == v[1])
						s[1] = 0;

					// GhostlyDeath <May 15, 2010> -- loc_8002E17C is an if 
					if (s[0] == v[1])
						s[0] = 0;
				}

			v[1] = *(int*)tablePtr1;
			incrBit += s[8];
			at = (incrBit < v[1]);

			// GhostlyDeath <May 15, 2010> -- loc_8002E19C is the end of a while
			if (at == 0)
				incrBit -= v[1];
		}

		v[0] = Deflate_StartDecodeByte();

		at = 256;
		s[0] = v[0];
	}

	a[1] = *(int*)s4p;
	// Z_Free();
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
            g_InputFile = fopen(argv[i], "r");

            /*
            int byteToBeRead;
            //fread(&byteToBeRead, sizeof(byteToBeRead), 1, inputFile);
            for (int i = 0; i < 100; i++) {
                byteToBeRead = fgetc(g_InputFile);
                std::cout << byteToBeRead << "\n";
            }
            */

            std::string outputFileName = "Test" + std::to_string(i) + ".bin";
            g_OutputFile = fopen(outputFileName.c_str(), "w");

			if (g_InputFile != nullptr) {
				Deflate_Decompress();
			}
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