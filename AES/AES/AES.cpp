#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

ifstream in("input.txt");
ofstream out("output.txt");

void SState(string State[4][4], string msg)
{
   for (int r = 0; r < 4; r++)
      for(int c = 0; c < 4; c++) 
        State[r][c] = msg[r + (4 * c)];
}
void InvSState(string State[4][4], string msg)
{
   for (int r = 0; r < 4; r++)
      for(int c = 0; c < 4; c++)
	  {
        State[r][c] += msg[2*r + (8 * c)];
		State[r][c] += msg[2*r + (8 * c) + 1];
	  }
}

//dec string -> hex string
string char_to_hex(string num)
{
    ostringstream stream;
	for(int i = 0; i < num.length(); ++i)
		stream << hex << int(num[i]);
    return stream.str();
}

//dec int -> hex string
string int_to_hex(int num)
{
    ostringstream stream;
	stream << hex << int(num);
    return stream.str();
}

void Arr_to_hex(string State[4][4])
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			State[i][j] = char_to_hex(State[i][j]);
}

//int -> string
string convert_int(int n)
{
	stringstream ss;
	ss << n;
	return ss.str();
}

void SubBytes(int Sbox[16][16], string State[4][4])
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			int m,n;
			if ((State[i][j])[0] >= '0' && (State[i][j])[0] <= '9')
				m = (int)(State[i][j])[0] - 48;
			else
				m = (State[i][j])[0] - 87;
			if ((State[i][j])[1] >= '0' && (State[i][j])[1] <= '9')
				n = (State[i][j])[1] - 48;
			else
				n = (State[i][j])[1] - 87;
			State[i][j] = int_to_hex(Sbox[m][n]);
			if(State[i][j].size() == 1)
				State[i][j] = "0" + State[i][j];
		}
}

void ShiftRow(string State[4][4], int i)
{
	string temp = State[i][0];
	for (int j = 0; j < 3; ++j)
		State[i][j] = State[i][j + 1];
	State[i][3] = temp;
}

void ShiftRows(string State[4][4])
{
	int k = 1;
	for (int i = 1; i < 4; ++i)
	{
		for(int j = 0; j < k; ++j)
		{
			ShiftRow(State, i);
		}
		k++;
	}
}

//Hex string -> bin string
string hex_to_bin (string sHex)
		{
			string sReturn = "";
			for (int i = 0; i < sHex.length (); ++i)
			{
				switch (sHex [i])
				{
					case '0': sReturn.append ("0000"); break;
					case '1': sReturn.append ("0001"); break;
					case '2': sReturn.append ("0010"); break;
					case '3': sReturn.append ("0011"); break;
					case '4': sReturn.append ("0100"); break;
					case '5': sReturn.append ("0101"); break;
					case '6': sReturn.append ("0110"); break;
					case '7': sReturn.append ("0111"); break;
					case '8': sReturn.append ("1000"); break;
					case '9': sReturn.append ("1001"); break;
					case 'a': sReturn.append ("1010"); break;
					case 'b': sReturn.append ("1011"); break;
					case 'c': sReturn.append ("1100"); break;
					case 'd': sReturn.append ("1101"); break;
					case 'e': sReturn.append ("1110"); break;
					case 'f': sReturn.append ("1111"); break;
				}
			}
			return sReturn;
		}

//Hex string XOR hex string
string sum(string a, string b)
{
	string bina = hex_to_bin(a);
	string binb = hex_to_bin(b);
	string sum = "";
	for(int i = 0; i < bina.size(); ++i)
	{
		if((bina[i] == '0' && binb[i] == '0') ||(bina[i] == '1' && binb[i] == '1'))
			sum += '0';
		else if ((bina[i] == '1' && binb[i] == '0') || (bina[i] == '0' && binb[i] == '1'))
			sum += '1';
	}
	unsigned long long c = stoull(sum, 0, 2);
	sum = int_to_hex(c);
	if(sum.size() == 1)
		sum = '0' + sum;
	return sum; 
}

string gmul2(string a)
{
	string st1 = hex_to_bin(a);
	if(st1[0] == '1')
	{
		for (int j = 0; j < 7; ++j)
			st1[j] = st1[j + 1];
		st1[7] = '0';
		unsigned long long temp1 = stoull(st1, 0, 2);
		st1 = int_to_hex(temp1);
		if (st1.size() == 1)
			st1 = '0' + st1;
		a = sum(st1, "1b");
	}
	else
	{
		for (int j = 0; j < 7; ++j)
			st1[j] = st1[j + 1];
		st1[7] = '0';
		unsigned long long temp1 = stoull(st1, 0, 2);
		st1 = int_to_hex(temp1);
		a = st1;
		if (a.size() == 1)
			a = '0' + a;
	}
	return a;
}

