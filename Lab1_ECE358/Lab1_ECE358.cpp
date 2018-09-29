// Lab1_ECE358.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include<queue>


class Packet
{
public:
	int arrivalTime;
	int transmissionTime;
};

std::queue<Packet*> queue;

int timeScale = 1000000;

// 12000 bits
int L = 12000;
// 1 Mbits/second = 1000000 bits/second
int C = 1000000;

int currentArrivalTime;
int transmissionTime;
int numGeneratedPackets;
double currentLambda;
int sumSojournTime;
int idleTime;
int totalTime;
int numPacketsInQueue;

double getRandom()
{
	return (double)rand() / (double)(RAND_MAX);
}

double generateRandomVariable(double lambda)
{
	return (-1.0 / lambda) * log(1.0 - getRandom());
}

double getTime(double lambda) 
{
	return generateRandomVariable(lambda) * timeScale;
}

void testGenerateRandomVariable()
{
	double lambda = 75;
	const int numRandomVariablesGenerated = 1000;
	
	double generatedRandomVariables[numRandomVariablesGenerated];

	double sum = 0;

	for (int i = 0; i < numRandomVariablesGenerated; i++) {
		double generatedRandomVariable = generateRandomVariable(lambda);

		generatedRandomVariables[i] = generatedRandomVariable;

		sum += generatedRandomVariable;
	}

	double experimentalMean = sum / (double)numRandomVariablesGenerated;

	double varianceSum = 0;

	for (int i = 0; i < numRandomVariablesGenerated; i++) 
	{

		varianceSum += pow(generatedRandomVariables[i] - experimentalMean, 2);
	}

	double experimentalVariance = varianceSum / (double)numRandomVariablesGenerated;


	std::cout << "The experimental mean is: " << experimentalMean << std::endl;
	std::cout << "The experimental variance is: " << experimentalVariance << std::endl;
}


void depart(int currentTime)
{
	if (queue.size() <= 0) {
		idleTime = idleTime + 1;
		return;
	};

	Packet* packet = queue.front();

	if (packet->transmissionTime <= 0) {
		sumSojournTime = sumSojournTime + (currentTime - packet->arrivalTime);
	}
	else
	{
		packet->transmissionTime = packet->transmissionTime - 1;
	}
}

void arrive(int currentTime)
{
	if (currentArrivalTime <= 0) {
		Packet* packet = new Packet();
		packet->arrivalTime = currentTime;
		packet->transmissionTime = transmissionTime;
		queue.push(packet);
		numGeneratedPackets++;

		currentArrivalTime = getTime(currentLambda) * timeScale;
	} 
	else {
		currentArrivalTime = currentArrivalTime - 1;
	}
}

void initialize()
{
	currentArrivalTime = 0;
	transmissionTime = 0;
	numGeneratedPackets = 0;
	currentLambda = 0;
	sumSojournTime = 0;
	idleTime = 0;
	totalTime = 5000*timeScale;
	numPacketsInQueue = 0;
}

void experiment() {

	currentArrivalTime = getTime(currentLambda) * timeScale;

	for (int i = 0; i < totalTime; i++) {
		arrive(i);
		depart(i);

		numPacketsInQueue += numPacketsInQueue + queue.size();
	}
}

void getValues()
{
	for (double utilization = 0.25; utilization < 0.95; utilization += 0.1) {
		initialize();

		currentLambda = utilization * C / L;

		experiment();

		int average_idle = idleTime;
		int average_en = numPacketsInQueue / totalTime;
		int averageSojournTime = sumSojournTime;

		std::cout << "Average Idle time: " << average_idle << ", " << "Average num packets in queue: " << average_en<< ", " << "Average time to be processed " << averageSojournTime << std::endl;
	}
}


int main()
{
	std::cout << "Hello World!\n";
	//testGenerateRandomVariable();
	getValues();
}


