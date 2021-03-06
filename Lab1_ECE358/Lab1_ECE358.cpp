// Lab1_ECE358.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"

#include <iostream>
#include<queue>


class SimulatedPacket
{
private:
	double packetArrivedTime;
	double packetTransmissionTime;

public: 
	SimulatedPacket(double arrivedTime, double transmissionTime)
	{
		packetArrivedTime = arrivedTime;
		packetTransmissionTime = transmissionTime;
	}

	double getPacketArrivedTime()
	{
		return packetArrivedTime;
	}

	double getPacketTransmissionTime()
	{
		return packetTransmissionTime;
	}

	void decrementPacketTransmissionTime()
	{
		packetTransmissionTime = packetTransmissionTime - 1;
	}

};

class TransmissionDetails
{
public:
	double L = 12000;
	double C = 1000000;
	double getSimulatedTransmissionTime()
	{
		// For this case
		return L;
	}

};

class BufferDetails
{
public:
	double bufferSize; 
	bool isFinite;
	BufferDetails(double size)
	{
		bufferSize = size;
		if (size == 0) {
			isFinite = false; 
		}
		else {
			isFinite = true;
		}
	}
};

std::queue<SimulatedPacket*> simulatedBuffer;

void clearSimulatedBuffer()
{
	while (simulatedBuffer.size() > 0) {
		simulatedBuffer.pop();
	}
}

double getTimeScale()
{
	return 1000000;
}

class CurrentServerDetails
{
private: 
	double totalNumberOfGeneratedPackets;
	double totalNumberOfDroppedPackets;

public:
	double currentArrivalTime;
	double totalTime = 6000000;
	double totalIdleTime;
	double sumNumPacketsInBuffer;

	CurrentServerDetails(double arrivalTime)
	{
		currentArrivalTime = arrivalTime;
		totalNumberOfGeneratedPackets = 0;
		totalNumberOfDroppedPackets = 0;
		totalIdleTime = 0;
		sumNumPacketsInBuffer = 0;
	}

	void incrementNumberOfGeneratedPackets()
	{
		totalNumberOfGeneratedPackets++;
	}
	
	double getTotalNumberOfGeneratedPackets()
	{
		return totalNumberOfGeneratedPackets;
	}

	void incrementTotalNumberOfDroppedPackets()
	{
		totalNumberOfDroppedPackets++;
	}

	double getTotalNumberOfDroppedPackets()
	{
		return totalNumberOfDroppedPackets;
	}

	void incrementCurrentArrivalTime(double arrivalTime)
	{
		currentArrivalTime += arrivalTime;
	}
	
	double getCurrentArrivalTime()
	{
		return currentArrivalTime;
	}
	bool hasArrived(double time)
	{
		if (time >= currentArrivalTime)
		{
			return true;
		}
		else {
			return false;
		}
	}

	double getPIdle()
	{
		return (double)totalIdleTime / (double)totalTime;
	}

	double getAverageNumPackets()
	{
		return (double)sumNumPacketsInBuffer / (double)totalTime;;
	}

	double getPLoss()
	{
		return (double)getTotalNumberOfDroppedPackets() / (double)getTotalNumberOfGeneratedPackets();
	}
};

TransmissionDetails transmissionDetails;
BufferDetails* bufferDetails;
CurrentServerDetails* currentServerDetails;

double currentLambda;

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
	return generateRandomVariable(lambda) * getTimeScale();
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

	double mean = sum / (double)numRandomVariablesGenerated;

	double varianceSum = 0;

	for (int i = 0; i < numRandomVariablesGenerated; i++) 
	{

		varianceSum += pow(generatedRandomVariables[i] - mean, 2);
	}

	double variance = (double)varianceSum / (double)numRandomVariablesGenerated;

	std::cout << "Mean: " << mean << std::endl;
	std::cout << "Variance: " << variance << std::endl;
}

void checkPacketDepart()
{
	SimulatedPacket* packet = simulatedBuffer.front();

	if (packet->getPacketTransmissionTime() <= 0) {
		simulatedBuffer.pop();
	}
	else
	{
		packet->decrementPacketTransmissionTime();
	}
}

void packetHasArrived(int presentTime)
{
	currentServerDetails->incrementNumberOfGeneratedPackets();

	if (!(bufferDetails->isFinite) || simulatedBuffer.size() < bufferDetails->bufferSize) {
		SimulatedPacket* packet = new SimulatedPacket(presentTime, transmissionDetails.getSimulatedTransmissionTime());
		simulatedBuffer.push(packet);
	}
	else {
		currentServerDetails->incrementTotalNumberOfDroppedPackets();
	}
	int time = getTime(currentLambda);
	currentServerDetails->incrementCurrentArrivalTime(time);
	 
}

void initialize(int bufferSize, int lambda)
{
	clearSimulatedBuffer();
	bufferDetails = new BufferDetails(bufferSize);
	transmissionDetails = TransmissionDetails();
	currentServerDetails = new CurrentServerDetails(0);
	currentServerDetails->incrementCurrentArrivalTime(getTime(lambda));
}

