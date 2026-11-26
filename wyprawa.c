#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

#define Y_MAX 500
#define X_MAX 500

int str_to_int(char* str) {
	int i = 0, num = 0;
	while (str[i] != '\0') {
		num *= 10;
		num += str[i] - '0';
		i++;
	}
	return num;
}

void render(int n, int m, wchar_t plansza[Y_MAX][X_MAX], int x, int y, wchar_t* chop, int* diamenty);
int wczytaj_mape(FILE* file, int n, int m, wchar_t plansza[Y_MAX][X_MAX]);
int validuj_mape(int n, int m, wchar_t plansza[Y_MAX][X_MAX]);
void handle_input(int* x, int* y, int n, int m, int chop_len, int diamenty, wchar_t plansza[Y_MAX][X_MAX]);
void validate_move(int* x, int* y, int n, int m, int chop_len);
void handle_pickup(wchar_t* symbol, int* diamenty);
void save(int x, int y, int n, int m, wchar_t plansza[Y_MAX][X_MAX], int diamenty);
void load(int* n, int* m, int* x, int* y, int* diamenty, wchar_t plansza[Y_MAX][X_MAX]);
int licz_diamenty(int n, int m, wchar_t plansza[Y_MAX][X_MAX]);
void write_to_log(char* text);

int main(int argc, char *argv[]) {
	setlocale(LC_ALL, "");
	int n, m;
	int x, y;
	int diamenty;
	wchar_t plansza[Y_MAX][X_MAX];
	if (argc == 1) {
		load(&n, &m, &x, &y, &diamenty, plansza);
		write_to_log("loaded a game");
	} else if (argc == 4) {
		x = y = diamenty = 0;
		FILE* file;
		file = fopen(argv[3], "r");
		if (file == NULL) {
			printf("nie udalo sie wczytac pliku\n");
			return 1;
		}

		n = str_to_int(argv[1]);
		m = str_to_int(argv[2]);
	
		wczytaj_mape(file, n, m, plansza);
		fclose(file);
		if (!validuj_mape(n, m, plansza)) {
			printf("blendny plik inicjujacy\n");
			return 1;
		}	
		write_to_log("started a new game");
	} else {
		printf("niepoprawna ilosc argumentow\n");
		printf("wywolaj program w nastepujacy sposob:\n");
		printf("./wyprawa <n> <m> <scierzka do pliku z mapa>\n");
		return 1;
	}

	int do_wygrania = licz_diamenty(n, m, plansza) + diamenty;
	
	
	
	wchar_t* chopek = L"¯\\_(ツ)_/¯";
	int actual_len = 10;
	
	//game loop
	render(n, m, plansza, x, y, chopek, &diamenty);
	while (diamenty < do_wygrania) {
		handle_input(&x, &y, n, m, actual_len, diamenty, plansza);
		render(n, m, plansza, x, y, chopek, &diamenty);
	}
	system("clear");
	wprintf(L"gratulacje wygrales\n");
	write_to_log("gracz wygral");
	return 0;
}


int wczytaj_mape(FILE* file, int n, int m, wchar_t plansza[Y_MAX][X_MAX]) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			plansza[i][j] = getwc(file);		
		}
		getwc(file);
	}
	return 0;
}
int validuj_mape(int n, int m, wchar_t plansza[Y_MAX][X_MAX]) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (plansza[i][j] != '.' && plansza[i][j] != '$' && plansza[i][j] != 'O') {
				return 0;
			}
		}
	}
	return 1;
}
void handle_input(int* x, int* y, int n, int m, int chop_len, int diamenty, wchar_t plansza[Y_MAX][X_MAX]) {
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
		case 'z':
			save(*x, *y, n, m, plansza, diamenty);
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
void render(int n, int m, wchar_t plansza[Y_MAX][X_MAX], int x, int y, wchar_t* chop, int* diamenty) {
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
		write_to_log("gracz podniusl diament");
	} else if(*symbol == L'O') {
		system("clear");
		wprintf(L"przegrales :(\n");
		write_to_log("gracz wpadl do dziury");
		exit(EXIT_FAILURE);
	}
}

void save(int x, int y, int n, int m, wchar_t plansza[Y_MAX][X_MAX], int diamenty) {
	//pierwszy rzad: x y n m diamenty
	//kolejne: plansza
	system("clear");
	char nazwa_zapisu[55];
	wprintf(L"podaj nazwe zapisu(max 50 znakow): ");
	scanf("%50s", nazwa_zapisu);
	strcat(nazwa_zapisu, ".wgs");
	FILE* saveFile;
	saveFile = fopen(nazwa_zapisu, "w");
	fprintf(saveFile, "%d %d %d %d %d\n", x, y, n, m, diamenty);
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			putc(plansza[i][j], saveFile);
		}
		putc('\n', saveFile);
	}
	write_to_log("zapisano gre");
	exit(EXIT_SUCCESS);
}

void load(int* n, int* m, int* x, int* y, int* diamenty, wchar_t plansza[Y_MAX][X_MAX]) {
	char saveName[55];
	system("clear");
	wprintf(L"ktory z tych plikow chcesz zaladowac?\n");
	system("ls . | grep '.wgs'");
	wprintf(L"------------------------\n");
	scanf("%54s", saveName);
	FILE* file;
	file = fopen(saveName, "r");
	fscanf(file, "%d %d %d %d %d ", x, y, n, m, diamenty);
	for(int i = 0; i < *n; i++) {
		for (int j = 0; j < *m; j++) {
			plansza[i][j] = getwc(file);
		}
		getwc(file);
	}
	fclose(file);
}

int licz_diamenty(int n, int m, wchar_t plansza[Y_MAX][X_MAX]) {
	int ile = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			if (plansza[i][j] == L'$') {
				ile++;
			}
		}
	}
	return ile;
}

void write_to_log(char* text) {
	FILE* logFile;
	logFile = fopen("log.txt", "a");
	fprintf(logFile, "%s\n", text);
	fclose(logFile);
}
