//Korpos Botond
//kbim2251
//Lab02_Fel02

//írjunk programot, amely szálak segítségével valósítja meg egy n elemű, pozitív értékeket tartalmazó tömb
//rendezését. A tömb elemeit egy input.dat nevű állományból olvassuk.

#define DIM 10000
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

int runmode;
unsigned int finished = 1;
unsigned int n, m, add;
unsigned int array[DIM];
unsigned int result[DIM];
pthread_mutex_t mutex;

void initResult(unsigned int* result)
{
    for(int i = 0; i < DIM; ++i)
    {
        result[i] = 0; //0-val inicializalja az eredmeny tomb osszes elemet
    }
}

void userInputFromConsoleUInt(unsigned int* inn, unsigned int* inm)
{
    scanf("%d", inn);
    scanf("%d", inm); //a konzolrol beolvassuk a felhasznalo altal megadott n es m ertekeket
}

void readArrayFromFileUInt(unsigned int n, int* array, char* filename) // Beolvassa az input.dat filebol a rendezendo sorozatot
{
    FILE* fptr;

    fptr = fopen(filename, "r");
    if(fptr == NULL)
    {
        printf("Nem sikerult megnyitani az input filet!");
    }

    for(int i = 0; i < n; ++i)
    {
        fscanf(fptr, "%d", &array[i]);
    }

    fclose(fptr);

}

void* threadSort(void* arg) // Ez a fuggveny felelos a sorozat rendezeseert
{
    unsigned int intv1 = *(int*)arg * n/m;
    unsigned int intv2 = (*(int*)arg + 1) * n/m + add;

    for(int i = intv1; i < intv2; ++i)
    {
        
        unsigned int actpoz = 0;
        for(int j = 0; j < n; ++j)
        {
            if(array[i] > array[j]) // keresi hany kisebb ertek van az eppen behelyezendo szamnal
            {
                ++actpoz;
            }
        }
        result[actpoz] = array[i];
        pthread_mutex_lock(&mutex); // mutex lock
        if(runmode == 1)
        {
            printf("Thread: %d; index: %d, value: %d; new pozition: %d\n", *(int*)arg, i, array[i], actpoz);
        }
        pthread_mutex_unlock(&mutex); // mutex unlock
    }
    if(runmode == 1)
    {
        printf("Thread: %d finished %d.\n", *(int*)arg, finished);
        ++finished;    
    }
    free(arg); // az atadott argumentum memoriafelszabaditasa
}

int handleThreads() // kezeli a szalakat
{
    unsigned int mod = n % m;
    add = 1;
    pthread_t th[m];
    pthread_mutex_init(&mutex, NULL);
    for(int i = 0; i < m; ++i)
    {
        if(!mod)
        {
            add = 0;
        }
        int* itr = malloc(sizeof(int));
        *itr = i;
        if(pthread_create(&th[i], NULL, &threadSort, itr)) //letrehoz m szalat
        {
            perror("HIBA: Nem sikerult letrehozni a szalat!\n");
            return 1;
        }
        //printf("SIKER: A %d. szal letrejott!\n", i);
        if(mod)
        {
            --mod;
        }
    }

    for(int i = 0; i < m; ++i)
    {
        if(pthread_join(th[i], NULL))
        {
            perror("HIBA: Nem fejezodott be a szal futasa!\n");
            return 2;
        }
        //printf("SIKER: A %d. szal befejezte a futast!\n", i);
    }
    pthread_mutex_destroy(&mutex);

    return 0;
}

void finalCheck() // a foszal felulirja az ismetlodo elemek miatt megmaradt 0-as ertekeket
{
    for(int i = 1; i < n; ++i)
    {
        if(!result[i])
        {
            result[i] = result[i - 1];
        }
    }
}

int main(int argc, char* argv[])
{
    initResult(result);

    userInputFromConsoleUInt(&n, &m);
    readArrayFromFileUInt(n, array, "input.dat");

    runmode = argc;
    clock_t start = clock();
    if(handleThreads())
    {
        printf("HIBA!");
        return 3;
    }
    finalCheck();
    clock_t end = clock();
    double runtime = (end - start)/(double)CLOCKS_PER_SEC;

    printf("Result: ");
    for(int i = 0; i < n; ++i)
    {
        printf("%d ", result[i]);
    }
    printf("\nTIME ELAPSED: %f\n", runtime);

    return 0;
}