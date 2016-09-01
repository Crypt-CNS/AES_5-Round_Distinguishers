#include<iostream>
#include<cstdio>
#include<ctime>
#include<cstring>
#include<vector>
#include<cstdlib>
#include"type.h"
#include"AES.h"
using namespace std;

struct pr {
	UINT32 x, y;
	pr(UINT32 a = 0, UINT32 b = 0) {
		x = a, y = b;
	}
};
vector<pr> tab[256][256];
UINT32 X3[4], K3[4], K2[4], U2[4], K4[4];

void testSB() {
	int din, dout;
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j < 256; j++) {
			din = i ^ j;
			dout = SBOX[i] ^ SBOX[j];
			tab[din][dout].push_back(pr(i, j));
			/*table[din][dout][0] = i;
			table[din][dout][1] = j;*/
		}
	}
	/*int sum = 0;
	while (cin >> hex >> dout) {
		sum = 0;
		for (int i = 0; i < 256; i++) {
			cout << tab[i][dout].size() <<" ";
			sum += tab[i][dout].size();
		}
		cout << endl << sum << endl;
	}

	while (cin >> hex >> din >> dout) {
		for (int i = 0; i < tab[din][dout].size(); i++) {
			cout << hex << tab[din][dout][i].x << tab[din][dout][i].y << endl;
		}
	}*/
}


const UINT32 dX3[4][4] = {
	0x0e, 0x0b, 0x3e, 0x1f,
	0x0b, 0x21, 0x1f, 0x09,
	0x1f, 0x3e, 0x07, 0x16,
	0x21, 0x07, 0x1d, 0x1f
};
const UINT32 dY4_12 = 0xeb;
const UINT32 dY4_13 = 0x94;

int X31_amt = 0, cX31[256][256];
UINT32 X31[17000], K31[17000];

