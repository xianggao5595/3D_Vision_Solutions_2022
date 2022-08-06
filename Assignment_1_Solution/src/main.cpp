// Assignment_1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// 7/6/2022: HW1 Example Solution

// Include C++ Input/Output, Math and Assert library
#include <iostream>
#include <cmath>
#include <assert.h>

// Include OpenCV library
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

const int rows = 1200;
const int cols = 1600;

void compute_ambient_modulation_relativephase(const std::vector<cv::Mat>& image, std::vector<cv::Mat>& output) {
	// 1. check if we have 3 input phase images
	assert(image.size() == 3);
	// 2. Initialize and compute the ambient, modulation and relatie phase 
	cv::Mat ambient(rows, cols, CV_64FC1);
	cv::Mat modulation(rows, cols, CV_64FC1);
	cv::Mat relativephase(rows, cols, CV_64FC1);
	for(size_t i = 0; i < rows; i ++)
		for (size_t j = 0; j < cols; j++) {
			ambient.at<double>(i, j) = 1.0 / 3.0 * (image[0].at<double>(i, j) + image[1].at<double>(i, j) + image[2].at<double>(i, j));	
			modulation.at<double>(i, j) = 1.0 / 3.0 * std::sqrt(3.0 * std::pow(image[0].at<double>(i, j) - image[2].at<double>(i, j),2.0)
				+ std::pow(2.0 * image[1].at<double>(i, j)-image[0].at<double>(i, j)-image[2].at<double>(i, j), 2.0));
			relativephase.at<double>(i, j) = std::atan2(std::sqrt(3.0) * (image[0].at<double>(i, j) - image[2].at<double>(i, j)), 2.0 * image[1].at<double>(i, j) - image[0].at<double>(i, j) - image[2].at<double>(i, j));
		}
	// 3. store the results into ouput 
	output.push_back(ambient);
	output.push_back(modulation);
	output.push_back(relativephase);
}
void display_save_output_ambient_modulation_relativephase(const std::vector<cv::Mat>& output) {

	// Normalize the phase image
	for (cv::Mat image : output) {
		cv::normalize(image, image, 0.0, 1.0, cv::NORM_MINMAX);
	}

	// Show ambient, modulation and phases images 
	cv::imshow("Ambient", output[0]);
	cv::waitKey(0);
	cv::imshow("Modulation", output[1]);
	cv::waitKey(0);
	cv::imshow("Relative Phase", output[2]);
	cv::waitKey(0);
	system("CLS");
	cv::destroyAllWindows();
	// Save all images
	cv::imwrite("Ambient.png", output[0] * 255);
	cv::imwrite("Modulation.png", output[1] * 255);
	cv::imwrite("RelativePhase.png", output[2] * 255);
	
}
int main()
{
	// Step 1: Read and display 3 phase images
	std::vector<cv::Mat> image;
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
		cv::waitKey(0); 
		std::system("CLS");
	}
	cv::destroyAllWindows();
	// Step 2: Compute, display and save ambient, modulation and relative phase images	
	std::vector<cv::Mat> output;
	compute_ambient_modulation_relativephase(image, output);
	display_save_output_ambient_modulation_relativephase(output);

	system("pause");
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
