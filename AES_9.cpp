#include<iostream>
#include<cstdio>
#include<ctime>
#include<cstring>
#include<vector>
#include<set>
#include<cstdlib>
#include"type.h"
#include"AES.h"
using namespace std;

struct pr {
	UINT32 x, y;
	pr(UINT32 a = 0, UINT32 b = 0) {
		x = a, y = b;
	}
	const bool operator < (pr a) const {
		if (x < a.x)
			return true;
		else if (x == a.x && y < a.y)
			return true;
		else
			return false;
	}
};
vector<pr> tab[256][256];
set<pr> X4[2][4];
vector<pr> X4_0[2][256][256], X4_1[2][256];
vector<UINT8> X2_0[256];

const UINT32 mask[4] = {
	0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
};
void SR(UINT32 res[4]) {
	UINT32 tmp[4];
	for (int i = 0; i < 4; i++) {
		tmp[i] = res[i];
	}

	res[0] = (tmp[0] & mask[0]) | (tmp[1] & mask[1]) | (tmp[2] & mask[2]) | (tmp[3] & mask[3]);
	res[1] = (tmp[1] & mask[0]) | (tmp[2] & mask[1]) | (tmp[3] & mask[2]) | (tmp[0] & mask[3]);
	res[2] = (tmp[2] & mask[0]) | (tmp[3] & mask[1]) | (tmp[0] & mask[2]) | (tmp[1] & mask[3]);
	res[3] = (tmp[3] & mask[0]) | (tmp[0] & mask[1]) | (tmp[1] & mask[2]) | (tmp[2] & mask[3]);
}

void _SR(UINT32 res[4]) {
	UINT32 tmp[4];
	for (int i = 0; i < 4; i++) {
		tmp[i] = res[i];
	}

	res[0] = (tmp[0] & mask[0]) | (tmp[3] & mask[1]) | (tmp[2] & mask[2]) | (tmp[1] & mask[3]);
	res[1] = (tmp[1] & mask[0]) | (tmp[0] & mask[1]) | (tmp[3] & mask[2]) | (tmp[2] & mask[3]);
	res[2] = (tmp[2] & mask[0]) | (tmp[1] & mask[1]) | (tmp[0] & mask[2]) | (tmp[3] & mask[3]);
	res[3] = (tmp[3] & mask[0]) | (tmp[2] & mask[1]) | (tmp[1] & mask[2]) | (tmp[0] & mask[3]);
}

UINT32 rc[] = {
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000
};
UINT32 getKg(UINT32 k, int rod) {
	k = (k << 8) | (k >> 24);
	UINT8 tmp;
	UINT32 kk = 0;
	for (int i = 0; i < 4; i++) {
		kk = SB(k);
	}
	kk ^= rc[rod];
	return kk;
}

void ddt() {
	UINT8 din, dout;
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			din = i ^ j;
			dout = SBOX[i] ^ SBOX[j];
			tab[din][dout].push_back(pr(i, j));
		}
	}
}

void getX2() {
	UINT8 A, B;
	for (int i = 0; i < 256; i++) {
		if (tab[i][1].size() > 0) {
			B = GF8[11][i];
			for (int j = 0; j < 256; j++) {
				if (GF8[13][j] == B) {
					B = j;
					break;
				}
			}
			for (int k = 0; k < tab[B][1].size(); k++) {
				for (int t = 0; t < tab[i][1].size(); t++) {
					X2_0[SBOX[tab[B][1][k].x]].push_back(SBOX[tab[i][1][t].x]);
				}
			}
		}
	}
}

UINT32 dX4[4] = {0x112a3902, 0x142f1437, 0x16352609, 0x37182318};
UINT32 dX5_lef[4] = {0x6a0000f2, 0x764c0000, 0x00f5c600, 0x00004a99};
UINT32 X5_mask[4] = { 0xee000090, 0x216d0000, 0x0071a400, 0x00006bce };

