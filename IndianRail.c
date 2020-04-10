#include<stdlib.h>
#include<sys/types.h>
#include<stdio.h>
#include<math.h>
#include<unistd.h>
#include<pthread.h>

	struct station 
		{
		int no_of_external_passengers;
		int no_of_internal_passengers;
		int no_of_boarded_passengers;
		pthread_mutex_t lock;
		pthread_cond_t passengers_seated_cond;
	};
	
	
void station_init (struct station *s);
void station_load_train(struct station *s,int count);
void *station_wait_for_train(struct station *s);
void station_on_board(struct station *s);
void *thread_passenger(void *args);
void *train_thread(void *args);


void station_init(struct station *s)
		{
		s->no_of_external_passengers = 0;
		s->no_of_boarded_passengers =0;
		s->no_of_internal_passengers =0;
		pthread_mutex_init(&(s->lock),NULL);
		pthread_cond_init(&(s->passengers_seated_cond),NULL);
		}
		
		
struct train_loading 
		{
			struct station *s;
			int no_of_empty_seats;
		};
		
		
void *train_thread(void *args)
	{
	struct train_loading *obj = (struct train_loading*)args;
	station_load_train(obj->s, obj->no_of_empty_seats);
	return NULL;
	}
	

volatile int thread_completed =0;

int c;
int outvalue;


		int main()
		{
		struct station s;
		station_init(&s);
		int i;
		printf("                                  **************************************************** \n");
		printf("                                            WELCOME TO OS PROJECT OF 11813849 \n");
		printf("                                  **************************************************** \n\n\n");
		printf("ENTER THE NO OF WAITING PASSENGERS \n");
		scanf("%d",&c);
		if(c<0){
			printf("INVALID INPUT \n");
			exit(1);	
		}
		
		int no_of_total_passengers =c;
		
		for(i=0;i<c;i++)
		{
		pthread_t thread1;
		int res =pthread_create(&thread1,NULL,thread_passenger,&s);//passenger threads
		res=pthread_join(thread1,NULL);
		if(res!=0)
		{
		perror("PTHREAD_CREATE");
		exit(1);
		}
		}
		printf("\nTOTAL PASSENGERS WAITING AT THE STATION: %d\n\n",s.no_of_external_passengers);
		int j;
		LOOP :do
		{
		int f,value;
		f=10;
		struct train_loading args = { &s,  f};
		pthread_t thread2;
		int e = pthread_create(&thread2, NULL,train_thread, &args);
		if (e != 0) {
		perror("pthread_create");
		exit(1);
		        }
		    
		printf("NO OF PASSENGER TRYING TO ENTER TRAIN %d\n",s.no_of_internal_passengers);
		station_on_board(&s);
		printf("NO OF BOARDED PASSENGER %d\n",s.no_of_boarded_passengers);
		printf("TOTAL PASSENGERS UPDATED BOARDED PASSENGERS %d,%d\n",no_of_total_passengers,s.no_of_boarded_passengers);
		}
		while(no_of_total_passengers > c);
		if(s.no_of_boarded_passengers == no_of_total_passengers)
		{
		printf("ALL PASSENGERS ARE BOARDED !!\n");
		}
		else
		{
		if(s.no_of_boarded_passengers < c )
		{
		goto LOOP;
		}
		}
		}
		
		
void *thread_passenger(void *args)
{
struct station *s = (struct station*)args;
station_wait_for_train(s);
thread_completed++;

}



void *station_wait_for_train(struct station *s)
{
pthread_mutex_lock(&(s->lock));
s->no_of_external_passengers++;
pthread_mutex_unlock(&(s->lock));
}



void station_load_train(struct station *s,int count)
{
		pthread_mutex_lock(&s->lock);
		printf("%d\n",count);
		
		while(count >0 && s->no_of_external_passengers  >0)
		{
		s->no_of_internal_passengers++;
		count--;
		}
		if(count == 0)
		{	
		   printf("TRAIN IS FULL \n\n");
		}
		pthread_cond_wait(&(s->passengers_seated_cond),&(s->lock));
		
		pthread_mutex_unlock(&(s->lock));}


void station_on_board(struct station *s)
		{
		pthread_mutex_lock((&s->lock));
		while(s->no_of_internal_passengers >0 && s->no_of_boarded_passengers < c)
		{
		pthread_cond_signal((&s->passengers_seated_cond));
		
		s->no_of_internal_passengers--;
		s->no_of_boarded_passengers++;
		}
		pthread_cond_broadcast(&s->passengers_seated_cond);
		
		pthread_mutex_unlock(&s->lock);		
}

