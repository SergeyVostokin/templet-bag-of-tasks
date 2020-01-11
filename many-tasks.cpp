/*$TET$actor*/

#define EVEREST_EXECUTION
#include "./lib/templet.hpp"

using namespace TEMPLET;
using namespace std;

struct btask:task {
	btask(taskengine&e):task(e, ""){}
};

const int NUM_WORKERS = 5;

/*$TET$*/

using namespace TEMPLET;

struct my_engine : engine{
	my_engine(int argc, char *argv[]){
		::init(this, argc, argv);
	}
	bool run(){ return TEMPLET::run(this); }
	void map(){ TEMPLET::map(this); }
};

#pragma templet ~request=

struct request : message{
	request(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, a, e);
	}

	void send(){
		if (_where == CLI){ TEMPLET::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ TEMPLET::send(this, _cli, _client_id); _where = CLI; }
	}

/*$TET$request$$data*/
    int    index;
    double num;
    bool is_first;
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet *master(port?request)

struct master : actor{
	enum tag{START,TAG_port};

	master(my_engine&e){
		TEMPLET::init(this, &e, master_recv_adapter);
/*$TET$master$master*/
      for(int i=0;i<10;i++) arr[i] = i;
      cur = 0;
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void port(request&m){m._server_id=TAG_port; m._srv=this;}

	static void master_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_port: ((master*)a)->port_handler(*((request*)m)); break;
		}
	}

	void port_handler(request&m){
/*$TET$master$port*/
         /*1*/
         if(m.is_first) m.is_first = false;
         else           arr[m.index] = m.num;
         
         req_list.push_back(&m);
         
         /*2*/
         while(!req_list.empty() && cur < 10){
            request* r = req_list.front();
            req_list.pop_front();
            
            r->index = cur;
            r->num = arr[cur];
            cur++;
            
            r->send();
         }
         
         /*3*/
         if(req_list.size() == NUM_WORKERS){
             for(int i=0;i<10;i++) cout << arr[i] << endl;
             stop();
         }
/*$TET$*/
	}

/*$TET$master$$code&data*/
     list<request*> req_list;
     double arr[10];
     int cur;
/*$TET$*/
};

#pragma templet *worker(port!request)+

struct worker : actor{
	enum tag{START,TAG_port};

	worker(my_engine&e):port(this, &e, TAG_port){
		TEMPLET::init(this, &e, worker_recv_adapter);
		TEMPLET::init(&_start, this, &e);
		TEMPLET::send(&_start, this, START);
/*$TET$worker$worker*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	request port;

	static void worker_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_port: ((worker*)a)->port_handler(*((request*)m)); break;
			case START: ((worker*)a)->start(); break;
		}
	}

	void start(){
/*$TET$worker$start*/
       port.is_first = true;
       port.send();
/*$TET$*/
	}

	void port_handler(request&m){
/*$TET$worker$port*/
        m.num = m.num * m.num;
        m.send();
/*$TET$*/
	}

/*$TET$worker$$code&data*/
/*$TET$*/
	message _start;
};

/*$TET$code&data*/
/*$TET$*/

int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
/*$TET$footer*/
    master a_master(e);
    
    worker** workers = new worker*[NUM_WORKERS];
    for(int i;i<NUM_WORKERS;i++){
        workers[i] = new worker(e);
        a_master.port(workers[i]->port);
    }

    e.run();
/*$TET$*/
}
