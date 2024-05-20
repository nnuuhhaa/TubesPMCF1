/* EL2208 Praktikum Pemecahan Masalah dengan C 2023/2024
* Modul            : Tubes - Travelling Salesmen Problem 
* Hari dan Tanggal : Senin 20 Mei 2024
* Nama (NIM)       : Nuha Muhammad Yahya (18321004)
* Asisten (NIM)    : Emmanuella Pramudita Rumanti (13220031)
* Nama File        : main.c
* Deskripsi        : Penyelesaian Travelling Salesman Problem menggunakan algoritma BFS untuk mencari rute terbaik
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <time.h>

#define N 6  // Program ini mengasumsikan jumlah kota tepat dengan nilai N yang didefinisikan disini. Nilai ini bisa diubah sesuai kebutuhan namun perlu didefinisikan terlebih dahulu.
#define R 6371
#define M_PI 3.14159265358979323846

// Kumpulan fungsi untuk operasi algoritma BFS
typedef struct {
    int currentCity;
    int path[N + 1];
    int pathLength;
    double cost;
} State;

typedef struct {
    State *states;
    int front, rear, size;
    unsigned capacity;
} Queue;

Queue* createQueue(unsigned capacity) {
    Queue* queue = (Queue*) malloc(sizeof(Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;
    queue->states = (State*) malloc(queue->capacity * sizeof(State));
    return queue;
}

int isFull(Queue* queue) {
    return (queue->size == queue->capacity);
}

int isEmpty(Queue* queue) {
    return (queue->size == 0);
}

void enqueue(Queue* queue, State state) {
    if (isFull(queue))
        return;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->states[queue->rear] = state;
    queue->size = queue->size + 1;
}

State dequeue(Queue* queue) {
    if (isEmpty(queue)) {
        State emptyState;
        emptyState.pathLength = -1;
        return emptyState;
    }
    State state = queue->states[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size = queue->size - 1;
    return state;
}

void tspBFS(double graph[N][N], int startCity, char cities[N][100]) {
    // Inisiasi queue dan state
    Queue* queue = createQueue(1000);
    State startState = {startCity, {startCity}, 1, 0.0};
    enqueue(queue, startState);

    // Inisiasi variabel jarak terpendek
    double minCost = INT_MAX;
    int bestPath[N + 1];

    while (!isEmpty(queue)) {
        State currentState = dequeue(queue);

        if (currentState.pathLength == N) { // Pembandingan rute terpendek ketika rute sudah melewati semua kota
            double returnCost = graph[currentState.currentCity][startCity];
            if (currentState.cost + returnCost < minCost) {
                minCost = currentState.cost + returnCost;
                for (int i = 0; i < N; i++) {
                    bestPath[i] = currentState.path[i];
                }
                bestPath[N] = startCity;
            }
            continue;
        }

        for (int nextCity = 0; nextCity < N; nextCity++) { // Pencarian kota yang belum dikunjungi pada rute
            int alreadyVisited = 0;
            for (int i = 0; i < currentState.pathLength; i++) {
                if (currentState.path[i] == nextCity) {
                    alreadyVisited = 1;
                    break;
                }
            }
            if (!alreadyVisited) { // Penambahan rute baru dengan kota baru yang akan dikkunjungi pada queue
                State nextState = currentState;
                nextState.currentCity = nextCity;
                nextState.path[nextState.pathLength] = nextCity;
                nextState.pathLength++;
                nextState.cost += graph[currentState.currentCity][nextCity];
                enqueue(queue, nextState);
            }
        }
    }

    char bestPathCity[N][100];

    for(int i = 0; i <= N; i++) // Mentranslate urutan rute dari integer menjadi string nama kota
    {
        strcpy(bestPathCity[i],cities[bestPath[i]]);
    }

    printf("Best route found:\n"); // Pencetakan rute
    for (int i = 0; i <= N; i++) {
        printf("%s", bestPathCity[i]);
        if(i < N)
        {
            printf(" -> ");
        }
    }
    printf("\nBest route distance: %f\n", minCost); // Pencetakan jarak

    free(queue->states); // Pembebasan memori
    free(queue);
}

// Fungsi untuk membaca dan mengekstrak data CSV ke matriks untuk diolah
int readCSV(const char *filename, double matrix[3][N], char cities[N][100], int *numCities) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Unable to open file");
        return -1;
    }

    char line[100];
    *numCities = 0;

    // Iterasi while loop pembacaan file eksternal baris demi baris
    while (fgets(line, sizeof(line), file)) {
        char *token;
        double latitude, longitude;

        // Tokenisasi nama kota
        token = strtok(line, ",");
        if (token == NULL) continue;
        strcpy(cities[*numCities], token);

        // Tokenisasi latitude
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        latitude = atof(token);

        // Tokenisasi longitude
        token = strtok(NULL, ",");
        if (token == NULL) continue;
        longitude = atof(token);

        // Penyimpanan data latitude dan longitude pada matriks
        matrix[0][*numCities] = (double)(*numCities); // Indeks
        matrix[1][*numCities] = latitude;
        matrix[2][*numCities] = longitude;

        (*numCities)++;
        if (*numCities >= N) {
            printf("Reached maximum number of cities (%d). Some data may not be read.\n", N);
            break;
        }
    }

    fclose(file);
    return 0;
}

// Fungsi untuk menghitung jarak antara dua kota dari garis bujur dan lintangnya
double haversine(double lat1, double lon1, double lat2, double lon2) {
    double dLat = (lat2 - lat1) * M_PI / 180.0;
    double dLon = (lon2 - lon1) * M_PI / 180.0;

    lat1 = lat1 * M_PI / 180.0;
    lat2 = lat2 * M_PI / 180.0;

    double a = sin(dLat/2) * sin(dLat/2) + cos(lat1) * cos(lat2) * sin(dLon/2) * sin(dLon/2);
    double c = 2 * atan2(sqrt(a), sqrt(1-a));

    return R * c;
}

// Fungsi untuk memetakan jarak antara setiap kota ke kota lainnya ke sebuah grafik
void createGraph(double matrix[3][N], int numCities, double graph[N][N]) {
    for (int i = 0; i < numCities; i++) {
        for (int j = 0; j < numCities; j++) {
            if (i == j) {
                graph[i][j] = 0.0;
            } else {
                double lat1 = matrix[1][i];
                double lon1 = matrix[2][i];
                double lat2 = matrix[1][j];
                double lon2 = matrix[2][j];
                graph[i][j] = haversine(lat1, lon1, lat2, lon2);
            }
        }
    }
}

// Program utama
int main() {
    double matrix[3][N];
    char cities[N][100];
    int numCities;
    double graph[N][N];
    char file[100];

    printf("Enter list of cities file name: ");
    scanf("%s", &file);

    if (readCSV(file, matrix, cities, &numCities) == 0) {
        createGraph(matrix, numCities, graph);

        // Pencetekan daftar kota
        printf("List of cities:\n");
        for (int i = 0; i < numCities; i++) {
            printf("%d. %s\n", i+1, cities[i]);
        }

        // Program meminta kota awal
        char startCityName[100];
        printf("Enter the starting city name: ");
        scanf("%s", startCityName);

        clock_t start_t, end_t;
        double t;
        start_t =clock(); // Inisiasi untuk timer

        // Pencarian indeks dari kota awal yang diinginkan pada matriks daftar kota
        int startCity = -1;
        for (int i = 0; i < numCities; i++) {
            if (strcmp(startCityName, cities[i]) == 0) {
                startCity = i;
                break;
            }
        }

        // Algoritma BFS akan digunakan dengan input grafik jarak untuk menghitung
        if (startCity != -1) {
            tspBFS(graph, startCity, cities);

        end_t = clock(); // timer end
        t = ((double)(end_t - start_t))/ CLOCKS_PER_SEC; // Waktu yang dibutuhkan untuk program dijalankan

        printf("Time elapsed: %f",t);
        } else { // Output error jika nama kota tidak sesuai dengan yang ada pada daftar kota
            printf("Invalid city name. Please enter a valid city from the list.\n");
        }
    } else { // Output error jika nama file tidak ditemukan / tidak sesuai
        printf("Error reading the CSV file.\n");
    }
    return 0;
}
