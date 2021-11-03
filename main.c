#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct data {
	int w;
	int p;
}Data;

char *readLine(FILE *fin) {
	char *s = NULL, *ns, c;
	int i = 0;
	while ((c = getc(fin)) != '\n') {
		if (i % 10 == 0) {
			ns = realloc(s,(i + 10) * sizeof(char));
			if (ns == NULL) {
				printf("GRESKA\n");
				free(s);
				exit(0);
			}
			else
				s = ns;
		}
		s[i++] = c;
	}
	ns = realloc(s, (i + 1) * sizeof(char));
	if (ns == NULL) {
		printf("GRESKA\n");
		free(s);
		exit(0);
	}
	else
		s = ns;
    s[i] = '\0';
	if (s[0] == '\0') {
		return NULL;
	}
	return s;
}

void addNode(Data *inf, char ins, int val, int p) {
	inf[ins - 'a'].w = val;
	inf[ins - 'a'].p = p;
}

void addEdge(int mat[][26], Data inf[], char *op, char ins) {
	if ((op[0] >= 'a' && op[0] <= 'z') && inf[op[0] - 'a'].w != 0) {
		mat[op[0] - 'a'][ins - 'a'] = inf[op[0] - 'a'].w;
	}
}

char *formGraph(FILE *fin, int mat[][26], Data *inf) {
	int i, val, j = 0;
	char *op1 = NULL, *op2 = NULL, op[4], *s, *ps, ins, *nop, *order = NULL, *norder;
	while ((s = readLine(fin)) != NULL) {
		ps = s;
		ins = *ps;
		norder = realloc(order, (j + 1) * sizeof(char));
		if (norder == NULL) {
			printf("GRESKA\n");
			exit(0);
		}
		else
			order = norder;
		order[j++] = ins;
		ps += 4;
		if (*ps != 'n' || (*ps == 'n' && *(ps+1) != 'o')) {
			i = 0;
			while (*ps != ' ') {
				nop = realloc(op1, (i + 1) * sizeof(char));
				if (nop == NULL){
					printf("GRESKA\n");
					exit(0);
				}
				op1 = nop;
				op1[i++] = *ps;
				ps++;
			}
		}
		nop = realloc(op1, (i + 1) * sizeof(char));
		if (nop == NULL) {
			printf("GRESKA\n");
			exit(0);
		}
		op1 = nop;
		op1[i] = '\0';
		ps++;
		i = 0;
		while (*ps != ' ') {
			op[i++] = *ps;
			ps++;
		}
		op[i] = '\0';
		i = 0;
		ps++;
		while (*ps != '\0') {
			nop = realloc(op2, (i + 1) * sizeof(char));
			if (nop == NULL) {
				printf("GRESKA\n");
				exit(0);
			}
			op2 = nop;
			op2[i++] = *ps;
			ps++;
		}
		nop = realloc(op2, (i + 1) * sizeof(char));
		if (nop == NULL) {
			printf("GRESKA\n");
			exit(0);
		}
		op2 = nop;
		op2[i] = '\0';
		if (op[0] == '+' || op[0] == '-')
			val = 3;
		else if (op[0] == '*')
			val = 10;
		else if (op[0] == '/')
			val = 23;
		else
			val = 1;
		addNode(inf, ins, val, j-1);
		addEdge(mat, inf, op1, ins);
		addEdge(mat, inf, op2, ins);
		free(s);
		s = NULL;
		free(op1);
		op1 = NULL;
		free(op2);
		op2 = NULL;
	}
	norder = realloc(order, (j + 1) * sizeof(char));
	if (norder == NULL) {
		printf("GRESKA\n");
		exit(0);
	}
	else
		order = norder;
	order[j] = '\0';
	return order;
}

void printGraph(FILE *fout, int mat[][26], Data inf[]) {
	int i, j, k;
	for (i = 0; i < 26; i++) {
		k = 0;
		if (inf[i].w != 0)
			fprintf(fout, "%c (w = %d) -> ", 'a' + i, inf[i].w);
		for (j = 0; j < 26; j++) {
			if (mat[i][j] != 0) {
				if (k != 0)
					fprintf(fout, ", ");
				else
					k = 1;
				fprintf(fout, "%c", 'a' + j);
			}
		}
		if (inf[i].w != 0)
			fprintf(fout, "\n");
	}
}

