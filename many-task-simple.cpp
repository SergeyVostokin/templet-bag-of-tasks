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
    
    void on_worker_ready(){
        // put the result
        /**/arr[pos_in_arr] = get_squared_number();/**/
        idle_worker_list.push_back(this);
        run_tasks();
    }
    
    static void run_tasks(){
        // run tasks while having tasks and free workers
        while(!idle_worker_list.empty() && /**/cur < NUM_TASKS/**/){
            worker*w = idle_worker_list.front();
            idle_worker_list.pop_front();

            /**/w->put_number_and_pos(arr[cur],cur);cur++;/**/

            w->submit([=](){w->on_worker_ready();});
        }
    }
    static list<worker*> idle_worker_list;
};

list<worker*> worker::idle_worker_list;
/**/



int main(int argc, char *argv[])
{
    taskengine eng("access_token");

    worker worker_arr[NUM_WORKERS];
    
    for(int i=0;i<NUM_WORKERS;i++){
        worker_arr[i].set_app_id("5e18b9903100006776804a1e");
		worker_arr[i].set_engine(eng);
		worker::idle_worker_list.push_back(&worker_arr[i]);
    }
	
	/**/
	for(int i=0;i<NUM_TASKS;i++){ arr[i] = i; cout << "arr[" << i << "]=" << arr[i] << endl;}
    cur = 0;
	/**/

    double time = omp_get_wtime();
	worker::run_tasks(); // submitting the first portion of tasks
    eng.wait_all();
    time = omp_get_wtime() - time;
	
	/**/
    cout << "----------\n";
	for(int i=0;i<NUM_TASKS;i++)  cout << "arr[" << i << "]=" << arr[i] << endl;;
	/**/
    
    cout << "speedup is:  " << NUM_TASKS*(TASK_TIME/1000.0)/ time << endl;
	cout << "NUM_TASKS is: " << NUM_TASKS << endl;
	cout << "TASK_TIME is: " << TASK_TIME/1000.0 << " sec" << endl;
	cout << "NUM_WORKERS is: " << NUM_WORKERS << endl;
}