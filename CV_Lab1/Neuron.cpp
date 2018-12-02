#include "Neuron.h"

void Neuron::setValue(double value)
{
	this->value = value;
}

Neuron::Neuron()
{
	this->value = 0;
}

Neuron::Neuron(double value)
{
	this->value = value;

}

double Neuron::activationFunction()
{
	return (1 / (1 + exp(-(this->weightedSum()))));
}

double Neuron::weightedSum()
{
	double result = 0;
	for (Input input : inputs)
	{
		result += input.weight * input.inputNeuron->activationFunction();
	}
	return result;
}
