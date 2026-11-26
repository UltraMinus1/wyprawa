#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>

int str_to_int(char* str) {
	int i = 0, num = 0;
	while (str[i] != '\0') {
		num *= 10;
		num += str[i] - '0';
		i++;
	}
	return num;
}

void render(int n, int m, wchar_t plansza[n][m], int x, int y, wchar_t* chop, int* diamenty);
int wczytaj_mape(FILE* file, int n, int m, wchar_t plansza[n][m]);
int validuj_mape(int n, int m, wchar_t plansza[n][m]);
void handle_input(int* x, int* y, int n, int m, int chop_len);
void validate_move(int* x, int* y, int n, int m, int chop_len);
void handle_pickup(wchar_t* symbol, int* diamenty);

int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "");
	if (argc != 4) {
		printf("niepoprawna ilosc argumentow\n");
		printf("wywolaj program w nastepujacy sposob:\n");
		printf("./wyprawa <n> <m> <scierzka do pliku z mapa>\n");
		return 1;
	}
	FILE* file;
	file = fopen(argv[3], "r");
	if (file == NULL) {
		printf("nie udalo sie wczytac pliku\n");
		return 1;
	}
	int n, m;
	n = str_to_int(argv[1]);
	m = str_to_int(argv[2]);
	wchar_t plansza[n][m];
	wczytaj_mape(file, n, m, plansza);
	fclose(file);
	if (!validuj_mape(n, m, plansza)) {
		printf("blendny plik inicjujacy\n");
		return 1;
	}
	wchar_t* chopek = L"¯\\_(ツ)_/¯";
	int actual_len = 10;
	int x = 0, y = 0;
	int diamenty = 0;
	//game loop
	while (1) {
		render(n, m, plansza, x, y, chopek, &diamenty);
		handle_input(&x, &y, n, m, actual_len);
	}
	return 0;
}


int wczytaj_mape(FILE* file, int n, int m, wchar_t plansza[n][m]) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			plansza[i][j] = getwc(file);		
		}
    		getwc(file);
	}
	return 0;
}
int validuj_mape(int n, int m, wchar_t plansza[n][m]) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (plansza[i][j] != '.' && plansza[i][j] != '$' && plansza[i][j] != 'O') {
				return 0;
			}
		}
	}
	return 1;
}
void handle_input(int* x, int* y, int n, int m, int chop_len) {
	char move;
	scanf(" %c", &move);
	switch (move) {
		case 'w':
			*y -= 1;
			break;
		case 's':
			*y += 1;
			break;
		case 'a':
			*x -= 1;
			break;
		case 'd':
			*x += 1;
			break;
	}
	validate_move(x, y, n, m, chop_len);
}
void validate_move(int* x, int* y, int n, int m, int chop_len) {
	if (*y < 0) {
		*y = 0;
	} else if (*y >= n) {
		*y = n - 1;
	}
	if (*x < 0) {
		*x = 0;
	} else if (*x - 1 + chop_len >= m) {
		*x = m - chop_len;
	}
}
void render(int n, int m, wchar_t plansza[n][m], int x, int y, wchar_t* chop, int* diamenty) {
	system("clear");
	int chop_len = wcslen(chop);
	for (int i = 0; i < n; i++) {
		for(int j = 0; j < m; j++) {
			if (i == y && j >= x && j < x + chop_len) {
				handle_pickup(&plansza[i][j], diamenty);
				putwc(chop[j - x], stdout);
			} else if (i == y && j == x + chop_len) {
				handle_pickup(&plansza[i][j], diamenty);
			}	
			else {
				putwc(plansza[i][j], stdout);
			}
		}
		putwc(L'\n', stdout);
	}
	wprintf(L"Diamenty: %d\n", *diamenty);
}

void handle_pickup(wchar_t* symbol, int* diamenty) {
	if (*symbol == L'$') {
		*diamenty += 1;
		*symbol = L'.';
	}
}
