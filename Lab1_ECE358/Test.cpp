
#include "pch.h"
#include <iostream>
#include <random>
#include <math.h>
#include <queue>   
#include <iomanip>

struct Packet {
	int ticks_until_departure;
	int arrival_time;
};

int arrival_time;

double average_idle;
double average_loss;
double average_en;
double average_et;

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0, 1.0);

// int for now, change to packet struct later
std::queue<Packet*> router_queue;

int idle_counter = 0;
double num_of_packets_in_queue_total = 0;
double total_num_packets_generated = 0;
double total_sojourn_time = 0;
double loss_counter = 0;

// Parameters
int TICKS = 100000 * 1000;
int TICK_SIZE = 1 / 0.000001;
int queue_size = -1;
double L_packet_size = 12000;
double C_transmission_rate = 1000000;

// According to p = lambda* L / C
double lambda = 0;

int calculate_arrival_time() {
	double U = distribution(generator);

	// Arrival time in seconds
	double arrival_time = (-1 / lambda) * log(1 - U);

	// Currently assuming 1 tick = 100 millisecond (100 * 10^-3)
	return arrival_time * TICK_SIZE;
}

void arrival(int current_time) {
	if (current_time >= arrival_time) {
		if (queue_size == -1 || (queue_size > 0 && router_queue.size() < queue_size)) {
			Packet *new_packet = new Packet();
			new_packet->ticks_until_departure = (L_packet_size / C_transmission_rate * TICK_SIZE);
			new_packet->arrival_time = current_time;

			router_queue.push(new_packet);
			total_num_packets_generated += 1;
		}
		else {
			loss_counter += 1;
		}

		arrival_time = current_time + calculate_arrival_time();
	}
}

void server(int current_time) {
	if (router_queue.size() > 0) {
		if (router_queue.front()->ticks_until_departure == 1) {
			total_sojourn_time += (current_time - router_queue.front()->arrival_time);
			router_queue.pop();
		}
		else {
			router_queue.front()->ticks_until_departure -= 1;
		}
	}
	else {
		idle_counter += 1;
	}
}


void experiment() {
	// Calculating first arrival time
	arrival_time = calculate_arrival_time();

	for (int i = 0; i < TICKS; i++) {
		arrival(i);
		server(i);

		num_of_packets_in_queue_total += router_queue.size();
	}
}

int main() {
	int queue_sizes[4] = { -1, 5, 10, 40 };
	std::cout << "Queue Size, Row, Loss Counter, Idle Counter, E[N], E[T]" << std::endl;
	for (int k = 0; k < 4; k++) {
		for (double r0 = 0.35; r0 <= 0.85; r0 += 0.1) {
			for (int i = 0; i <= 0; i++) {
				int router_size = router_queue.size();
				for (int j = 0; j < router_size; j++) {
					router_queue.pop();
				}

				total_num_packets_generated = 0;
				total_sojourn_time = 0;
				idle_counter = 0;
				num_of_packets_in_queue_total = 0;
				loss_counter = 0;

				// Parameters
				queue_size = queue_sizes[k];
				TICKS = 5000000;
				TICK_SIZE = 1 / 0.000001;
				L_packet_size = 12000;
				C_transmission_rate = 1000000;

				lambda = r0 * C_transmission_rate / L_packet_size;

				experiment();

				average_idle = (double)idle_counter/ (double)TICKS;
				average_loss = (double)loss_counter / (double)total_num_packets_generated;
				average_en = (double)num_of_packets_in_queue_total / (double) TICKS;
				average_et = (double)total_sojourn_time/ (double) TICKS;
			}

			std::cout << "CurrentLambda: " << lambda << ", " << queue_sizes[k] << ", " << r0 << ", " << average_loss<< ", " << average_idle << ", " << average_en << ", " << average_et  << std::endl;
			average_idle = 0;
			average_loss = 0;
			average_en = 0;
			average_et = 0;
		}
	}
	return 0;
}