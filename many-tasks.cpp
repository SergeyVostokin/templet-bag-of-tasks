/*$TET$actor*/

#define EVEREST_EXECUTION
#include "./lib/templet.hpp"

using namespace TEMPLET;
using namespace std;

struct btask:task {
	btask(taskengine&e):task(e, "5ccaba85100000638af4eabe"){}
};

/*$TET$*/

using namespace TEMPLET;

struct my_engine : engine{
	my_engine(int argc, char *argv[]){
		::init(this, argc, argv);
	}
	bool run(){ return TEMPLET::run(this); }
	void map(){ TEMPLET::map(this); }
};

#pragma templet ~msg=

struct msg : message{
	msg(actor*a, engine*e, int t) : _where(CLI), _cli(a), _client_id(t){
		::init(this, a, e);
	}

	void send(){
		if (_where == CLI){ TEMPLET::send(this, _srv, _server_id); _where = SRV; }
		else if (_where == SRV){ TEMPLET::send(this, _cli, _client_id); _where = CLI; }
	}

/*$TET$msg$$data*/
/*$TET$*/

	enum { CLI, SRV } _where;
	actor* _srv;
	actor* _cli;
	int _client_id;
	int _server_id;
};

#pragma templet *worker(m!msg,t.btask)+

struct worker : actor{
	enum tag{START,TAG_m,TAG_t};

	worker(my_engine&e):m(this, &e, TAG_m),t(*(e._teng)){
		TEMPLET::init(this, &e, worker_recv_adapter);
		TEMPLET::init(&_start, this, &e);
		TEMPLET::send(&_start, this, START);
		t.set_on_ready([&]() { t_handler(t); resume(); });
/*$TET$worker$worker*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	msg m;
	btask t;
	void t_submit() { t.submit(); suspend(); };

	static void worker_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_m: ((worker*)a)->m_handler(*((msg*)m)); break;
			case START: ((worker*)a)->start(); break;
		}
	}

	void start(){
/*$TET$worker$start*/
/*$TET$*/
	}

	void m_handler(msg&m){
/*$TET$worker$m*/
/*$TET$*/
	}

	void t_handler(btask&t){
/*$TET$worker$t*/
/*$TET$*/
	}

/*$TET$worker$$code&data*/
/*$TET$*/
	message _start;
};

#pragma templet *master(m?msg)

struct master : actor{
	enum tag{START,TAG_m};

	master(my_engine&e){
		TEMPLET::init(this, &e, master_recv_adapter);
/*$TET$master$master*/
/*$TET$*/
	}

	bool access(message*m){ return TEMPLET::access(m, this); }
	bool access(message&m){ return TEMPLET::access(&m, this); }

	void at(int _at){ TEMPLET::at(this, _at); }
	void delay(double t){ TEMPLET::delay(this, t); }
	double time(){ return TEMPLET::time(this); }
	void stop(){ TEMPLET::stop(this); }

	void m(msg&m){m._server_id=TAG_m; m._srv=this;}

	static void master_recv_adapter (actor*a, message*m, int tag){
		switch(tag){
			case TAG_m: ((master*)a)->m_handler(*((msg*)m)); break;
		}
	}

	void m_handler(msg&m){
/*$TET$master$m*/
/*$TET$*/
	}

/*$TET$master$$code&data*/
/*$TET$*/
};

/*$TET$code&data*/
/*$TET$*/

int main(int argc, char *argv[])
{
	my_engine e(argc, argv);
/*$TET$footer*/
/*$TET$*/
}
