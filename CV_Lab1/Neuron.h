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
	Neuron(double value);
	double activationFunction();
	virtual double weightedSum();
protected:
	std::vector<Input> inputs;
	double value;


};