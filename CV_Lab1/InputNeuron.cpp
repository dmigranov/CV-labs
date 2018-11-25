#include "InputNeuron.h"

double InputNeuron::weightedSum()
{
	return value;
}

InputNeuron::InputNeuron(double value)
{
	this->value = value;
}
