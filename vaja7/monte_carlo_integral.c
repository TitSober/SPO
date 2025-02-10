#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>

#define MAX_TRIES 1e9

int counter;
int tries;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int countSinIntegral(){
    struct timespec myTime;
    double x,y;
    clock_gettime(CLOCK_REALTIME,&myTime); // program prevedi s stikalom -lrt
    unsigned int seed = myTime.tv_nsec;
    while(1){
        int localCounter = 0;
        int ltries = 1e6;
        for(int i = 0; i < ltries; i++){

            x = (((double)rand_r(&seed)) / RAND_MAX) * M_PI; 
            y = ((double)rand_r(&seed)) / RAND_MAX;
            if(y < sin(1.0*x)){
                localCounter++;
                //printf("%f %f\n",x,y);
            }
            
    
        }
        pthread_mutex_lock(&mutex);
        counter += localCounter;
        tries += ltries;
        if(tries > MAX_TRIES){
            pthread_mutex_unlock(&mutex); 
            return 0;
        }
        pthread_mutex_unlock(&mutex); 
        
        }
    
}
/*void *threadCheckerP(void *threads){
    pthread_t *thr = (pthread_t *)threads;
    while(1){
        pthread_mutex_lock(&mutex);
        if(tries > 1000000000){
            printf("stop\n");   
            for(int i = 0; i < sizeof(thr)/sizeof(pthread_t); i++){
                pthread_cancel(thr[i]);
                printf("tukaj\n");
            }
            for(int i = 0; i < sizeof(thr)/sizeof(pthread_t); i++){
                pthread_join(thr[i],NULL);
            }
            break;
        }
        pthread_mutex_unlock(&mutex);
        usleep(10000);
    }
    return NULL;
}*/
int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Uporaba: %s število niti\n", argv[0]);
        return -1;
    }
    counter = 0;
    tries = 0;
    int niti = atoi(argv[1]); // št. niti
    pthread_t threads[niti];
   
    time_t st = time(NULL);
    for(int i = 0; i < niti; i++){
        pthread_create(&threads[i],NULL,(void *)countSinIntegral,NULL);
    }
    //pthread_create(&threadChecker,NULL,(void *)threadCheckerP,threads);
    for(int i = 0; i < niti; i++){
        pthread_join(threads[i],NULL);
    }
    time_t en = time(NULL) -st;
    printf("Count = %d\n", counter);
    printf("Tries = %d\n", tries);
    printf("Procent  = %f\n", (double)counter/tries);

    double factor = (double)counter/tries;
    double fullarea = M_PI * 1.0;
    double result = factor * fullarea;
    printf("Integral sin(x) = %f\n", result);
    printf("Time = %d\n", en);

    return 0;
    
}
