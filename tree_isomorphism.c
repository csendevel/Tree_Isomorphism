#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <malloc.h>
#include <math.h>
#include <string.h>

#define N 150000

typedef long long ll;
typedef unsigned long long ull;

typedef struct TVertex {
	int value;
	struct TVertex* next;
} Vert;

typedef struct TRow {
	int row_length;
	struct TVertex* first;
	struct TRow* next_row;
} Row;

typedef struct TQvalue {
	int value;
	struct TQvalue* next;
}Q;

const ull P = 239017;

int m, n;
int deg[N];
Row* graph1[N];
Row* graph2[N];
int levels[N];
int max_vert;
bool used[N];
ull hash1, hash2;

//C++ - C
//new - malloc(sizeof(type))
//delete - free()

Row* insert(Row* row, int vert) {
	Vert* t = NULL;

	if (row->first == NULL) {
		row->first = malloc(sizeof(Vert));
		row->first->value = vert;
		row->first->next = NULL;
	}
	else {
		t = row->first;
		while (t->next != NULL)
			t = t->next;

		t->next = malloc(sizeof(Vert));
		t->next->value = vert;
		t->next->next = NULL;
	}
	row->row_length++;

	return row;
}

//sort nlogn
int partition(ll* mass, int first, int last) {
	ll x = mass[last];
	int i = first - 1;
	ll tmp = 0;
	for (int j = first; j < last; j++) {
		if (mass[j] < x) {
			i++;
			tmp = mass[i];
			mass[i] = mass[j];
			mass[j] = tmp;
		}
	}
	tmp = mass[i + 1];
	mass[i + 1] = mass[last];
	mass[last] = tmp;

	return i + 1;
}

void sort(ll* mass, int first, int last) {
	if (first < last) {
		int q = partition(mass, first, last);
		sort(mass, first, q - 1);
		sort(mass, q + 1, last);
	}
}

void Tree_Print(Row* graph[], int vn) {
	Vert* ptr = NULL;
	for (int i = 1; i <= vn; i++) {
		printf("Node: %d\n", i + 1);
		ptr = graph[i]->first;
		while (ptr != NULL) {
			printf("%d ", ptr->value + 1);
			ptr = ptr->next;
		}
		printf("\n");
	}
}

ull get_hash(int vert, Row* graph[]) {
	used[vert] = true;
	int size = graph[vert]->row_length;

	long long* child_hash = malloc(size * sizeof(long long));

	int cnt = 0;
	ull hash = 0;
	ull power = 1;

	for (int i = 0; i < size; i++)
		child_hash[i] = 0;

	Vert* ptr = graph[vert]->first;

	while (ptr != NULL) {
		if (used[ptr->value] == false) {
			child_hash[cnt++] = get_hash(ptr->value, graph);
		}
		ptr = ptr->next;
	}

	sort(child_hash, 0, cnt - 1);

	for (int i = 0; i < cnt; i++) {
		hash += ((child_hash[i]) * (child_hash[i]) + (child_hash[i]) * power + 3);
		power = (power * P);
	}

	free(child_hash);
	child_hash = NULL;

	return hash;
}

int dfs(int v, int depth, Row* graph[]) {
	used[v] = true;
	int tmp = depth;
	Vert* ptr = graph[v]->first;

	while (ptr != NULL) {
		if (used[ptr->value] == false) {
			int t = dfs(ptr->value, depth + 1, graph);
			if (t > tmp) {
				tmp = t;
			}
		}
		ptr = ptr->next;
	}

	return tmp;
}

int bfs(int v, Row* graph[]) {

	int cnt = 0, new_cnt = 0, qvalue_cnt = 0;
	int last_vert = v;

	Q* first = malloc(sizeof(Q));
	Q* last = first;
	first->value = v;
	first->next = NULL;
	qvalue_cnt = cnt = 1;
	levels[v] = 1;
	Vert* ptr = NULL;

	while (qvalue_cnt > 0) {

		int vert = first->value;
		used[vert] = true;
		ptr = graph[vert]->first;

		last_vert = vert;

		while (ptr != NULL) {
			if (used[ptr->value] == false) {
				Q* tmp = malloc(sizeof(Q));
				qvalue_cnt++;
				levels[ptr->value] = levels[vert] + 1;
				last->next = tmp;
				tmp->value = ptr->value;
				last = tmp;
			}
			ptr = ptr->next;
		}

		qvalue_cnt--;
		Q* tmp = first->next;
		first->next = NULL;
		free(first);
		first = tmp;
	}

	return last_vert;
}

