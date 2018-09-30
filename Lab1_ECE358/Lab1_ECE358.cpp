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
int queueSize;
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
int lossCounter;

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
		queue.pop();
	}
	else
	{
		packet->transmissionTime = packet->transmissionTime - 1;
	}
}

void arrive(int currentTime)
{
	if (currentTime >= currentArrivalTime) {
		if (queueSize == -1 || queue.size() < queueSize) {
			Packet* packet = new Packet();
			packet->arrivalTime = currentTime;
			packet->transmissionTime = transmissionTime;
			queue.push(packet);
		}
		else {
			lossCounter++;
		}
		int time = getTime(currentLambda);
		currentArrivalTime = time + currentArrivalTime;
		numGeneratedPackets++;
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
	totalTime = 5000000;
	numPacketsInQueue = 0;
}

void experiment2() {

	currentArrivalTime = getTime(currentLambda);

	for (int i = 0; i < totalTime; i++) {
		arrive(i);
		depart(i);

		numPacketsInQueue = numPacketsInQueue + queue.size();
	}
}

void getValues()
{
	int queueSizes[4] = { -1, 5, 10, 40 };
	for (int k = 0; k < 4; k++) {

		for (double utilization = 0.35; utilization <= 0.85; utilization += 0.1) {
			initialize();

			queueSize = queueSizes[k];

			currentLambda = (double)utilization *(double)C / (double)L;
			transmissionTime = L;
			experiment2();

			double average_idle = (double)idleTime / (double)totalTime;
			double average_en = (double)numPacketsInQueue / (double)totalTime;
			double numberofpacketsloss = (double)lossCounter / (double)numGeneratedPackets;


			std::cout << "Queue size: " << queueSize << ", " << "CurrentLambda: " << currentLambda << ", Average Idle time: " << average_idle << ", " << "Average num packets in queue: " << average_en << ", " << "Number of packets lost: " << numberofpacketsloss << std::endl;
		}
	}
}


int red()
{
	std::cout << "Hello World!\n";
	//testGenerateRandomVariable();
	getValues();
	return 0;
}


