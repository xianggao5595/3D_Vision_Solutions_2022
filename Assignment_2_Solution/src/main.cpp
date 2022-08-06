// Assignment2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// 
// 7/18/2022: HW2 Example Solution by Xiang
#define _USE_MATH_DEFINES

// Include C++ Input/Output, Math and Assert library
#include <iostream>
#include <cmath>
#include <assert.h>

// Include OpenCV library
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

const int rows = 1200, cols = 1600, nPeriod = 7;

void read_all_images(std::vector<cv::Mat>& image) {
	for (size_t i = 0; i < 42; i++)
	{
		image.push_back(cv::imread(std::to_string(i + 1) + ".png", cv::IMREAD_GRAYSCALE));
		image[i].convertTo(image[i], CV_64FC1);
		cv::normalize(image[i], image[i], 0.0, 1.0, cv::NORM_MINMAX);
	}

}

void initialize_multi_frequency_phaseunwrap(std::vector<cv::Mat>& unwrappedPhase_V, std::vector<cv::Mat>& unwrappedPhase_H, const std::vector<cv::Mat>& image) {
	// Initializing the absolute phases
	cv::Mat relativephase_H(rows, cols, CV_64FC1);
	cv::Mat relativephase_V(rows, cols, CV_64FC1);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
		{
			relativephase_V.at<double>(i, j) = std::atan2(std::sqrt(3.0) * (image[0].at<double>(i, j) - image[2].at<double>(i, j)), 2.0 * image[1].at<double>(i, j) - image[0].at<double>(i, j) - image[2].at<double>(i, j));
			relativephase_H.at<double>(i, j) = std::atan2(std::sqrt(3.0) * (image[0 + nPeriod * 3].at<double>(i, j) - image[2 + nPeriod * 3].at<double>(i, j)), 2.0 * image[1 + nPeriod * 3].at<double>(i, j) - image[0 + nPeriod * 3].at<double>(i, j) - image[2 + nPeriod * 3].at<double>(i, j));
		}
	unwrappedPhase_V.push_back(relativephase_V);
	unwrappedPhase_H.push_back(relativephase_H);
}

void vertical_multi_frequency_phaseunwrap(std::vector<cv::Mat>& unwrappedPhase_V, const std::vector<cv::Mat>& image) {
	// Vertical - Multi-frequency based phase unwrapping 
	for (size_t k = 0; k < 7 - 1; k++) {

		cv::Mat relativePhase(rows, cols, CV_64FC1);
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++)
			{
				relativePhase.at<double>(i, j) = std::atan2(std::sqrt(3.0) * (image[0 + 3 * (k + 1)].at<double>(i, j) - image[2 + 3 * (k + 1)].at<double>(i, j)), 2.0 * image[1 + 3 * (k + 1)].at<double>(i, j) - image[0 + 3 * (k + 1)].at<double>(i, j) - image[2 + 3 * (k + 1)].at<double>(i, j));
			}

		cv::Mat wrappedCount(rows, cols, CV_64FC1);
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++)
			{
				wrappedCount.at<double>(i, j) = std::round(unwrappedPhase_V[k].at<double>(i, j) / M_PI - relativePhase.at<double>(i, j) / (2 * M_PI));
			}

		cv::Mat absolutePhase(rows, cols, CV_64FC1);
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++)
			{
				absolutePhase.at<double>(i, j) = 2 * M_PI * wrappedCount.at<double>(i, j) + relativePhase.at<double>(i, j);
			}
		unwrappedPhase_V.push_back(absolutePhase);
	}

}

void horizontal_multi_frequency_phaseunwrap(std::vector<cv::Mat>& unwrappedPhase_H, const std::vector<cv::Mat>& image) {
	// Horizontal - Multi-frequency based phase unwrappping
	for (size_t k = 0; k < 7 - 1; k++) {

		cv::Mat relativePhase(rows, cols, CV_64FC1);
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++)
			{
				relativePhase.at<double>(i, j) = std::atan2(std::sqrt(3.0) * (image[0 + 3 * (k + 1) + nPeriod * 3].at<double>(i, j) - image[2 + 3 * (k + 1) + nPeriod * 3].at<double>(i, j)), 2.0 * image[1 + 3 * (k + 1) + nPeriod * 3].at<double>(i, j) - image[0 + 3 * (k + 1) + nPeriod * 3].at<double>(i, j) - image[2 + 3 * (k + 1) + nPeriod * 3].at<double>(i, j));
			}

		cv::Mat wrappedCount(rows, cols, CV_64FC1);
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++)
			{
				wrappedCount.at<double>(i, j) = std::round(unwrappedPhase_H[k].at<double>(i, j) / M_PI - relativePhase.at<double>(i, j) / (2 * M_PI));
			}

		cv::Mat absolutePhase(rows, cols, CV_64FC1);
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++)
			{
				absolutePhase.at<double>(i, j) = 2 * M_PI * wrappedCount.at<double>(i, j) + relativePhase.at<double>(i, j);
			}
		unwrappedPhase_H.push_back(absolutePhase);
	}

}

void visualize_all_vertical_absolute_phases(const std::vector<cv::Mat>& image) {

	// Normalize to display the images
	size_t count = 0;
	for (cv::Mat img : image) {
		count++;
		cv::normalize(img, img, 0.0, 1.0, cv::NORM_MINMAX);
		cv::imshow("Absolute Phase_" + std::to_string(count), img);
		cv::waitKey();
	}
}
void visualize_all_horizontal_absolute_phases(const std::vector<cv::Mat>& image) {

	// Normalize to display the images
	size_t count = 0 + 7;
	for (cv::Mat img : image) {
		count++;
		cv::normalize(img, img, 0.0, 1.0, cv::NORM_MINMAX);
		cv::imshow("Absolute Phase_" + std::to_string(count), img);
		cv::waitKey();
	}
}


int main() {

	std::vector<cv::Mat> image;
	read_all_images(image);

	// Initialize multifrequency phase unwrapping 
	std::vector<cv::Mat> unwrappedPhase_V, unwrappedPhase_H;
	initialize_multi_frequency_phaseunwrap(unwrappedPhase_V, unwrappedPhase_H, image);
	
	// Run the multifrequency phase unwrapping algorithm
	vertical_multi_frequency_phaseunwrap(unwrappedPhase_V, image);
	horizontal_multi_frequency_phaseunwrap(unwrappedPhase_H, image);
	
	// Display the absolute phase images
	visualize_all_vertical_absolute_phases(unwrappedPhase_V);
	visualize_all_horizontal_absolute_phases(unwrappedPhase_H);

	cv::waitKey();
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