int hasPrev(int mat[][26], int j) {
	int i;
	for (i = 0; i < 26; i++) {
		if (mat[i][j] != 0)
			return 1;
	}
	return 0;
}

int hasPost(int mat[][26], int i) {
	int j;
	for (j = 0; j < 26; j++) {
		if (mat[i][j] != 0)
			return 1;
	}
	return 0;
}

void criticalPath(int mat[][26], Data inf[], char *order) {
	int *est, *lst, i = 0, j, k, cpl = 0, l = 0;
	char *s = order, *cp;
	est = malloc(strlen(order) * sizeof(int));
	lst = malloc(strlen(order) * sizeof(int));
	cp = malloc((strlen(order) + 1) * sizeof(char));
	if (est == NULL || lst == NULL || cp == NULL) {
		printf("GRESKA\n");
		exit(0);
	}
	while (*s != '\0') {
		if (!hasPrev(mat, *s - 'a')) {
			est[i++] = 0;
		}
		else {
			k = 0;
			for (j = 0; j < 26; j++) {
				if (mat[j][*s - 'a'] != 0) {
					if (k == 0) {
						est[i] = est[inf[j].p] + mat[j][*s - 'a'];
						k = 1;
					}
					else {
						if (est[i] < est[inf[j].p] + mat[j][*s - 'a'])
							est[i] = est[inf[j].p] + mat[j][*s - 'a'];
					}
				}
			}
			i++;
		}
		s++;
	}
	for (i = 0; i < strlen(order); i++) {
		if (est[i] + inf[order[i] - 'a'].w > cpl)
			cpl = est[i] + inf[order[i] - 'a'].w;
	}
	s = order + strlen(order) - 1;
	i = strlen(order) - 1;
	while (s >= order) {
		if (!hasPost(mat, *s - 'a')) {
			lst[i] = cpl - inf[*s-'a'].w;
		}
		else {
			k = 0;
			for (j = 0; j < 26; j++) {
				if (mat[*s-'a'][j] != 0) {
					if (k == 0) {
						lst[i] = lst[inf[j].p] - mat[*s - 'a'][j];
						k = 1;
					}
					else {
						if (lst[i] > lst[inf[j].p] - mat[*s - 'a'][j])
							lst[i] = lst[inf[j].p] - mat[*s - 'a'][j];
					}
				}
			}
		}
		s--;
		i--;
	}
	k = 0;
	printf("Duzina kriticnog puta je %d.\n", cpl);
	printf("Na kriticnom putu se nalaze cvorovi ");
	for (i = 0; i < strlen(order); i++) {
		if (k == 0 && est[i] == lst[i]) {
			printf("%c", order[i]);
			cp[k] = order[i];
			k = 1;
		}
		else if (k != 0 && est[i] == lst[i] && mat[cp[k-1] - 'a'][order[i] - 'a'] != 0) {
			printf(", %c", order[i]);
			cp[k++] = order[i];
		}
	}
	cp[k] = '\0';
	printf(".\nRaspored operacija po taktovima:\n");
	for (i = 0; i < cpl + 1; i++) {
		printf("%d: ", i);
		for (j = 0; j < strlen(order); j++) {
			if (i >= est[j] && i <= lst[j])
				printf("%c ", order[j]);
		}
		printf("\n");
	}
	free(est);
	free(lst);
	free(cp);
}

