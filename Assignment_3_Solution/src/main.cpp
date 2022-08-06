// Assignment_3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// 7/18/2022 by Xiang
#define _USE_MATH_DEFINES

#include <iostream>
#include <cmath>

#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"

#include "graphcut_unwrapper.h"

const int rows = 1200, cols = 1600;

void read_display_images(std::vector<cv::Mat>& image) {
	for (size_t i = 0; i < 3; i++) {
		// Add phase image 
		image.push_back(cv::imread(std::to_string(i) + ".png", cv::IMREAD_GRAYSCALE));

		// Convert the phase image into double precision type
		image[i].convertTo(image[i], CV_64FC1);

		// Normalize the phase image
		cv::normalize(image[i], image[i], 0.0, 1.0, cv::NORM_MINMAX);
		cv::imshow("Relative Phase " + std::to_string(i), image[i]);
		if (i <= 1) std::cout << "Press Enter to show next image..." << std::endl;
		else std::cout << "Finish showing all images!" << std::endl;
		cv::waitKey(0.0);
		std::system("CLS");
	}

}

void compute_ambient_modulation_relativephase(const std::vector<cv::Mat>& image, std::vector<cv::Mat>& output) {
	// 1. check if we have 3 input phase images
	assert(image.size() == 3);
	// 2. Initialize and compute the ambient, modulation and relatie phase 
	cv::Mat ambient(rows, cols, CV_64FC1);
	cv::Mat modulation(rows, cols, CV_64FC1);
	cv::Mat relativephase(rows, cols, CV_64FC1);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++) {
			ambient.at<double>(i, j) = 1.0 / 3.0 * (image[0].at<double>(i, j) + image[1].at<double>(i, j) + image[2].at<double>(i, j));
			modulation.at<double>(i, j) = 1.0 / 3.0 * std::sqrt(3.0 * std::pow(image[0].at<double>(i, j) - image[2].at<double>(i, j), 2.0)
				+ std::pow(2.0 * image[1].at<double>(i, j) - image[0].at<double>(i, j) - image[2].at<double>(i, j), 2.0));
			relativephase.at<double>(i, j) = std::atan2(std::sqrt(3.0) * (image[0].at<double>(i, j) - image[2].at<double>(i, j)), 2.0 * image[1].at<double>(i, j) - image[0].at<double>(i, j) - image[2].at<double>(i, j));
		}
	// 3. store the results into ouput 
	output.push_back(ambient);
	output.push_back(modulation);
	output.push_back(relativephase);
}

void display_save_output_ambient_modulation_relativephase(const std::vector<cv::Mat>& output) 
{

	// Normalize the phase image
	for (cv::Mat image : output) {
		cv::normalize(image, image, 0.0, 1.0, cv::NORM_MINMAX);
	}

	// Show ambient, modulation and phases images 
	cv::imshow("Ambient", output[0]);
	cv::waitKey();
	cv::imshow("Modulation", output[1]);
	cv::waitKey();
	cv::imshow("Relative Phase", output[2]);
	cv::waitKey();
	system("CLS");

	// Save all images
	cv::imwrite("Ambient.png", output[0] * 255.0);
	cv::imwrite("Modulation.png", output[1] * 255.0);
	cv::imwrite("RelativePhase.png", output[2] * 255.0);

}

void show_single_image(const cv::Mat& image, const std::string& name) {
	cv::normalize(image, image, 0.0, 1.0, cv::NORM_MINMAX);
	cv::imshow(name, image);
	cv::waitKey();
	cv::imwrite(name + ".png", image * 255);
}

int main()
{

	// Step 1: Read and display 3 phase images
	std::vector<cv::Mat> image;
	read_display_images(image);

	// Step 2: Compute, display and save ambient, modulation and relative phase images	d
	std::vector<cv::Mat> output;
	compute_ambient_modulation_relativephase(image, output);
	display_save_output_ambient_modulation_relativephase(output);
	cv::destroyAllWindows();

	// Step 3: Graphcut Phase Unwrapping 

	// Declaring and initializing arrays
	double* arr_relativePhase = new double[rows * cols];
	double* arr_wrappedCount = new double[rows * cols];
	double* arr_absolutePhase = new double[rows * cols];

	cv::normalize(output[2], output[2], 0.0, 1.0, cv::NORM_MINMAX);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++) {
			size_t idx = i * cols + j;
			arr_relativePhase[idx] = output[2].at<double>(i, j)* 2 * M_PI - M_PI; // phase -pi to pi however output must be 0-255
			arr_wrappedCount[idx] = 0.0;
			arr_absolutePhase[idx] = 0.0;
		}

	// Graphcutunwrapping class designed for phase unwrapping
	MRF::CGraphcutunwrapper* Graphcut = new MRF::CGraphcutunwrapper(rows, cols);
	Graphcut->_input_wrapped_phase(arr_relativePhase);
	Graphcut->_unwrapping();
	Graphcut->_output_wrapped_count(arr_wrappedCount);
	Graphcut->_output_unwrapped_phase(arr_absolutePhase);

	cv::Mat absolutePhase(rows, cols, CV_64FC1);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++) {
			size_t idx = i * cols + j;
			absolutePhase.at<double>(i, j) = arr_absolutePhase[idx];
		}

	// display and save result
	show_single_image(absolutePhase, "absolute phase");
	cv::waitKey();

	delete[](arr_relativePhase, arr_wrappedCount, arr_absolutePhase);
    return 0;

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
