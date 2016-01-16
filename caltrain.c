#include "pintos_thread.h"

struct station {
	struct lock Lock;
	struct condition trainCanGo;
	struct condition passengerCanBoard;
	int passenger_waiting;
	int seats_available;
	int train_instation;
};

void
station_init(struct station *station)
{
	lock_init(&station->Lock);
	cond_init(&station->trainCanGo);
	cond_init(&station->passengerCanBoard);
	station->passenger_waiting = 0;
	station->train_instation = 1;
}

void
station_load_train(struct station *station, int count)
{
	lock_acquire(&Lock);
	station->train_instation = 1;
	station->seats_available = count;
	if (station->seats_available > 0 && station->passenger_waiting > 0) 
		cond_boradcast(&passengerCanBoard, &Lock);
	while (station->seats_available != 0 && station->passenger_waiting != 0) {
		cond_wait(&trainCanGo, &Lock);
	}
	station->train_instation = 0;
	lock_release(&Lock);
}

void
station_wait_for_train(struct station *station)
{
	lock_acquire(&Lock);
	station->passenger_waiting++;
	while ( (!station->train_instation) || (station->train_instation && station->seats_available == 0)) {
		cond_wait(&passengerCanBoard, &Lock);
	}
	station->passenger_waiting--;
	station->seats_available--;
	lock_release(&Lock);
}

void
station_on_board(struct station *station)
{
	lock_acquire(&Lock);
	if (station->seats_available == 0 || station->passenger_waiting == 0)
		cond_signal(&trainCanGo, &Lock);
	lock_release(&Lock);
}

