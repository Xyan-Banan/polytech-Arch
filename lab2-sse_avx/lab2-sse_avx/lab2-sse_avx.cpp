/*
* 1. Напишите программу «Изучение команд ММХ-SSE-AVX»,
* аналогично примеру, но с новыми командами (необходимо
* использовать минимум две “особенные” команды и команду к AVX-регистрам).
* “Особенными” командами можно считать команды
	* с насыщением,
	* сравнения,
	* перестановок,
	* упаковки/ распаковки,
	* SSE3 и др.
2. Введите в программу три функции с форматами данных
__m64, __m128, __m256 из библиотек Cи (#include <xmmintrin.h>,
<mmintrin.h>, <immintrin.h>), которые выполняют операции над
описанными ранее массивами типа char и float. Покажите в
Disassembler-е на регистрах выполнение команд.
Пример функции:__m64 _mm_add_pi8 (__m64 m1 , __m64 m2);
Функции у студентов должны отличаться.

3. Напишите программу работы с массивами с использованием их
векторного описания и функций библиотеки dvec. С применением
Disassembler-а определите количество машинных команд,
затраченных на реализацию программы.
*
*/
#include <stdio.h>
#include <iostream>
#include <conio.h>
#include <immintrin.h>
#include <xmmintrin.h>
#include <mmintrin.h>
#include <dvec.h>

using namespace std;

void printCharArrayFormatted(char* array, int size, const char* d1 = "", const char* d2 = "") {
	for (int i = 0; i < size; i++) {
		if (*d1 && *d2) {
			cout << ((int)array[i] < 0 ? d1 : d2) << "\t";
		}
		else {
			cout << (int)array[i] << "\t";
		}
	}
	cout << endl;
}

void printFloatArray(float* array, int size) {
	for (int i = 0; i < size; i++) {
		cout << array[i] << "\t";
	}
	cout << endl;
}

void printShortArray(short* array, int size) {
	for (int i = 0; i < size; i++) {
		cout << array[i] << "\t";
	}
	cout << endl;
}

int main()
{
	cout << "Hello World!\n";

	int dw1[] = { 33000, 15 };
	int dw2[] = { -10, -33000 };
	short dw3[4];

	_asm {
		movq mm0, dw1
		movq mm1, dw2
		packssdw mm0, mm1
		movq dw3, mm0
		emms
	}

	cout << "Packing:\n";
	printf("%i \t%i \t%i \t%i\n", dw1[0], dw1[1], dw2[0], dw2[1]);
	for (int i = 0; i < 4; i++) {
		cout << dw3[i] << " \t";
	}
	cout << endl << endl;

	char qw1[8] = { 1, 2, 1, -10, 0,  0, 3, 1 };
	char qw2[8] = { 2, 1, 1,   0, 0, 10, 2, 2 };
	char qwgt[8];
	char qweq[8];

	_asm {
		movq mm0, qw2
		movq mm1, qw1
		movq mm2, qw1
		pcmpgtb mm1, mm0
		pcmpeqb mm2, mm0
		movq qwgt, mm1
		movq qweq, mm2
		emms
	}

	cout << "Comparison:\n";
	printCharArrayFormatted(qw1, 8);
	printCharArrayFormatted(qw2, 8);
	printCharArrayFormatted(qwgt, 8, ">", "<=");
	printCharArrayFormatted(qweq, 8, "==", "!=");
	cout << endl;

	float avx1[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	float avx2[8] = { 1, 1, 2, 3, 4, 5, 6, 7 };
	float avx[8];

	_asm { // обнуление xmm и ymm (avx) регистров (256 avx)
		//vzeroall
		vmovups ymm0, avx1
		vmovups ymm1, avx2
		vmulps ymm0, ymm0, ymm0
		vmulps ymm1, ymm1, ymm1
		vaddps ymm2, ymm1, ymm0
		vsqrtps ymm2, ymm2
		vmovups avx, ymm2
	}

	cout << "Hypos (AVX):\n";
	printFloatArray(avx1, 8);
	printFloatArray(avx2, 8);
	printFloatArray(avx, 8);
	cout << endl;

	short int simmin1[] = { -1,2,3,-4 };
	short int simmin2[] = { -3,1,-4,7 };
	__m64 simmout = _mm_mullo_pi16(*(__m64*)simmin1, *(__m64*)simmin2);
	_asm { emms }

	cout << "Multiplication:\n";
	printShortArray(simmin1, 4);
	printShortArray(simmin2, 4);
	printShortArray(simmout.m64_i16, 4);
	cout << endl;

	float fmmin1[8] = { 6, 6, 6, 6, 6, 6, 6, 6 };
	float fmmin2[8] = { 2, 1, 6, 4, 5, 3, 7, 8 };
	__m128 fmmout = _mm_cmpeq_ps(*(__m128*)fmmin1, *(__m128*)fmmin2);
	_asm { emms }

	cout << "Comparasing (equal?):\n";
	printFloatArray(fmmin1, 4);
	printFloatArray(fmmin2, 4);
	for (int i = 0; i < 4; i++) {
		cout << ((fmmout.m128_i32[i] < 0) ? "==" : "!=") << "\t";
	}
	cout << endl << endl;

	__m256 fmmout2 = _mm256_div_ps(*(__m256*)fmmin1, *(__m256*)fmmin2);

	cout << "Division:\n";
	printFloatArray(fmmin1, 8);
	printFloatArray(fmmin2, 8);
	printFloatArray(fmmout2.m256_f32, 8);
	cout << endl;

	//dvec 24 строки на ассемблере для выполнения функции
	Is16vec8 dvec1(1, 2, 3, 4, 5, 6, 7, 8);
	Is16vec8 dvec2(1, 2, 3, 4, 5, 6, 7, 8);

	cout << "Multiplication (dvec):\n";
	printShortArray((short*)&dvec1, 8);
	printShortArray((short*)&dvec2, 8);

	dvec1 *= dvec2;
	printShortArray((short*)&dvec1, 8);
	cout << endl;

	return 0;
}