void printTransitive(int mat[][26], Data inf[], int c) {
	int p[26][26], i, j, k, t, h;
	for (i = 0; i < 26; i++) {
		for (j = 0; j < 26; j++) {
			if (mat[i][j] == 0)
				p[i][j] = 0;
			else
				p[i][j] = 1;
		}
	}
	for (k = 0; k < 26; k++) {
		for (i = 0; i < 26; i++) {
			if (p[i][k] == 1) {
				for (j = 0; j < 26; j++) {
					p[i][j] = p[i][j] | p[k][j];
				}
			}
		}
	}
	h = 0;
	for (j = 0; j < 26; j++) {
		t = 0;
		for (i = 0; i < 26; i++) {
			if (i != j && mat[c - 'a'][i] != 0 && p[i][j] == 1) {
				t = 1;
				break;
			}
		}
		if (h == 0) {
			if (p[c - 'a'][j] == 1 && mat[c - 'a'][j] != 0 && t) {
				printf("%c -> %c", c, 'a' + j);
				h = 1;
			}
		}
		else {
			if (p[c - 'a'][j] == 1 && mat[c - 'a'][j] != 0 && t) {
				printf(", %c -> %c", c, 'a' + j);
			}
		}
	}
	if (h == 0) {
		printf("nema ih.\n");
	}
	else {
		printf(".\n");
	}
}

void deleteNode(Data inf[], int i) {
	inf[i].w = 0;
	inf[i].p = 0;
}

void deleteEdge(int mat[][26], int i, int j) {
	mat[i][j] = 0;
}

void deleteGrapf(int mat[][26], Data inf[], char **order) {
	int i, j;
	for (i = 0; i < 26; i++) {
		deleteNode(inf, i);
	}
	for (i = 0; i < 26; i++) {
		for (j = 0; j < 26; j++) {
			deleteEdge(mat, i, j);
		}
	}
	free(*order);
	*order = NULL;
}

int main() {
	FILE *fin, *fout;
	Data inf[26];
	int mat[26][26], i, j, end = 1, input;
	char *order = NULL, *datin, *datout, c;
	for (i = 0; i < 26; i++) {
		for (j = 0; j < 26; j++) {
			mat[i][j] = 0;
		}
	}
	for (i = 0; i < 26; i++) {
		inf[i].w = 0;
		inf[i].p = 0;
	}
	while (end) {
		printf("\n1. Unos operacija\n"
			"2. Ispis grafa\n"
			"3. Duzina kriticnog puta, kriticni put i raspored operacija po taktovima\n"
			"4. Tranzitivne grane za unetu granu\n"
			"5. Brisanje grafa\n"
			"0. Zavrsetak rada\n\n"
			"Vas izbor? ");
		scanf("%d", &input);
		switch (input) {
		case 1: printf("Unesi ime datoteke, odnosno enter za standardni ulaz: ");
			fin = stdin;
			getchar();
			datin = readLine(fin);
			if (datin != NULL) {
				fin = fopen(datin, "r");
				if (fin == NULL) {
					printf("GRESKA\n");
					exit(0);
				}
			}
			order = formGraph(fin, mat, inf);
			break;
		case 2: if (order == NULL) {
			printf("Nema grafa!!!\n");
			break;
		    }
			fout = stdout;
			printf("Unesi ime datoteke, odnosno enter za standardni izlaz: ");
			getchar();
			datout = readLine(stdin);
			if (datout != NULL) {
				fout = fopen(datout, "w");
				if (fout == NULL) {
					printf("GRESKA");
					exit(0);
				}
			}
			fprintf(fout, "Izgled grafa:\n\n");
			printGraph(fout, mat, inf);
			break;
		case 3: if (order == NULL) {
			printf("Nema grafa!!!\n");
			break;
		    }
			criticalPath(mat, inf, order);
			break;
		case 4: if (order == NULL) {
			printf("Nema grafa!!!\n");
			break;
		    }
			printf("Operacija? ");
			getchar();
			c = getchar();
			printf("Grane na koje operacija %c utice indirektno su: ", c);
			printTransitive(mat, inf, c);
			break;
		case 5: if (order == NULL) {
			printf("Nema grafa!!!\n");
			break;
		    }
			deleteGrapf(mat, inf, &order);
			break;
		case 0: end = 0;
			printf("Kraj.\n");
			break;
		default: printf("Nepostojeci izbor!!!\n");
			break;
		}
	}
	return 0;
}