int* longest_path(Row* graph[]) {
	for (int i = 0; i <= n; i++) used[i] = 0;
	int fvert = bfs(1, graph);
	for (int i = 0; i <= n; i++) used[i] = 0;
	for (int i = 0; i <= n; i++) levels[i] = 0;
	int svert = bfs(fvert, graph);
	for (int i = 0; i <= n; i++) used[i] = 0;

	int* path = malloc(sizeof(int) * (n + 1));

	path[0] = levels[svert];
	int vert = svert;
	Vert* ptr = NULL;
	path[levels[fvert]] = fvert;
	while (vert != fvert) {
		path[levels[vert]] = vert;
		ptr = graph[vert]->first;
		while (ptr != NULL) {
			if (levels[ptr->value] < levels[vert]) {
				vert = ptr->value;
				break;
			}
			ptr = ptr->next;
		}
	}

	return path;
}

//lex sort O(nlogn)
static int myCompare(const void* a, const void* b) {
	return strcmp(*(const char**)a, *(const char**)b);
}

void lx_sort(const char* arr[], int n) {
	qsort(arr, n, sizeof(const char*), myCompare);
}

int compareStrings(const char* s1, const char* s2) {
	size_t i;

	for (i = 0; s1[i] == s2[i]; i++) {
		if (s1[i] == '\0')
			return 0;
	}
	if ((unsigned char)s1[i] < (unsigned char)s2[i])
		return -1;
	else
		return 1;
}

//lex sort O(n)
char** lex_sort(char** mass, int num) {
	int maxl = 0, tmp = 0;

	for (int i = 0; i < num; i++) {
		tmp = strlen(mass[i]);
		if (tmp > maxl) maxl = tmp;
	}

	int* len_cnt = (int*)malloc(sizeof(int) * (maxl + 1)); // create len_cnt

	for (int i = 0; i <= maxl; i++) {
		len_cnt[i] = 0;
	}

	// create length
	char*** length = (char***)malloc(sizeof(char**) * (maxl + 1));

	for (int i = 0; i <= maxl; i++) {
		length[i] = malloc(sizeof(char*) * (num + 1));
	}

	int j = 0;

	for (int i = 0; i < num; i++) {
		tmp = strlen(mass[i]);

		j = len_cnt[tmp];

		length[tmp][j] = mass[i];

		len_cnt[tmp] = j + 1;
	}

	int tmp_ptr = 1, q_ptr = 1, b1_ptr = 0, b2_ptr = 0;

	//
	char** queue = (char**)malloc(sizeof(char*) * (num + 1));
	char** bucket1 = (char**)malloc(sizeof(char*) * num);
	char** bucket2 = (char**)malloc(sizeof(char*) * num);

	for (int l = maxl; l > 0; l--) {
		tmp_ptr = 1;

		if (len_cnt[l] == 0) continue;

		for (int k = 0; k < len_cnt[l]; k++) {
			queue[tmp_ptr] = length[l][k]; tmp_ptr++;
		}

		if (tmp_ptr > q_ptr) q_ptr = tmp_ptr;

		tmp_ptr = 1;
		q_ptr--;

		while (q_ptr > 0) {
			char tempo = queue[tmp_ptr][l - 1];

			if (tempo == '0') {
				bucket1[b1_ptr] = queue[tmp_ptr];
				b1_ptr++;
			}
			else {
				bucket2[b2_ptr] = queue[tmp_ptr];
				b2_ptr++;
			}
			q_ptr--; tmp_ptr++;
		}

		int next_no_empty = l - 1;
		while (len_cnt[next_no_empty] == 0 && next_no_empty > 0) next_no_empty--;

		for (int i = 1; i <= len_cnt[next_no_empty]; i++)
			queue[i] = "0";

		q_ptr = len_cnt[next_no_empty]; q_ptr++;

		for (int i = 0; i < b1_ptr; i++) {
			queue[q_ptr] = bucket1[i];
			q_ptr++;
		}

		b1_ptr = 0;

		for (int i = 0; i < b2_ptr; i++) {
			queue[q_ptr] = bucket2[i];
			q_ptr++;
		}

		b2_ptr = 0;
	}

	char** ans = (char**)malloc(sizeof(char*) * num);

	for (int i = 0; i < num; i++) {
		ans[i] = (char*)malloc(sizeof(char) * strlen(queue[i + 1]));
		strcpy(ans[i], queue[i + 1]);
	}

	free(len_cnt); // delete len_cnt

	free(bucket1); free(bucket2);
	free(queue);
	for (int i = 0; i <= maxl; i++) free(length[i]);
	free(length);

	return ans;
}

