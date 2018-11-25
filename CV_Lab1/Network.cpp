#include "Network.h"

Network::Network(uint imageRows, uint imageCols) : outputNeurons(5)
{

	for (int i = 0; i < imageRows * imageCols; i++)
	{
		Neuron * n = new InputNeuron(0);

		for (uint k = 0; k < COUNT; k++)
		{
			InputNeuron::Input *in = new InputNeuron::Input();
			in->inputNeuron = n;
			outputNeurons.at(i).inputs.push_back(*in);
		}

		inputNeurons.push_back(n);
	}
}