void MixColumns(string State[4][4])
{
	for(int c = 0; c < 4; ++c)
	{
		string a[4];
		string b[4];
		for(int i = 0; i < 4; ++i)
		{
			a[i] = State[i][c];
			b[i] = a[i];
			b[i] = gmul2(b[i]);
		}
		State[0][c] = sum(sum(sum(sum(b[0], a[1]), b[1]), a[2]), a[3]);
		State[1][c] = sum(sum(sum(sum(a[0], b[1]), a[2]), b[2]), a[3]);
		State[2][c] = sum(sum(sum(sum(a[0], a[1]), b[2]), a[3]), b[3]);
		State[3][c] = sum(sum(sum(sum(a[0], b[0]), a[1]), a[2]), b[3]);
	}
}

void AddRoundKey(string State[4][4], string KeySchedule[4][44], int k)
{
	for(int i = 0; i < 4; ++i)
		for(int j = k; j < k + 4; ++j)
		{
			State[i][j - k] = sum(State[i][j - k], KeySchedule[i][j]);
		}
}

void SubWord(string a[4], int Sbox[16][16])
{
	for (int j = 0; j < 4; ++j)
	{
		int m,n;
		if ((a[j])[0] >= '0' && (a[j])[0] <= '9')
			m = (int)(a[j])[0] - 48;
		else
			m = (a[j])[0] - 87;
		if ((a[j])[1] >= '0' && (a[j])[1] <= '9')
			n = (a[j])[1] - 48;
		else
			n = (a[j])[1] - 87;
		a[j] = int_to_hex(Sbox[m][n]);
		if(a[j].size() == 1)
			a[j] = "0" + a[j];
	}
}

void RotWord(string a[4])
{
	string temp = a[0];
	for(int i = 0; i < 3; ++i)
		a[i] = a[i + 1];
	a[3] = temp;
}

void KeyExpansion(string KeySchedule[4][44], string Key[4][4], string Rcon[4][10], int Sbox[16][16])
{
	for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
			KeySchedule[i][j] = Key[i][j];
	string temp[4];
	for (int i = 4; i < 44; ++i)
	{
		for(int j = 0; j < 4; ++j)
			temp[j] = KeySchedule[j][i - 1];
		if (i % 4 == 0)
		{
			RotWord(temp);
			SubWord(temp, Sbox);
			for(int j = 0; j < 4; ++j)
				temp[j] = sum(temp[j], Rcon[j][i/4 - 1]);
		}
		for(int j = 0; j < 4; ++j)
			KeySchedule[j][i] = sum(temp[j], KeySchedule[j][i - 4]);
	}
}

void InvSubBytes(int InvSbox[16][16], string State[4][4])
{
		for (int i = 0; i < 4; ++i)
		for (int j = 0; j < 4; ++j)
		{
			int m,n;
			if ((State[i][j])[0] >= '0' && (State[i][j])[0] <= '9')
				m = (int)(State[i][j])[0] - 48;
			else
				m = (State[i][j])[0] - 87;
			if ((State[i][j])[1] >= '0' && (State[i][j])[1] <= '9')
				n = (State[i][j])[1] - 48;
			else
				n = (State[i][j])[1] - 87;
			State[i][j] = int_to_hex(InvSbox[m][n]);
			if(State[i][j].size() == 1)
				State[i][j] = "0" + State[i][j];
		}
}

void InvShiftRow(string State[4][4], int i)
{
	string temp = State[i][3];
	for (int j = 3; j > 0; --j)
		State[i][j] = State[i][j - 1];
	State[i][0] = temp;
}

void InvShiftRows(string State[4][4])
{
	int k = 1;
	for (int i = 1; i < 4; ++i)
	{
		for(int j = 0; j < k; ++j)
		{
			InvShiftRow(State, i);
		}
		k++;
	}
}

void InvMixColumns(string State[4][4])
{
	for(int c = 0; c < 4; ++c)
	{
		string a[4];
		for(int i = 0; i < 4; ++i)
			a[i] = State[i][c];
		State[0][c] = sum(sum(sum(sum(sum(gmul2(gmul2(gmul2(a[0]))), gmul2(gmul2(a[0]))), gmul2(a[0])),
			sum(sum(gmul2(gmul2(gmul2(a[1]))), gmul2(a[1])), a[1])), 
			sum(sum(gmul2(gmul2(gmul2(a[2]))), gmul2(gmul2(a[2]))), a[2])),
			sum(gmul2(gmul2(gmul2(a[3]))), a[3]));

		State[1][c] = sum(sum(sum(sum(sum(gmul2(gmul2(gmul2(a[1]))), gmul2(gmul2(a[1]))), gmul2(a[1])),
			sum(sum(gmul2(gmul2(gmul2(a[2]))), a[2]), gmul2(a[2]))), 
			sum(sum(gmul2(gmul2(gmul2(a[3]))), gmul2(gmul2(a[3]))), a[3])),
			sum(gmul2(gmul2(gmul2(a[0]))), a[0]));

		State[2][c] = sum(sum(sum(sum(sum(gmul2(gmul2(gmul2(a[2]))), gmul2(gmul2(a[2]))), gmul2(a[2])),
			sum(sum(gmul2(gmul2(gmul2(a[3]))), a[3]), gmul2(a[3]))), 
			sum(sum(gmul2(gmul2(gmul2(a[0]))), gmul2(gmul2(a[0]))), a[0])),
			sum(gmul2(gmul2(gmul2(a[1]))), a[1]));

		State[3][c] = sum(sum(sum(sum(sum(gmul2(gmul2(gmul2(a[3]))), gmul2(gmul2(a[3]))), gmul2(a[3])),
			sum(sum(gmul2(gmul2(gmul2(a[0]))), a[0]), gmul2(a[0]))), 
			sum(sum(gmul2(gmul2(gmul2(a[1]))), gmul2(gmul2(a[1]))), a[1])),
			sum(gmul2(gmul2(gmul2(a[2]))), a[2]));
	}
}
//a[0]*9 ==  sum(gmul2(gmul2(gmul2(a[0]))), a[0])
//a[0]*b == sum(sum(gmul2(gmul2(gmul2(a[0]))), a[0]), gmul2(a[0]))
//a[0]*d == sum(sum(gmul2(gmul2(gmul2(a[0]))), gmul2(a[0])), gmul2(a[0]))
//a[0]*e == sum(sum(gmul2(gmul2(gmul2(a[0]))), gmul2(gmul2(a[0]))), gmul2(a[0]))

