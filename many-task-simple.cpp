#include "./lib/everest.hpp"
#include <list>
#include <omp.h>

using namespace TEMPLET;
using namespace std;

const int NUM_WORKERS = 5;

/**/  // this marks problem specific code
const int TASK_TIME = 10 * 1000; // ms
const int NUM_TASKS = 10;
double arr[NUM_TASKS];
int cur;

struct worker:task {
	int number;
	int pos_in_arr;
	
	worker():task(){}
    
    void put_number_and_pos(int n, int pos){
		number = n;
		pos_in_arr = pos;
		
        json in;
		in["name"] = "squared-number-task";
		in["inputs"]["n"] = n;
		in["inputs"]["time"] = TASK_TIME;
		input(in);
    }
    
    int get_squared_number(){
        int n=0;
        json out=result();
        n = out["squared-n"];
        return n;
    }
};
/**/

worker worker_arr[NUM_WORKERS];
list<worker*> idle_worker_list;

void on_worker_ready(worker*w)
{
	// put the result, if any
	if(w){
       /**/arr[w->pos_in_arr] = get_squared_number();/**/
	   idle_worker_list.push_back(w);
	}          
	 
	// run tasks while having tasks and free workers
	while(!idle_worker_list.empty() && /**/cur < NUM_TASKS/**/){
		w = idle_worker_list.front();
		idle_worker_list.pop_front();
		
		/**/w->put_number_and_pos(arr[cur],cur++);/**/
		
		w->submit([=](){on_worker_ready(w)});
	}
}

int main(int argc, char *argv[])
{
    taskengine eng("access_token");

    for(int i;i<NUM_WORKERS;i++){
        worker[i].set_app_id("application_id");
		worker[i].set_engine(eng);
		idle_worker_list.push_back(&worker[i]);
    }
	
	/**/
	for(int i=0;i<NUM_TASKS;i++) arr[i] = i;
    cur = 0;
	/**/

    double time = omp_get_wtime()
	on_worker_ready(0); // submitting the first portion of tasks
    eng.wait_all();
    time = omp_get_wtime() - time;
	
	/**/
	for(int i=0;i<NUM_TASKS;i++) cout << arr[i] << endl;
	/**/
    
    cout << "speedup is:  " << NUM_TASKS*(TASK_TIME/1000.0)/ time << endl;
	cout << "NUM_TASKS is: " << NUM_TASKS << endl;
	cout << "TASK_TIME is: " << TASK_TIME/1000.0 << " sec" << endl;
	cout << "NUM_WORKERS is: " << NUM_WORKERS << endl;
}