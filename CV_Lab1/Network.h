#pragma once

constexpr auto COUNT = 5;

#include <opencv2/opencv.hpp>
#include "Neuron.h"
#include "InputNeuron.h"

using namespace cv;
class Network
{

public:
	Network(uint imageRows, uint imageCols);
private:
	std::vector<Neuron*> inputNeurons;

	std::vector<Neuron> outputNeurons;

};