void getX3_1() {
	memset(cX31, -1, sizeof(cX31));
	UINT32 dW3_1 = 0x6a0000f2, dY3_1, X3_1 = 0, _X3_1 = 0, W3_1, X4_1;
	UINT8 tmp[4], in[4];
	for (int i = 0; i < 256; i++) {
		//cout << i << endl;
		if (tab[i][dY4_12].size() > 0) {
			dW3_1 = 0x6a0000f2 | (i << 16);
			for (int j = 0; j < 256; j++) {
				dW3_1 = 0x6a0000f2 | (i << 16);
				if (tab[j][dY4_13].size() > 0) {
					dW3_1 |= (j << 8);
					X4_1 = 0xee000090 | (tab[i][dY4_12][0].x << 16) | (tab[j][dY4_13][0].x << 8);
					dY3_1 = _MC(dW3_1);
					/*cout << hex << i << " " << j << endl;
					cout << hex << dW3_1 << " " << dY3_1 << endl << endl;*/
					int t;
					for (t = 0; t < 4; t++) {
						tmp[t] = (dY3_1 << (t << 3)) >> 24;
						in[t] = dX3[0][t];
						if (tab[in[t]][tmp[t]].size() == 0) {
							break;
						}
					}
					if (t == 4) {
						for (int a = 0; a < tab[in[0]][tmp[0]].size()/2; a++) {
							X3_1 = tab[in[0]][tmp[0]][a].x;
							_X3_1 = tab[in[0]][tmp[0]][a].y;
							for (int b = 0; b < tab[in[1]][tmp[1]].size() / 2; b++) {
								X3_1 = (X3_1 << 8) | tab[in[1]][tmp[1]][b].x;
								_X3_1 = (_X3_1 << 8) | tab[in[1]][tmp[1]][b].y;
								for (int c = 0; c < tab[in[2]][tmp[2]].size() / 2; c++) {
									X3_1 = (X3_1 << 8) | tab[in[2]][tmp[2]][c].x;
									_X3_1 = (_X3_1 << 8) | tab[in[2]][tmp[2]][c].y;
									for (int d = 0; d < tab[in[3]][tmp[3]].size() / 2; d++) {
										X3_1 = (X3_1 << 8) | tab[in[3]][tmp[3]][d].x;
										_X3_1 = (_X3_1 << 8) | tab[in[3]][tmp[3]][d].y;

										cX31[tab[in[0]][tmp[0]][a].x][tab[in[1]][tmp[1]][b].x] = X31_amt;
										X31[X31_amt] = X3_1;
										K31[X31_amt++] = MC(SB(X3_1)) ^ X4_1;
										cX31[tab[in[0]][tmp[0]][a].y][tab[in[1]][tmp[1]][b].y] = X31_amt;
										X31[X31_amt] = _X3_1;
										K31[X31_amt++] = MC(SB(_X3_1)) ^ X4_1;
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

pr getvalid(UINT32 rot, UINT32 pa, UINT32 pb, UINT32 ma, UINT32 mb, UINT32 Xm, UINT32 dX3) {
	UINT8 tmp1, tmp2;
	UINT32 res, X4, dW3, dY3, t;
	pr red;
	while (true) {
		res = 0;
		tmp1 = rand() % 256;
		if (tab[tmp1][ma].size() > 0) {
			tmp2 = rand() % 256;
			if (tab[tmp2][mb].size() > 0) {
				dW3 = rot | (tmp1 << pa) | (tmp2 << pb);
				X4 = Xm | (tab[tmp1][ma][0].x << pa) | (tab[tmp2][mb][0].x << pb);
				dY3 = _MC(dW3);
				for (t = 0; t < 4; t++) {
					tmp1 = dY3 << (t << 3) >> 24;
					tmp2 = dX3 << (t << 3) >> 24;
					if (tab[tmp2][tmp1].size() > 0) {
						res = (res << 8) | tab[tmp2][tmp1][0].x;
					}
					else {
						break;
					}
				}
				if (t == 4) {
					red.x = res;
					red.y = MC(SB(res)) ^ X4;
					return red;
				}
			}
		}
	}
}

const UINT32 dW3_2 = 0x764c0000;
const UINT32 dW3_3 = 0x00f5c600;
const UINT32 dW3_4 = 0x00004a99;
const UINT8 dY4_23 = 0x75;
const UINT8 dY4_24 = 0xde;
const UINT8 dY4_31 = 0x6c;
const UINT8 dY4_34 = 0xa4;
const UINT8 dY4_41 = 0xf6;
const UINT8 dY4_42 = 0x87;
const UINT32 dX3_1 = 0x0e0b3e1f;
const UINT32 dX3_2 = 0x0b211f09;
const UINT32 dX3_3 = 0x1f3e0716;
const UINT32 dX3_4 = 0x21071d1f;
const UINT32 X4_2 = 0x216d0000;
const UINT32 X4_3 = 0x0071a400;
const UINT32 X4_4 = 0x00006bce;

UINT32 rc[] = {
	0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000
};

UINT32 getKg(UINT32 k, int rod) {
	k = (k << 8) | (k >> 24);
	UINT8 tmp, kk = 0;
	for (int i = 0; i < 4; i++) {
		tmp = (k << (i << 3)) >> 24;
		kk = (kk << 8) | SB(tmp);
	}
	kk ^= rc[rod];
	return kk;
}

const UINT32 Z2[4] = {
	0x7c000000, 0x0000007c, 0x00007c00, 0x007c0000
};

void getX3_234() {
	pr tmp;
	UINT8 X3_11, X3_12, a, b, c, W2_11, W2_24;
	int i;
	while (true) {
		tmp = getvalid(dW3_2, 8, 0, dY4_23, dY4_24, X4_2, dX3_2);
		X3[1] = tmp.x, K3[1] = tmp.y;
		tmp = getvalid(dW3_3, 24, 0, dY4_31, dY4_34, X4_3, dX3_3);
		X3[2] = tmp.x, K3[2] = tmp.y;
		tmp = getvalid(dW3_4, 24, 16, dY4_41, dY4_42, X4_4, dX3_4);
		X3[3] = tmp.x, K3[3] = tmp.y;
		K2[0] = getKg(K3[1], 2) ^ K3[2];
		K2[1] = K3[2] ^ K3[3];
		U2[0] = _MC(K2[0]), U2[1] = _MC(K2[1]);
		W2_11 = (Z2[0] >> 24) ^ (U2[0] >> 24);
		W2_24 = Z2[1] ^ U2[1];


		a = (X3[3] << 8) >> 24, b = (X3[2] << 16) >> 24, c = (X3[1] << 24) >> 24;
		X3_11 = GF8[11][a] ^ GF8[13][b] ^ GF8[9][c] ^ W2_11;
		for (i = 0; i < 256; i++) {
			if (GF8[14][i] == X3_11) {
				X3_11 = i;
				break;
			}
		}
		if (i < 256) {
			a = (X3[1]) >> 24, b = (X3[3] << 16) >> 24, c = (X3[2] << 24) >> 24;
			X3_12 = GF8[11][a] ^ GF8[9][b] ^ GF8[14][c] ^ W2_24;
			for (i = 0; i < 256; i++) {
				if (GF8[13][i] == X3_12) {
					X3_12 = i;
					break;
				}
			}
			if (i < 256) {
				if (cX31[X3_11][X3_12] != -1) {
					X3[0] = X31[cX31[X3_11][X3_12]];
					K3[0] = K31[cX31[X3_11][X3_12]];
					/*cout << "W2 : " << hex << (int)W2_11 << " "<< (int)W2_24 << endl;
					cout << "U2 : " << hex << U2[0] << " " << U2[1] << endl;
					cout << X3[0] << " " << (int)X3_11 << " " << (int)X3_12 << endl;*/

					return;
				}
			}
		}

	}
}

void check() {
	UINT32 Y4[4], _Y4[4], _X3[4], dx, dY4[4];
	int i, j;
	for (i = 0; i < 4; i++) {
		dx = 0;
		for (j = 0; j < 4; j++) {
			dx = (dx << 8) | dX3[i][j];
		}
		_X3[i] = X3[i] ^ dx;
	}

	for (i = 0; i < 4; i++) {
		Y4[i] = SB(MC(SB(X3[i])) ^ K3[i]);
		_Y4[i] = SB(MC(SB(_X3[i])) ^ K3[i]);
		dY4[i] = Y4[i] ^ _Y4[i];
	}

	cout << "Y4\n";
	for (i = 0; i < 4; i++) {
		cout << hex << Y4[i] << " ";
	}
	cout << "\nY4'\n";
	for (i = 0; i < 4; i++) {
		cout << hex << _Y4[i] << " ";
	}
	cout << "\ndY4\n";
	for (i = 0; i < 4; i++) {
		cout << hex << dY4[i] << " ";
	}
	cout << endl;
}

const UINT32 mask[4] = {
	0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff
};
const UINT32 X5[4] = {
	0xd7000000, 0x00400000, 0x0000f300, 0x0000009e
};

void getK4() {
	UINT32 W4[4], Y4[4];
	for (int i = 0; i < 4; i++) {
		Y4[i] = SB(MC(SB(X3[i])) ^ K3[i]);
	}

	W4[0] = (Y4[0] & mask[0]) | (Y4[1] & mask[1]) | (Y4[2] & mask[2]) | (Y4[3] & mask[3]);
	W4[1] = (Y4[1] & mask[0]) | (Y4[2] & mask[1]) | (Y4[3] & mask[2]) | (Y4[0] & mask[3]);
	W4[2] = (Y4[2] & mask[0]) | (Y4[3] & mask[1]) | (Y4[0] & mask[2]) | (Y4[1] & mask[3]);
	W4[3] = (Y4[3] & mask[0]) | (Y4[0] & mask[1]) | (Y4[1] & mask[2]) | (Y4[2] & mask[3]);

	for (int i = 0; i < 4; i++) {
		W4[i] = MC(W4[i]);
		K4[i] = (W4[i] ^ X5[i]) & mask[i];
	}
	K4[2] = K4[2] | ((K4[3] ^ K3[1])&mask[3]);
	K4[1] = K4[1] | ((K4[2] ^ K3[0])&(mask[2] | mask[3]));
}

void getU2() {
	UINT32 W2[4], XX3[4];

	XX3[0] = (X3[0] & mask[0]) | (X3[3] & mask[1]) | (X3[2] & mask[2]) | (X3[1] & mask[3]);
	XX3[1] = (X3[1] & mask[0]) | (X3[0] & mask[1]) | (X3[3] & mask[2]) | (X3[2] & mask[3]);
	XX3[2] = (X3[2] & mask[0]) | (X3[1] & mask[1]) | (X3[0] & mask[2]) | (X3[3] & mask[3]);
	XX3[3] = (X3[3] & mask[0]) | (X3[2] & mask[1]) | (X3[1] & mask[2]) | (X3[0] & mask[3]);

	for (int i = 2; i < 4; i++) {
		W2[i] = _MC(XX3[i]);
		U2[i] = (W2[i] ^ Z2[i])&mask[4 - i];
	}
	/*cout << "W2_1 : " << _MC(XX3[0]) << endl;
	cout << "W2_2 : " << _MC(XX3[1]) << endl;
	cout << "U2^Z2_1 : " << (U2[0] ^ Z2[0]) << endl;
	cout << "U2^Z2_2 : " << (U2[1] ^ Z2[1]) << endl;*/
}

void getK2() {
	K2[2] = (K3[3] ^ K4[0]) & mask[0];

	UINT8 a, b, c, d, K2_34, K2_41;
	int i;
	a = (K2[2] >> 24);
	d = (U2[2] >> 8);
	while (true) {
		b = rand() % 256;
		c = rand() % 256;

		K2_34 = GF8[13][a] ^ GF8[9][b] ^ GF8[14][c] ^ d;
		for (i = 0; i < 256; i++) {
			if (GF8[11][i] == K2_34) {
				K2[2] = (a << 24) | (b << 16) | (c << 8) | i;
				break;
			}
		}
		if (i < 256) {
			break;
		}
	}
	K4[0] = K3[3] ^ K2[2];
	K2[3] = K4[0] ^ K4[1];

	a = K2[3] >> 16;
	b = K2[3] >> 8;
	c = K2[3];
	d = U2[3] >> 16;
	K2_41 = GF8[14][a] ^ GF8[11][b] ^ GF8[13][c] ^ d;
	for (i = 0; i < 256; i++) {
		if (GF8[9][i] == K2_41) {
			K2[3] = (i << 24) | (a << 16) | (b << 8) | c;
			break;
		}
	}
}


UINT32 XF[8][4], KKF[8][4], _XF[8][4];
void _AES(int idx) {
	for (int i = 0; i < 4; i++) {
		XF[idx][i] = _MC(XF[idx + 1][i] ^ KKF[idx][i]);
		_XF[idx][i] = _MC(_XF[idx + 1][i] ^ KKF[idx][i]);
	}

	UINT32 tmp[4], _tmp[4];

	tmp[0] = (XF[idx][0] & mask[0]) | (XF[idx][3] & mask[1]) | (XF[idx][2] & mask[2]) | (XF[idx][1] & mask[3]);
	tmp[1] = (XF[idx][1] & mask[0]) | (XF[idx][0] & mask[1]) | (XF[idx][3] & mask[2]) | (XF[idx][2] & mask[3]);
	tmp[2] = (XF[idx][2] & mask[0]) | (XF[idx][1] & mask[1]) | (XF[idx][0] & mask[2]) | (XF[idx][3] & mask[3]);
	tmp[3] = (XF[idx][3] & mask[0]) | (XF[idx][2] & mask[1]) | (XF[idx][1] & mask[2]) | (XF[idx][0] & mask[3]);

	_tmp[0] = (_XF[idx][0] & mask[0]) | (_XF[idx][3] & mask[1]) | (_XF[idx][2] & mask[2]) | (_XF[idx][1] & mask[3]);
	_tmp[1] = (_XF[idx][1] & mask[0]) | (_XF[idx][0] & mask[1]) | (_XF[idx][3] & mask[2]) | (_XF[idx][2] & mask[3]);
	_tmp[2] = (_XF[idx][2] & mask[0]) | (_XF[idx][1] & mask[1]) | (_XF[idx][0] & mask[2]) | (_XF[idx][3] & mask[3]);
	_tmp[3] = (_XF[idx][3] & mask[0]) | (_XF[idx][2] & mask[1]) | (_XF[idx][1] & mask[2]) | (_XF[idx][0] & mask[3]);

	for (int i = 0; i < 4; i++) {
		XF[idx][i] = _SB(tmp[i]);
		_XF[idx][i] = _SB(_tmp[i]);
	}
}

void DAES(int idx) {
	UINT32 stmp[4], _stmp[4];
	for (int i = 0; i < 4; i++) {
		stmp[i] = SB(XF[idx][i]);
		_stmp[i] = SB(_XF[idx][i]);
	}

	UINT32 tmp[4], _tmp[4];

	tmp[0] = (stmp[0] & mask[0]) | (stmp[1] & mask[1]) | (stmp[2] & mask[2]) | (stmp[3] & mask[3]);
	tmp[1] = (stmp[1] & mask[0]) | (stmp[2] & mask[1]) | (stmp[3] & mask[2]) | (stmp[0] & mask[3]);
	tmp[2] = (stmp[2] & mask[0]) | (stmp[3] & mask[1]) | (stmp[0] & mask[2]) | (stmp[1] & mask[3]);
	tmp[3] = (stmp[3] & mask[0]) | (stmp[0] & mask[1]) | (stmp[1] & mask[2]) | (stmp[2] & mask[3]);

	_tmp[0] = (_stmp[0] & mask[0]) | (_stmp[1] & mask[1]) | (_stmp[2] & mask[2]) | (_stmp[3] & mask[3]);
	_tmp[1] = (_stmp[1] & mask[0]) | (_stmp[2] & mask[1]) | (_stmp[3] & mask[2]) | (_stmp[0] & mask[3]);
	_tmp[2] = (_stmp[2] & mask[0]) | (_stmp[3] & mask[1]) | (_stmp[0] & mask[2]) | (_stmp[1] & mask[3]);
	_tmp[3] = (_stmp[3] & mask[0]) | (_stmp[0] & mask[1]) | (_stmp[1] & mask[2]) | (_stmp[2] & mask[3]);

	for (int i = 0; i < 4; i++) {
		XF[idx+1][i] = MC(tmp[i]) ^ KKF[idx][i];
		_XF[idx+1][i] = MC(_tmp[i]) ^ KKF[idx][i];
	}
}

UINT32 C[4], _C[4];
void DDAES(int idx) {
	UINT32 stmp[4], _stmp[4];
	for (int i = 0; i < 4; i++) {
		stmp[i] = SB(XF[idx][i]);
		_stmp[i] = SB(_XF[idx][i]);
	}

	UINT32 tmp[4], _tmp[4];

	tmp[0] = (stmp[0] & mask[0]) | (stmp[1] & mask[1]) | (stmp[2] & mask[2]) | (stmp[3] & mask[3]);
	tmp[1] = (stmp[1] & mask[0]) | (stmp[2] & mask[1]) | (stmp[3] & mask[2]) | (stmp[0] & mask[3]);
	tmp[2] = (stmp[2] & mask[0]) | (stmp[3] & mask[1]) | (stmp[0] & mask[2]) | (stmp[1] & mask[3]);
	tmp[3] = (stmp[3] & mask[0]) | (stmp[0] & mask[1]) | (stmp[1] & mask[2]) | (stmp[2] & mask[3]);

	_tmp[0] = (_stmp[0] & mask[0]) | (_stmp[1] & mask[1]) | (_stmp[2] & mask[2]) | (_stmp[3] & mask[3]);
	_tmp[1] = (_stmp[1] & mask[0]) | (_stmp[2] & mask[1]) | (_stmp[3] & mask[2]) | (_stmp[0] & mask[3]);
	_tmp[2] = (_stmp[2] & mask[0]) | (_stmp[3] & mask[1]) | (_stmp[0] & mask[2]) | (_stmp[1] & mask[3]);
	_tmp[3] = (_stmp[3] & mask[0]) | (_stmp[0] & mask[1]) | (_stmp[1] & mask[2]) | (_stmp[2] & mask[3]);

	for (int i = 0; i < 4; i++) {
		C[i] = tmp[i] ^ KKF[idx][i];
		_C[i] = _tmp[i] ^ KKF[idx][i];
	}
}

UINT32 KF[6][6];
void final() {
	KF[2][0] = K2[0], KF[2][1] = K2[1], KF[2][2] = K2[2], KF[2][3] = K2[3];
	KF[2][4] = K3[0], KF[2][5] = K3[1], KF[3][0] = K3[2], KF[3][1] = K3[3];

	for (int j = 1; j >= 0; j--) {
		for (int i = 0; i < 6; i++) {
			if (i == 5) {
				KF[j][0] = KF[j + 1][0] ^ getKg(KF[j][5], j);
			}
			else
			{
				KF[j][i + 1] = KF[j + 1][i] ^ KF[j + 1][i + 1];
			}
		}
	}

	int x = 0, y = 4;
	for (int i = 0; i <= 3; i++) {
		for (int j = 0; j < 4; j++) {
			KKF[i][j] = KF[x][y++];
			if (y == 6) {
				x++, y = 0;
			}
		}
	}

	UINT32 tmp[4];
	tmp[0] = X3[0] ^ dX3_1;
	tmp[1] = X3[1] ^ dX3_2;
	tmp[2] = X3[2] ^ dX3_3;
	tmp[3] = X3[3] ^ dX3_4;

	_XF[3][0] = (tmp[0] & mask[0]) | (tmp[3] & mask[1]) | (tmp[2] & mask[2]) | (tmp[1] & mask[3]);
	_XF[3][1] = (tmp[1] & mask[0]) | (tmp[0] & mask[1]) | (tmp[3] & mask[2]) | (tmp[2] & mask[3]);
	_XF[3][2] = (tmp[2] & mask[0]) | (tmp[1] & mask[1]) | (tmp[0] & mask[2]) | (tmp[3] & mask[3]);
	_XF[3][3] = (tmp[3] & mask[0]) | (tmp[2] & mask[1]) | (tmp[1] & mask[2]) | (tmp[0] & mask[3]);


	XF[3][0] = (X3[0] & mask[0]) | (X3[3] & mask[1]) | (X3[2] & mask[2]) | (X3[1] & mask[3]);
	XF[3][1] = (X3[1] & mask[0]) | (X3[0] & mask[1]) | (X3[3] & mask[2]) | (X3[2] & mask[3]);
	XF[3][2] = (X3[2] & mask[0]) | (X3[1] & mask[1]) | (X3[0] & mask[2]) | (X3[3] & mask[3]);
	XF[3][3] = (X3[3] & mask[0]) | (X3[2] & mask[1]) | (X3[1] & mask[2]) | (X3[0] & mask[3]);


	for (int i = 2; i >= 0; i--) {
		_AES(i);
	}
}

void result() {
	UINT32 Kini[4], ini[4], _ini[4];
	for (int i = 0; i < 4; i++) {
		Kini[i] = KF[0][i];
		ini[i] = XF[0][i] ^ Kini[i];
		_ini[i] = _XF[0][i] ^ Kini[i];
	}

	for (int j = 1; j < 6; j++) {
		for (int i = 0; i < 6; i++) {
			if (i == 0) {
				KF[j][i] = KF[j - 1][i] ^ getKg(KF[j - 1][5], j - 1);
			}
			else {
				KF[j][i] = KF[j][i - 1] ^ KF[j - 1][i];
			}
		}
	}

	int x = 0, y = 4;
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			KKF[i][j] = KF[x][y++];
			if (y == 6) {
				x++, y = 0;
			}
		}
	}

	for (int i = 1; i < 8; i++) {
		DAES(i-1);
	}

	cout << "\nm\n";
	for (int i = -1; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == -1) {
				printf("%08x ", ini[j]);
			}
			else {
				printf("%08x ", XF[i][j]);
			}
		}
		cout << endl;
	}

	cout << "\nm'\n";
	for (int i = -1; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == -1) {
				printf("%08x ", _ini[j]);
			}
			else {
				printf("%08x ", _XF[i][j]);
			}
		}
		cout << endl;
	}

	cout << "\nm^m'\n";
	for (int i = -1; i < 8; i++) {
		for (int j = 0; j < 4; j++) {
			if (i == -1) {
				printf("%08x ", (ini[j] ^ _ini[j]));
			}
			else {
				printf("%08x ", (XF[i][j] ^ _XF[i][j]));
			}
		}
		cout << endl;
	}
	DDAES(7);
	cout << "\nC\n";
	for (int i = 0; i < 4; i++) {
		printf("%08x ", C[i]);
	}
	cout << "\nC'\n";
	for (int i = 0; i < 4; i++) {
		printf("%08x ", _C[i]);
	}
	cout << "\nC&C'\n";
	for (int i = 0; i < 4; i++) {
		printf("%08x ", (_C[i]^C[i]));
	}
}

void checkAll() {
	cout << "\n";
	for (int i = 0; i <= 3; i++) {
		for (int j = 0; j < 6; j++) {
			cout << KF[i][j] << " ";
		}
		cout << endl;
	}

	for (int j = 1; j < 4; j++) {
		for (int i = 0; i < 6; i++) {
			if (i == 0) {
				KF[j][i] = KF[j-1][i] ^ getKg(KF[j-1][5], j-1);
			}
			else {
				KF[j][i] = KF[j][i - 1] ^ KF[j-1][i];
			}
		}
	}

	for (int i = 0; i <= 3; i++) {
		for (int j = 0; j < 6; j++) {
			cout << KF[i][j] << ", ";
		}
		cout << endl;
	}


	/*cout << "KF3\n";
	for (int i = 0; i < 6; i++) {
		cout << hex << KF[3][i] << " ";
	}
	cout << "\nK3\n";
	for (int i = 0; i < 4; i++) {
		cout << hex << K3[i] << " ";
	}
	cout << "\nK4\n";
	for (int i = 0; i < 4; i++) {
		cout << hex << K4[i] << " ";
	}

	cout << "\n\nKKF3\n";
	for (int i = 0; i < 4; i++) {
		cout << KKF[3][i] << " ";
	}*/

	cout << "\n\nX3\n";
	for (int i = 0; i < 4; i++)
		printf("%08x ", XF[3][i]);
	cout << "\nX3'\n";
	for (int i = 0; i < 4; i++)
		cout << _XF[3][i] << " ";
	cout << "\ndX3'\n";
	for (int i = 0; i < 4; i++)
		cout << (_XF[3][i] ^ XF[3][i]) << " ";
	_AES(2);
	DAES(2);
	cout << "\n\nX3\n";
	for (int i = 0; i < 4; i++)
		cout << XF[3][i] << " ";
	cout << "\nX3'\n" ;
	for (int i = 0; i < 4; i++)
		cout << _XF[3][i] << " ";
	cout << "\ndX3'\n";
	for (int i = 0; i < 4; i++)
		cout << (_XF[3][i]^XF[3][i]) << " ";
}

int main(int argc, char *argv[]){
	srand(time(NULL));
	UINT32 tmp;
	/*for (int i = 0; i < 4; i++) {
		tmp = (dX3[i][0] << 24) | (dX3[(3+i)%4][1] << 16) | (dX3[(2+i)%4][2] << 8) | (dX3[(i+1)%4][3]);
		cout << hex << tmp << " ";
		tmp = _MC(tmp);
		cout << tmp << " ";
			cout <<  MC(tmp) << endl;
	}*/
	testSB();
	getX3_1();
	getX3_234();
	/*cout << "X3\n";
	for (int i = 0; i < 4; i++) {
		cout << hex << X3[i] << " ";
	}
	cout << "\nK3\n";
	for (int i = 0; i < 4; i++) {
		cout << hex << K3[i] << " ";
	}
	cout << endl;*/

	//check();
	getK4();
	getU2();
	getK2();
	final();
	cout << "X0\n";
	for (int i = 0; i < 4; i++)
		cout <<hex<< XF[0][i] << " ";
	cout << "\nX0'\n";
	for (int i = 0; i < 4; i++) {
		cout << _XF[0][i] << " ";
	}
	cout << "\nKey\n";
	for (int i = 0; i < 6; i++) {
		cout << KF[0][i] << " ";
	}
	//checkAll();
	result();

	int zby;
	cin >>zby;

	return 0;
}