void Padding(string msg, int count)
{
	while(msg.size() % 16 != 0)
	{
		msg += '0';
		count++;
	}
}

string Encrypt(string KeySchedule[4][44], int Sbox[16][16], string msg, int count)
{
	string State[4][4];
	SState(State, msg);
	Arr_to_hex(State);
	Padding(msg, count);
	AddRoundKey(State, KeySchedule, 0);
	for (int i = 0; i < 9; ++i)
	{
		SubBytes(Sbox, State);
		ShiftRows(State);
		MixColumns(State);
		AddRoundKey(State, KeySchedule, (i + 1)*4);
	}
	SubBytes(Sbox, State);
	ShiftRows(State);
	AddRoundKey(State, KeySchedule, 40);
	string ans = "";
	for(int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ans = ans + State[j][i];
		}
	}
	return ans;
}

string Decrypt(string KeySchedule[4][44], int InvSbox[16][16], int count, string msg)
{
	string State[4][4];
	InvSState(State, msg);
	AddRoundKey(State, KeySchedule, 40);
	for(int i = 9; i > 0; --i)
	{
		InvShiftRows(State);
		InvSubBytes(InvSbox, State);
		AddRoundKey(State, KeySchedule,i*4);
		InvMixColumns(State);
	}
	InvShiftRows(State);
	InvSubBytes(InvSbox, State);
	AddRoundKey(State, KeySchedule, 0);
	string ans = "";
	for(int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			ans = ans + State[j][i];
		}
	}
	string res;
	for (int i = 0; i < ans.size(); i += 2)
	{
		istringstream iss(ans.substr(i, 2));
		int temp;
		iss >> hex >> temp;
		res += static_cast<char>(temp);
	}
	int size = res.size();
	string resres = res.substr(0, size - count);
	return resres;
}

int main()
{
	string msg;
	string KeySchedule[4][44];
	string Key[4][4] = 
	{
		"2b", "28", "ab", "09",
		"7e", "ae", "f7", "cf",
		"15", "d2", "15", "4f",
		"16", "a6", "88", "3c"
	};
	int Sbox[16][16] = 
	{
        0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 
        0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 
        0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 
        0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 
        0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 
        0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 
        0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8, 
        0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2, 
        0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73, 
        0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb, 
        0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79, 
        0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08, 
        0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 
		0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e, 
		0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf, 
		0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
	};
	int InvSbox[16][16] = 
	{
		0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
	};
	string Rcon[4][10] =
	{
		{"01", "02", "04", "08", "10", "20", "40", "80", "1b", "36"},
		{"00", "00", "00", "00", "00", "00", "00", "00", "00", "00"},
		{"00", "00", "00", "00", "00", "00", "00", "00", "00", "00"},
		{"00", "00", "00", "00", "00", "00", "00", "00", "00", "00"}
	};
	string State[4][4];
	KeyExpansion(KeySchedule, Key, Rcon, Sbox);
	getline(in, msg);
	cout << "Input: " << msg << endl;
	//int count = Padding(msg);
	string msg1 = "";
	string enc = "";
	if(count != 0)
	{
		for(int i = 0; i < msg.size(); i+=16)
		{
			for(int j = i; j < 16 + i; ++j)
			{
				msg1 += msg[j];
			}
			enc += Encrypt(KeySchedule, Sbox, msg1);
			msg1 = "";
		}
	}
	else
		enc = Encrypt(KeySchedule, Sbox, msg);
	string temp = "";
	string dec = "";
	if(count != 0)
	{
		for(int i = 0; i < enc.size(); i+=32)
		{
			for(int j = i; j < 32 + i; ++j)
			{
				temp += enc[j];
			}
			if(enc.size() - 32 == i)
				dec += Decrypt(KeySchedule, InvSbox, count, temp);
			else
				dec += Decrypt(KeySchedule, InvSbox, 0, temp);
			temp = "";
		}
	}
	else
		enc = Encrypt(KeySchedule, Sbox, msg);
	cout << "After Encrypt: " << enc << endl;
	cout << "After Decrypt: " << dec << endl;
	system("pause");
	return EXIT_SUCCESS;
}