char* Assign_Canonical_Names(int v, Row* graph[]) {
	used[v] = true;

	int cnt = 0, len = 0;
	Vert* ptr = graph[v]->first;

	char** names = (char**)malloc(sizeof(char*) * (deg[v] + 1));

	while (ptr != NULL) {
		if (used[ptr->value] == false) {
			char* ttt = Assign_Canonical_Names(ptr->value, graph);

			char* tmp = (char*)malloc(sizeof(char) * (strlen(ttt) + 1));
			strcpy(tmp, ttt);

			free(ttt);
			len += strlen(tmp);
			
			names[cnt] = (char*)malloc((strlen(tmp) + 1) * sizeof(char));
			strcpy(names[cnt], tmp);
			
			free(tmp);
			cnt++;
		}
		ptr = ptr->next;
	}
	
	char* new_name = (char*)malloc(sizeof(char) * (len + 10)); //!!!!!!!

	strcpy(new_name, "1");

	char** sort_names = lex_sort(names, cnt);
	
	for (int i = 0; i < cnt; i++) free(names[i]);
	free(names);

	for (int i = 0; i < cnt; i++)
		strcat(new_name, sort_names[i]);

	strcat(new_name, "0");

	free(sort_names);

	return new_name;
}

int** load_tree() {
	FILE* load_file_stream;
	load_file_stream = fopen("tree1.txt", "r");
	int c = 0;
	int m;
	fscanf(load_file_stream, "%d", &n);

	int** trees = (int**)malloc(4 * sizeof(int*));
	for (int i = 0; i < 4; i++) trees[i] = malloc(n * sizeof(int));

	for (int i = 0; i < 2 * n - 2; i++)
		fscanf(load_file_stream, "%d", &trees[i % 2][i / 2]);

	for (int i = 0; i < 2 * n - 2; i++)
		fscanf(load_file_stream, "%d", &trees[(i % 2) + 2][i / 2]);

	fclose(load_file_stream);

	return trees;
}

int main() {
	int** data = load_tree();

	int root = 0;

	for (int i = 1; i <= n; i++) {
		graph1[i] = malloc(sizeof(Row));
		graph1[i]->first = NULL;
		graph1[i]->row_length = 0;

		graph2[i] = malloc(sizeof(Row));
		graph2[i]->first = NULL;
		graph2[i]->row_length = 0;
	}

	for (int i = 1; i < n; i++) {
		insert(graph1[data[1][i - 1]], data[0][i - 1]);
		insert(graph1[data[0][i - 1]], data[1][i - 1]);
		deg[data[0][i - 1]]++;
		deg[data[1][i - 1]]++;
	}

	int* tmp = longest_path(graph1);

	root = tmp[0];
	root = tmp[(root + 1) / 2];
	free(tmp);

	for (int i = 0; i <= n; i++) used[i] = 0;

	char* name1 = Assign_Canonical_Names(root, graph1);

	for (int i = 0; i <= n; i++) used[i] = 0;
	for (int i = 0; i <= n; i++) deg[i] = 0;

	for (int i = 1; i < n; i++) {
		insert(graph2[data[2][i - 1]], data[3][i - 1]);
		insert(graph2[data[3][i - 1]], data[2][i - 1]);
		deg[data[2][i - 1]]++;
		deg[data[3][i - 1]]++;
	}

	tmp = longest_path(graph2);

	root = tmp[0];
	root = tmp[(root + 1) / 2];
	free(tmp);

	for (int i = 0; i <= n; i++) used[i] = 0;

	char* name2 = Assign_Canonical_Names(root, graph2);
	//printf("%s %s\n", name1, name2);
	if (strcmp(name1, name2) == 0) printf("Isomorphic\n");
	else printf("Not isomorphic\n");

	for (int i = 0; i < 4; i++)
		free(data[i]);
	free(data);

	//more tests
	/*for (int i = 0; i <= n; i++) used[i] = 0;
	hash2 = get_hash(root, graph2);
	for (int i = 0; i <= n; i++) used[i] = 0;
	hash1 = get_hash(root, graph1);

	printf("%llu\n", hash1);
	printf("%llu\n", hash2);
	printf("%d\n", dfs(root, 0, graph2));
	printf("%d\n", bfs(3, graph1));

	int** tmp = longest_path(graph1);

	for (int i = 0; i <= path[0]; i++)
		printf("%d ", tmp[i]);

	root = tmp[0];
	root = tmp[(root + 1) / 2];
	printf("\n%d", root);*/

	//testing lex_sort - passed
	/*char* test[] = {"111", "111", "11"};

	char** t = lex_sort(test, 3);

	for (int i = 0; i < 3; i++) printf("%s ", *(t+i));
	printf("\n");

	char* test2[] = { "111", "111", "11" };

	lx_sort(test2, 3);

	for (int i = 0; i < 3; i++) printf("%s ", test2[i]);*/
}