void runSimulation() {

	for (int i = 0; i < currentServerDetails->totalTime; i++) {
		if (currentServerDetails->hasArrived(i)) 
		{
			packetHasArrived(i);
		}
		if (simulatedBuffer.size() == 0) 
		{
			currentServerDetails->totalIdleTime = currentServerDetails->totalIdleTime + 1;
		}
		else 
		{
			checkPacketDepart();
		}
		currentServerDetails->sumNumPacketsInBuffer = currentServerDetails->sumNumPacketsInBuffer + simulatedBuffer.size();
	}
}

void printInfo(int queueSize, double averageNumberOfPackets, double utilization, double averageIdleTime, double lambda, double pLoss)
{
	if (queueSize == 0)
	{
		std::cout << "Queue Size: " << "INF";
	}
	else
	{
		std::cout << "Queue Size: " << queueSize;
	}
	std::cout << ", ";
	std::cout << "Lambda: " << lambda;
	std::cout << ", ";
	std::cout << "Utilization: " << utilization;
	std::cout << ", ";
	std::cout << "E[N]: " << averageNumberOfPackets;
	std::cout << ", ";
	std::cout << "PIdle: " << averageIdleTime;
	std::cout << ", ";
	std::cout << "PLoss: " << pLoss;
	std::cout << std::endl;
}

void question1Answer()
{
	std::cout << "Question 1 Answer: " << std::endl;
	std::cout << std::endl;
	testGenerateRandomVariable();
	std::cout << std::endl;
}

void question3Answer()
{
	std::cout << "Question 3 Answer: " << std::endl;
	std::cout << std::endl;

	for (double utilization = 0.35; utilization <= 0.85; utilization += 0.1) {
		currentLambda = (double)utilization *(double)transmissionDetails.C / (double)transmissionDetails.L;
		initialize(0, currentLambda);
		runSimulation();
		printInfo(0, currentServerDetails->getAverageNumPackets(), utilization, currentServerDetails->getPIdle(), currentLambda, currentServerDetails->getPLoss());
	}
	 std::cout << std::endl;
}

void question4Answer()
{
	std::cout << "Question 4 Answer: " << std::endl;
	std::cout << std::endl;
	currentLambda = (double)1.2 *(double)transmissionDetails.C / (double)transmissionDetails.L;
	initialize(0, currentLambda);
	runSimulation();
	printInfo(0, currentServerDetails->getAverageNumPackets(), 1.2, currentServerDetails->getPIdle(), currentLambda, currentServerDetails->getPLoss());
	std::cout << std::endl;
}


void question6Answer()
{
	std::cout << "Question 6 Answer: " << std::endl;
	std::cout << std::endl;

	int bufferSizes[4] = { 0, 5, 10, 40 };
	for (int i = 0; i < 4; i++) {

		for (double utilization = 0.5; utilization <= 2.0; utilization += 0.1) {
			currentLambda = (double)utilization *(double)transmissionDetails.C / (double)transmissionDetails.L;
			initialize(bufferSizes[i], currentLambda);
			runSimulation();
			printInfo(bufferSizes[i], currentServerDetails->getAverageNumPackets(), utilization, currentServerDetails->getPIdle(), currentLambda, currentServerDetails->getPLoss());
		}

		currentLambda = (double)2.0 *(double)transmissionDetails.C / (double)transmissionDetails.L;
		initialize(bufferSizes[i], currentLambda);
		runSimulation();
		printInfo(bufferSizes[i], currentServerDetails->getAverageNumPackets(), 2.0, currentServerDetails->getPIdle(), currentLambda, currentServerDetails->getPLoss());

		for (double utilization = 2.2; utilization <= 5.0; utilization += 0.2) {
			currentLambda = (double)utilization *(double)transmissionDetails.C / (double)transmissionDetails.L;
			initialize(bufferSizes[i], currentLambda);
			runSimulation();
			printInfo(bufferSizes[i], currentServerDetails->getAverageNumPackets(), utilization, currentServerDetails->getPIdle(), currentLambda, currentServerDetails->getPLoss());
		}

		currentLambda = (double)5.0 *(double)transmissionDetails.C / (double)transmissionDetails.L;
		initialize(bufferSizes[i], currentLambda);
		runSimulation();
		printInfo(bufferSizes[i], currentServerDetails->getAverageNumPackets(), 5.0, currentServerDetails->getPIdle(), currentLambda, currentServerDetails->getPLoss());

		for (double utilization = 5.4; utilization <= 10.0; utilization += 0.4) {
			currentLambda = (double)utilization *(double)transmissionDetails.C / (double)transmissionDetails.L;
			initialize(bufferSizes[i], currentLambda);
			runSimulation();
			printInfo(bufferSizes[i], currentServerDetails->getAverageNumPackets(), utilization, currentServerDetails->getPIdle(), currentLambda, currentServerDetails->getPLoss());
		}

		currentLambda = (double)10.0 *(double)transmissionDetails.C / (double)transmissionDetails.L;
		initialize(bufferSizes[i], currentLambda);
		runSimulation();
		printInfo(bufferSizes[i], currentServerDetails->getAverageNumPackets(), 10.0, currentServerDetails->getPIdle(), currentLambda, currentServerDetails->getPLoss());
	}
	std::cout << std::endl;
}

int main()
{
	question1Answer();
	question3Answer();
	question4Answer();
	question6Answer();
	return 0;
}


