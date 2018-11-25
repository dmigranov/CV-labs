#pragma once
#include "Neuron.h"
class InputNeuron : public Neuron
{
public:
	virtual double weightedSum();
	InputNeuron(double value);
};

