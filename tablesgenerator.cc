/*
FEC - Forward error correction
Written in 2017 by <Ahmet Inan> <xdsopl@gmail.com>
To the extent possible under law, the author(s) have dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#include <iostream>
#include <cassert>

#define GENERATOR(M, POLY, TYPE) TablesGenerator<M, POLY, TYPE>::generate(#TYPE);

template <int M, int POLY, typename TYPE>
struct TablesGenerator
{
	static const int Q = 1 << M, N = Q - 1;
	static_assert(M <= 8 * sizeof(TYPE), "TYPE not wide enough");
	static_assert(Q == (POLY & ~N), "POLY not of degree Q");
	static void generate(const char *type)
	{
		TYPE log[Q], exp[Q];
		log[exp[N] = 0] = N;
		TYPE a = 1;
		for (int i = 0; i < N; ++i, a = next(a))
			log[exp[i] = a] = i;
		assert(1 == a);
		TYPE Artin_Schreier_imap[N];
		for (int i = 0; i < N; ++i)
			Artin_Schreier_imap[i] = 0;
		for (int x = 2; x < N; x += 2) {
			int xxx = exp[(2*log[x])%N]^x;
			assert(xxx);
			assert(!Artin_Schreier_imap[xxx]);
			Artin_Schreier_imap[xxx] = x;
		}
		std::cout << "template <>" << std::endl;
		std::cout << "struct Tables<" << M << ", " << POLY << ", " << type << ">\n{\n";
		print(log, "log", type, Q);
		print(exp, "exp", type, Q);
		print(Artin_Schreier_imap, "Artin_Schreier_imap", type, N);
		std::cout << "};\n" << std::endl;
	}
	static TYPE next(TYPE a)
	{
		return a & (TYPE)(Q >> 1) ? (a << 1) ^ (TYPE)POLY : a << 1;
	}
	static void print(TYPE *table, const char *name, const char *type, int size)
	{
		std::cout << "\tstatic " << type << " " << name << "(" << type << " a)\n\t{\n\t\t";
		std::cout << "static const " << type << " table[" << size << "] = {";
		for (int i = 0; i < size; ++i) {
			std::cout << " " << (int)table[i];
			if (i < size - 1)
				std::cout << ",";
		}
		std::cout << " };" << std::endl;
		std::cout << "\t\treturn table[a];\n\t}\n";
	}
};

int main()
{
	std::cout << "/* generated by generator.cc */" << std::endl << std::endl;
	std::cout << "template <int M, int POLY, typename TYPE>" << std::endl;
	std::cout << "struct Tables {};" << std::endl << std::endl;

	// BBC WHP031 RS(15, 11) T=2
	GENERATOR(4, 0b10011, uint8_t);

	// DVB-T RS(255, 239) T=8
	GENERATOR(8, 0b100011101, uint8_t);

	// FUN RS(65535, 65471) T=32
	GENERATOR(16, 0b10001000000001011, uint16_t);
}