void getX4(UINT8 ca, UINT8 cb, int cap, int cbp, int idx) {
	UINT32 dX5, tmp1, dW4, tmp2, X51, X52;
	UINT8 a[4], b[4];
	vector<UINT32> X5[2];
	int k, sum = 0;
	for (int i = 0; i < 256; i++) {
		if (tab[i][ca].size() > 0) {
			for (int j = 0; j < 256; j++) {
				if (tab[j][cb].size() > 0) {
					dX5 = (i << cap) | (j << cbp) | dX5_lef[idx];
					dW4 = _MC(dX5);
					tmp1 = dW4, tmp2 = dX4[idx];
					for (k = 3; k >= 0; k--) {
						a[k] = tmp1, b[k] = tmp2;
						tmp1 = tmp1 >> 8, tmp2 = tmp2 >> 8;
						if (tab[b[k]][a[k]].size() == 0) {
							break;
						}
					}
					if (k == -1) {
						X5[0].clear(), X5[1].clear();
						int sam = tab[i][ca].size(), sbm = tab[j][cb].size(), sx = 0;
						for (int sa = 0; sa < sam; sa++) {
							for (int sb = 0; sb < sbm; sb++) {
								X5[0].push_back((tab[i][ca][sa].x << cap) | (tab[j][cb][sb].x << cbp) | X5_mask[idx]);
								X5[1].push_back((tab[i][ca][sa].x << cap) | (tab[j][cb][sb].x << cbp) | (X5_mask[idx] ^ dX5_lef[idx]));
							}
						}

						int s0m = tab[b[0]][a[0]].size(), s1m = tab[b[1]][a[1]].size();
						int s2m = tab[b[2]][a[2]].size(), s3m = tab[b[3]][a[3]].size();
						sum += sam * sbm * 2 * s0m * s1m * s2m * s3m;
						for (int s0 = 0; s0 < s0m; s0++) {
							tmp1 = (tab[b[0]][a[0]][s0].x << 24);
							for (int s1 = 0; s1 < s1m; s1++) {
								tmp1 =  (tmp1&mask[0])| (tab[b[1]][a[1]][s1].x << 16);
								for (int s2 = 0; s2 < s2m; s2++) {
									tmp1 = (tmp1&(mask[0]|mask[1])) | (tab[b[2]][a[2]][s2].x << 8);
									for (int s3 = 0; s3 < s3m; s3++) {
										tmp1 = (tmp1 & (~mask[3])) | tab[b[3]][a[3]][s3].x;
										tmp2 = MC(SB(tmp1));
										//cout << (MC(SB(tmp1)) ^ MC(SB(tmp1^dX4[idx]))) << endl;
										for (int t = 0; t < X5[0].size(); t++) {
											X4[0][idx].insert(pr(tmp1, (tmp2 ^ X5[0][t])));
										}
										for (int t = 0; t < X5[1].size(); t++) {
											X4[1][idx].insert(pr(tmp1, (tmp2 ^ X5[1][t])));
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

void clsX4_01() {
	UINT8 a, b;
	set<pr>::iterator it;

	for (it = X4[0][0].begin(); it != X4[0][0].end(); it++) {
		pr tmp = *it;
		//cout << hex << tmp.x << " " << tmp.y << endl;
		a = tmp.x >> 24;
		b = tmp.x >> 16;
		X4_0[0][a][b].push_back(tmp);
	}

	for (it = X4[1][0].begin(); it != X4[1][0].end(); it++) {
		pr tmp = *it;
		a = tmp.x >> 24;
		b = tmp.x >> 16;
		X4_0[1][a][b].push_back(tmp);
	}

	for (it = X4[0][1].begin(); it != X4[0][1].end(); it++) {
		pr tmp = *it;
		a = tmp.x >> 24;
		X4_1[0][a].push_back(tmp);
	}
	for (it = X4[1][1].begin(); it != X4[1][1].end(); it++) {
		pr tmp = *it;
		a = tmp.x >> 24;
		X4_1[1][a].push_back(tmp);
	}
}

UINT32 fX4[4], fK4[4], fK3[4], fK2[4], fU3[4], fK5[4];
UINT8 fY2_01;

UINT32 cal_X4_1(int idx) {
	UINT8 a, b, A, B, c, d, C, D;
	UINT32 W3;

	fK3[1] = fK4[2] ^ fK4[3];

	fU3[1] = _MC(fK3[1]);
	c = fU3[1] >> 8, d = fU3[1];
	C = fX4[3] >> 8, D = fX4[2];
	if (idx == 0) {
		c ^= 0x7c, d ^= 0x7c;
	}
	else {
		c ^= 0x63, d ^= 0x77;
	}

	a = C ^ D ^ GF8[2][c] ^ c ^ GF8[3][d] ^ GF8[2][d];

	for (int i = 0; i < 256; i++) {
		if ((i ^ GF8[3][i]) == a) {
			a = i;
			break;
		}
	}
	b = C ^ a ^ GF8[2][c] ^ GF8[3][d];
	W3 = (a << 24) | (b << 16) | (c << 8) | d;
	return MC(W3);
}

UINT32 cek_X4_0(int idx) {
	UINT8 a, b, A, B, c, d, C, D, _B;
	UINT32 W3, _X4;
	fK3[0] = fK4[1] ^ fK4[2];
	fU3[0] = _MC(fK3[0]);
	a = fU3[0] >> 24;
	if (idx == 0) {
		a ^= 0x7c, d = (fU3[0] ^ 0x7c);
	}
	else {
		a ^= 0x7b, d = (fU3[0] ^ 0x63);
	}

	B = fX4[3] >> 16, C = fX4[2] >> 8, D = fX4[1];
	c = C ^ D ^ a ^ GF8[3][a] ^ GF8[2][d] ^ GF8[3][d];

	for (int i = 0; i < 256; i++) {
		if ((i^GF8[2][i]) == c) {
			c = i;
			break;
		}
	}

	b = C ^ a ^ GF8[2][c] ^ GF8[3][d];
	W3 = (a << 24) | (b << 16) | (c << 8) | d;
	_X4 = MC(W3), _B = _X4 >> 16;

	if (B == _B) {
		A = _X4 >> 24;
		return A;
	}
	return -1;
}

bool cek_X2(int idx) {
	UINT32 W2_3, Z2_3;
	UINT8 Y;
	fK2[3] = fK4[1] ^ fK4[0];
	if (idx == 0) {
		W2_3 = 0x01010101 ^ fK2[3];
	}
	else {
		W2_3 = 0x02030000 ^ fK2[3];
	}

	Z2_3 = _MC(W2_3);
	Y = Z2_3 >> 16;
	if (X2_0[Y].size() > 0) {
		fY2_01 = Y;
		return true;
	}
	return false;
}


const UINT32 X6[2][4] = {
	0xD7000000, 0x400000, 0xf300, 0x9e,
	0x52000000, 0x520000, 0x5200, 0x52
};
void getK5(int idx) {
	UINT32 Y5[4], W5[4];
	for (int i = 0; i < 4; i++) {
		Y5[i] = SB(MC(SB(fX4[i]))^fK4[i]);
	}
	SR(Y5);
	for (int i = 0; i < 4; i++) {
		W5[i] = MC(Y5[i]);
	}

	for (int i = 0; i < 4; i++) {
		fK5[i] = (W5[i] ^ X6[idx][i]) & mask[i];
	}

	fK5[2] |= (fK5[3] ^ fK4[1]) & mask[3];
	fK5[1] |= (fK5[2] ^ fK4[0]) & (mask[3] | mask[2]);
}

void getU3(int idx) {
	UINT32 W3[4];
	for (int i = 0; i < 4; i++) {
		W3[i] = fX4[i];
	}
	_SR(W3);
	for (int i = 2; i < 4; i++) {
		W3[i] = _MC(W3[i]);
	}
	if (idx == 0) {
		fU3[2] = (W3[2] ^ 0x7c7c7c7c) & (mask[1] | mask[2]);
		fU3[3] = (W3[3] ^ 0x7c7c7c7c) & (mask[1] | mask[0]);
	}
	else {
		fU3[2] = (W3[2] ^ 0x007b6300) & (mask[1] | mask[2]);
		fU3[3] = (W3[3] ^ 0x77630000) & (mask[1] | mask[0]);
	}
}

bool cek_Z2(int idx) {
	UINT32 W2_0;
	fK2[0] = (getKg(fK3[1], 2)) ^ fK3[2];
	if (idx == 0) {
		W2_0 = fK2[0] ^ 0x01010101;
	}
	else {
		W2_0 = fK2[0] ^ 0x03000002;
	}
	UINT8 tmp = _MC(W2_0) >> 24;
	for (int i = 0; i < X2_0[fY2_01].size(); i++) {
		if (X2_0[fY2_01][i] == tmp)
			return true;
	}
	return false;
}

bool cek_K3() {
	fK5[0] = (getKg(fK4[3], 3)) ^ fK3[2];
	fK3[3] = (fK5[0] ^ fK5[1]) & (~mask[0]);

	UINT8 a, b, c, d, A, B, C, D;
	a = fU3[3] >> 24, b = fU3[3] >> 16;
	B = fK3[3] >> 16, C = fK3[3] >> 8, D = fK3[3];
	A = a ^ GF8[11][B] ^ GF8[13][C] ^ GF8[9][D];
	for (int i = 0; i < 256; i++) {
		if (GF8[14][i] == A) {
			A = i;
			break;
		}
	}
	fK3[3] |= (A << 24);
	fU3[3] = _MC(fK3[3]);
	B = fU3[3] >> 16;
	if (B == b) {
		return true;
	}
	else {
		return false;
	}
}

bool getK3(int idx) {
	UINT8 a, b, c, d, A, B, C, D;

	fK3[2] = (getKg(fK4[3], 3) ^ fK5[0])&mask[0];
	b = fU3[2] >> 16, c = fU3[2] >> 8, A = fK3[2] >> 24;
	for (int i = 0; i < 256; i++) {
		a = A ^ i ^ GF8[3][b] ^ GF8[2][b] ^ GF8[3][c] ^ c;
		for (int j = 0; j < 256; j++) {
			if ((j ^ GF8[2][j]) == a) {
				a = j;
				break;
			}
		}
		d = A ^ GF8[2][a] ^ GF8[3][b] ^ c;
		fU3[2] = (a << 24) | (b << 16) | (c << 8) | d;
		fK3[2] = MC(fU3[2]);

		if (cek_Z2(idx)) {
			if (cek_K3())
				return true;
		}
	}
	return false;
}

void cal() {
	UINT32 _X4;
	UINT8 A, B;
	int _A;
	set<pr>::iterator i, j;
	for (int idx = 0; idx < 2; idx++) {
		for (i = X4[idx][3].begin(); i != X4[idx][3].end(); i++) {
			fX4[3] = i->x;
			fK4[3] = i->y;
			for (j = X4[idx][2].begin(); j != X4[idx][2].end(); j++) {
				fX4[2] = j->x;
				fK4[2] = j->y;

				_X4 = cal_X4_1(idx);
				A = (_X4 >> 24);
				B = (_X4 >> 16);

				for (int k = 0; k < X4_1[idx][A].size(); k++) {
					fX4[1] = X4_1[idx][A][k].x;
					fK4[1] = X4_1[idx][A][k].y;

					_A = cek_X4_0(idx);
					if (_A != -1) {
						for (int t = 0; t < X4_0[idx][_A][B].size(); t++) {
							fX4[0] = X4_0[idx][_A][B][t].x;
							fK4[0] = X4_0[idx][_A][B][t].y;

							if (cek_X2(idx)) {
								getK5(idx);
								getU3(idx);
								if (getK3(idx)) {
									return;
								}
							}
						}
					}
				}
			}
		}
	}
}

UINT32 K[7][6], K_1[4], KK[9][4];
UINT32 P[4], _P[4], _C[4], C[4], X[9][4], _X[9][4];

void testKey() {
	cout << "testK2" << endl;
	for (int i = 0; i < 4; i++) {
		cout << hex << KK[2][i] << " " << fK2[i] << endl;
	}
	cout << "testK3" << endl;
	for (int i = 0; i < 4; i++) {
		cout << hex << KK[3][i] << " " << fK3[i] << endl;
	}
	cout << "testK4" << endl;
	for (int i = 0; i < 4; i++) {
		cout << hex << KK[4][i] << " " << fK4[i] << endl;
	}
}

void testX() {
	UINT32 tmp[4];
	cout << "testX4" << endl;
	for (int i = 0; i < 4; i++) {
		tmp[i] = X[4][i];
	}

	SR(tmp);
	for (int i = 0; i < 4; i++) {
		cout << hex << tmp[i] << " " << fX4[i] << endl;
	}
	cout << "testX5" << endl;
	for (int i = 0; i < 4; i++) {
		cout << (SB(MC(SB(fX4[i]))^fK4[i]) ^ SB(MC(SB(fX4[i] ^ dX4[i]))^fK4[i])) <<endl;
		cout << hex << X[5][i] << " " << _X[5][i] << endl;
		cout << X5_mask[i] << endl;
	}
}
void AES(int rod, UINT32 obj[4], UINT32 pre[4]) {
	for (int i = 0; i < 4; i++) {
		obj[i] = SB(pre[i]);
	}
	SR(obj);
	for (int i = 0; i < 4; i++) {
		obj[i] = MC(obj[i]) ^ KK[rod][i];
	}
}

void _AES(int rod, UINT32 obj[4], UINT32 nxt[4]) {
	for (int i = 0; i < 4; i++) {
		obj[i] = _MC(nxt[i] ^ KK[rod][i]);
	}
	_SR(obj);
	for (int i = 0; i < 4; i++) {
		obj[i] = _SB(obj[i]);
	}
}

void getRes() {
	K[3][0] = fK3[2], K[3][1] = fK3[3];
	for (int i = 0; i < 4; i++) {
		K[3][2 + i] = fK4[i];
	}
	for (int i = 2; i >= 0; i--) {
		for (int j = 1; j <= 6; j++) {
			if (j == 6) {
				K[i][0] = getKg(K[i][5], i) ^ K[i + 1][0];
			}
			else {
				K[i][j] = (K[i + 1][j] ^ K[i + 1][j - 1]);
			}
		}
	}

	for (int i = 1; i < 7 ; i++) {
		for (int j = 0; j < 6; j++) {
			if (j == 0) {
				K[i][0] = getKg(K[i-1][5], i-1) ^ K[i-1][0];
			}
			else {
				K[i][j] = (K[i-1][j] ^ K[i][j - 1]);
			}
		}
	}
	for (int i = 0; i < 4; i++) {
		K_1[i] = K[0][i];
	}
	int x = 0, y = 4;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 4; j++) {
			KK[i][j] = K[x][y++];
			if (y == 6) {
				x++, y = 0;
			}
		}
	}
//	testKey();

	for (int i = 0; i < 4; i++) {
		X[4][i] = fX4[i];
		_X[4][i] = fX4[i] ^ dX4[i];
	}
	_SR(X[4]);
	_SR(_X[4]);

	for (int i = 3; i >= 0; i--) {
		_AES(i, X[i], X[i+1]);
		_AES(i, _X[i], _X[i+1]);
	}
	for (int i = 0; i < 4; i++) {
		P[i] = K_1[i] ^ X[0][i];
		_P[i] = K_1[i] ^ _X[0][i];
	}

	for (int i = 1; i < 9; i++) {
		AES(i-1, X[i], X[i-1]);
		AES(i-1, _X[i], _X[i-1]);
	}
//	testX();
	for (int i = 0; i < 4; i++) {
		C[i] = SB(X[8][i]);
		_C[i] = SB(_X[8][i]);
	}
	SR(C), SR(_C);
	for (int i = 0; i < 4; i++) {
		C[i] ^= K[8][i];
		_C[i] ^= K[8][i];
	}
}

void output() {
	cout << "\n\nKey\n";
	for (int i = 0; i < 6; i++) {
		cout << hex << K[0][i] << endl;
	}

	cout << "\nm\n";
	for (int i = 0; i < 4; i++) {
		printf("%08x ", P[i]);
	}
	cout << endl;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%08x ", X[i][j]);
		}
		cout << endl;
	}
	for (int i = 0; i < 4; i++) {
		printf("%08x ", C[i]);
	}

	cout << "\nm'\n";
	for (int i = 0; i < 4; i++) {
		printf("%08x ", _P[i]);
	}
	cout << endl;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%08x ", _X[i][j]);
		}
		cout << endl;
	}
	for (int i = 0; i < 4; i++) {
		printf("%08x ", _C[i]);
	}

	cout << "\nm^m'\n";
	for (int i = 0; i < 4; i++) {
		printf("%08x ", _P[i]^P[i]);
	}
	cout << endl;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 4; j++) {
			printf("%08x ", _X[i][j]^ X[i][j]);
		}
		cout << endl;
	}
	for (int i = 0; i < 4; i++) {
		printf("%08x ", _C[i]^ C[i]);
	}
}

void test() {
	UINT32 tmp[4], obj[4], k[4];
	for (int i = 0; i < 4; i++) {
		tmp[i] = rand();
		k[i] = rand();
		cout << tmp[i] << " ";
	}
	cout << endl;
	for (int i = 0; i < 4; i++) {
		obj[i] = SB(tmp[i]);
	}
	SR(obj);
	for (int i = 0; i < 4; i++) {
		obj[i] = MC(obj[i]) ^ k[i];
	}
	for (int i = 0; i < 4; i++) {
		obj[i] = _MC(obj[i] ^ k[i]);
	}
	_SR(obj);
	for (int i = 0; i < 4; i++) {
		tmp[i] = _SB(obj[i]);
	}

	for (int i = 0; i < 4; i++) {
		cout << tmp[i] << " ";
	}
	cout << endl;
}

int main() {
	ddt();
	getX2();
	getX4(0x6c, 0xa4, 24, 0, 2);
	getX4(0xf6, 0x87, 24, 16, 3);
	getX4(0x75, 0xde, 8, 0, 1);
	getX4(0xeb, 0x94, 16, 8, 0);
	clsX4_01();

	cal();
	getRes();
	//test();
	output();
	int zby;
	cin >> zby;


	return 0;
}
