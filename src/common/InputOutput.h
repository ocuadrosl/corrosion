/*
 * InputOutput.h
 *
 *  Created on: Jan 5, 2018
 *      Author: oscar
 */

#ifndef SRC_COMMON_INPUTOUTPUT_H_
#define SRC_COMMON_INPUTOUTPUT_H_

#include <vector>
#include "itkNumericSeriesFileNames.h"
#include "itkImageFileWriter.h"
#include "itkImageFileReader.h"
#include "itkCastImageFilter.h"
#include <dirent.h>
#include <iomanip>
#include "../common/Types.h"
#include "itkMeshFileWriter.h"
#include "itkTriangleMeshToBinaryImageFilter.h"

namespace io
{

void print(const std::string& message, const bool & ok, short size = 50)
{

	short diff = size - message.length();

	if (ok)
	{
		std::cout << message << std::setfill(' ') << std::setw(diff) << "[OK]" << std::endl;
	}
	else
	{
		//std::cout << message << std::setfill(' ') << std::setw(diff) << "[FAIL]" << std::endl;
	}

}
/*
 *	FORMAT a single column with all the measures in millimeters
 *
 *
 * */
std::vector<double> readAbradedHeightByLayerFile(std::string fileName)
{
	std::ifstream inputFile;

	inputFile.open(fileName.c_str(), std::ifstream::in);

	std::vector<double> measurements;

	if (inputFile.is_open())
	{
		double value;
		while (inputFile >> value)
		{
			measurements.push_back(value);
		}

	}
	else
	{
		std::cerr << "Abraded-height-by-layer file is not found" << std::endl;

	}

	io::print("Reading abraded-height-by-layer file:" + std::to_string(measurements.size()), 1);

	return measurements;

}

/*
 * basic names generator: 1,2,3,4...,n
 *
 * */
std::vector<std::string> imageNamesGenerator(const std::string &dir, std::string format, const std::string & fileExt)
{

	itk::NumericSeriesFileNames::Pointer numericSeriesFileNames = itk::NumericSeriesFileNames::New();
	numericSeriesFileNames->SetStartIndex(1);
	numericSeriesFileNames->SetEndIndex(58);
	numericSeriesFileNames->SetIncrementIndex(1);
	numericSeriesFileNames->SetSeriesFormat(dir + format + "%d." + fileExt.c_str());

	io::print("Create names", 1);

	return numericSeriesFileNames->GetFileNames();

}

template<typename imageType>
void writeImage(const typename imageType::Pointer &image, const std::string &fileName, const std::string& message = "")
{

	typedef itk::ImageFileWriter<imageType> writer_t;
	typename writer_t::Pointer writer = writer_t::New();
	writer->SetFileName(fileName);
	writer->SetInput(image);
	writer->Update();

	print("Write image " + message, true);
}

template<typename imageType>
typename imageType::Pointer readImage(const std::string& file_name)
{

	typedef itk::ImageFileReader<imageType> readerType;

	typename readerType::Pointer reader = readerType::New();

	reader->SetFileName(file_name);

	try
	{
		reader->Update();

	} catch (itk::ExceptionObject & e)
	{
		std::cerr << "Exception reading: " << file_name << " null pointer is returned" << std::endl;
		typename imageType::Pointer nullImage;
		return nullImage;
	}

	return reader->GetOutput();

}

std::vector<type::grayImagePointer> readImageSeries(const std::vector<std::string> &fileNames)
{

	std::vector<type::grayImagePointer> images;

	for (unsigned int i = 0; i < fileNames.size(); ++i)
	{
		type::grayImagePointer image = readImage<type::grayImageType>(fileNames[i]);

		if (image.IsNotNull())
		{
			images.push_back(image);

		}
	}

	return images;

}

template<typename inputType, typename outputType>
typename outputType::Pointer imageCast(typename inputType::Pointer inputImage)
{
	typedef itk::CastImageFilter<inputType, outputType> castFilterType;
	typename castFilterType::Pointer castFilter = castFilterType::New();
	castFilter->SetInput(inputImage);
	castFilter->Update();

	return castFilter->GetOutput();
}

void writeMesh(type::meshTypePointer inputMesh, const std::string & fileName)
{
	typedef itk::MeshFileWriter<type::meshType> WriterType;
	WriterType::Pointer writer = WriterType::New();
	writer->SetFileName(fileName);
	writer->SetInput(inputMesh);
	writer->Update();
	io::print("Write mesh", 1);
}

}
#endif /* SRC_COMMON_INPUTOUTPUT_H_ */
