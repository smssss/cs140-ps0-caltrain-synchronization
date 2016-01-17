#include "pintos_thread.h"

struct station {
	struct lock Lock;
	struct condition trainCanGo;
	struct condition passengerCanBoard;
	int passenger_waiting;
	int passenger_boarded;
	int seats_available;
	int seats_capacity;
	int train_in_station;
};

void
station_init(struct station *station)
{
	lock_init(&station->Lock);
	cond_init(&station->trainCanGo);
	cond_init(&station->passengerCanBoard);
	station->passenger_waiting = 0;
	station->passenger_boarded = 0;
	station->train_in_station = 0;
}

void
station_load_train(struct station *station, int count)
{
	lock_acquire(&station->Lock);
	station->train_in_station = 1;
	station->seats_available = count;
	station->seats_capacity = count;
	if (station->seats_available > 0 && station->passenger_waiting > 0) {
		cond_broadcast(&station->passengerCanBoard, &station->Lock);
	}
	while (station->seats_available > 0 && station->passenger_waiting > 0) {
		cond_wait(&station->trainCanGo, &station->Lock);
	}
	station->passenger_boarded = 0;
	station->train_in_station = 0;
	lock_release(&station->Lock);
}

void
station_wait_for_train(struct station *station)
{
	lock_acquire(&station->Lock);
	station->passenger_waiting++;
	while (!station->train_in_station || (station->train_in_station && station->seats_capacity == station->passenger_boarded)) {
		cond_wait(&station->passengerCanBoard, &station->Lock);
	}
	station->passenger_waiting--;
	station->passenger_boarded++;
	lock_release(&station->Lock);
}

void
station_on_board(struct station *station)
{
	lock_acquire(&station->Lock);
	station->seats_available--;  //seat the passenger
	if (station->seats_available == station->seats_capacity - station->passenger_boarded) //all seats are taken
		cond_signal(&station->trainCanGo, &station->Lock);
	lock_release(&station->Lock);
}

