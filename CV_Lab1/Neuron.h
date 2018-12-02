#pragma once
#include <vector>

class Neuron
{


public:
	class Input
	{
	public:
		double weight = 0.1;
		Neuron * inputNeuron;
	};

	void setValue(double value);
	Neuron();
	Neuron(double value);
	double activationFunction();
	virtual double weightedSum();

	std::vector<Input> inputs;
protected:
	double value;


};