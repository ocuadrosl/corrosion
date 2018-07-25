/*
 * Image3D.h
 *
 *  Created on: Jan 5, 2018
 *      Author: oscar
 */

#ifndef SRC_IMAGE3D_H_
#define SRC_IMAGE3D_H_

#include "common/InputOutput.h"
#include "common/Types.h"
#include "common/ImageProcessing.h"
#include "itkTileImageFilter.h"
#include "common/MutualInformationRegistration.h"
#include "common/Math.h"
#include "itkImageMomentsCalculator.h"
#include "common/ImageProcessing.h"

#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelMap.h"

#include <dirent.h>
#include <sys/stat.h>

class Image3D
{
public:

	Image3D();
	~Image3D()
	{
	}
	;

	void readImageSeries(const std::string &dirName, std::string format, const std::string & fileExt);
	void createImage3D();
	std::vector<type::grayImagePointer> imageSeries; //to store the original images
	void setFixedImage(type::grayImagePointer fixedImage);
	void setFixedImage(unsigned imageId);
	type::grayImagePointer3D getImage3D() const;

	double getConversionFactor() const;
	double getHeight() const;
	double getMeanAbraded() const;

	void setAbradedMean(double meanAbraded);
	void setConversionFactor(double conversionFactor);
	void setAbradedHeight(double height);
	void setAbradedHeightByLayer(const std::vector<double>& abradedHeightByLayer);
	void setBreakPoint(int breakPoint);

private:
	void changeCanvasSize();
	void imageSeriesAlignment();
	void fillAbradedSamplesByInterpolationAverage();
	void fillAbradedSamplesByInterpolation();
	void closeBorders(double radious);
	void imageSeriesSegmentation();

	type::grayImagePointer3D imageSeriesTo3DImage();
	type::grayFloatImagePointer fixedImage;
	int fixedImageId; //default value = null = -1
	int breakPoint;

	type::grayImagePointer3D image3D;

	double abradedHeight; //in millimeters
	double abradedMean; //in millimeters, how many millimeters are there between two samples, in average
	double conversionFactor; //how many pixels are there in a millimeter.

	std::vector<double> abradedHeightByLayer; //it contains the abraded height by sample

};
Image3D::Image3D() :
		fixedImageId(-1), abradedHeight(0.0), abradedMean(0.0), conversionFactor(0), breakPoint(-1)
{

}

void Image3D::setBreakPoint(int breakPoint)
{
	this->breakPoint = breakPoint;
}

void Image3D::setAbradedHeightByLayer(const std::vector<double>& abradedHeightByLayer)
{
	this->abradedHeightByLayer = abradedHeightByLayer;
}

double Image3D::getConversionFactor() const
{
	return conversionFactor;
}
/*
 * Number of pixels in a millimeter
 *
 * */
void Image3D::setConversionFactor(double conversionFactor)
{
	this->conversionFactor = conversionFactor;
}

double Image3D::getHeight() const
{
	return abradedHeight;
}

void Image3D::setAbradedHeight(double height)
{
	this->abradedHeight = height;
}

double Image3D::getMeanAbraded() const
{
	return abradedMean;
}

void Image3D::setAbradedMean(double meanAbraded)
{
	this->abradedMean = meanAbraded;
}

type::grayImagePointer3D Image3D::getImage3D() const
{
	return image3D;
}

void Image3D::setFixedImage(unsigned imageId)
{
	fixedImageId = static_cast<int>(imageId);
}

type::grayImagePointer3D Image3D::imageSeriesTo3DImage()
{

	typedef itk::TileImageFilter<type::grayImageType, type::grayImageType3D> FilterType;
	typename FilterType::Pointer filter = FilterType::New();

	itk::FixedArray<unsigned, 3> layout;
	layout[0] = 1;
	layout[1] = 1;
	layout[2] = 0;

	filter->SetLayout(layout);

	for (int i = 0; i < imageSeries.size(); ++i)
	{

		imageSeries[i]->DisconnectPipeline();

		filter->SetInput(i, imageSeries[i]);
	}
	filter->Update();

	io::print("Creating the 3D image", 1);

	return filter->GetOutput();;

}

void Image3D::setFixedImage(type::grayImagePointer fixedImage)
{
	this->fixedImage = io::imageCast<type::grayImageType, type::grayFloatImageType>(fixedImage);
}

/*
 *
 * This changes the canvas size of all images using the largest one as the reference
 *
 * */
void Image3D::changeCanvasSize()
{

	unsigned maxSize = 0;
	type::grayImageType::SizeType regionSize;

	for (unsigned i = 0; i < imageSeries.size(); ++i)
	{
		regionSize = imageSeries[i]->GetLargestPossibleRegion().GetSize();
		maxSize = (regionSize[0] > maxSize) ? regionSize[0] : ((regionSize[1] > maxSize) ? regionSize[1] : maxSize);
	}

	for (unsigned i = 0; i < imageSeries.size(); ++i)
	{
		imageSeries[i] = ip::changeCanvasImageSize<type::grayImageType>(imageSeries[i], maxSize, maxSize); //squared images
	}

	if (fixedImage.IsNotNull())
	{
		fixedImage = ip::changeCanvasImageSize<type::grayFloatImageType>(fixedImage, maxSize, maxSize);
		//todo messages ...
	}

	io::print("Standardizing canvas size", 1);

}
/*
 *
 *	no "/" at the end of dirName
 *
 *
 */
void Image3D::readImageSeries(const std::string &dirName, std::string format, const std::string & fileExt)
{

	//std::vector<std::string> imageNames = io::imageNamesGenerator(dirName, format, fileExt);

	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dirName.c_str())) == NULL)
	{
		std::cerr << "Error opening " << dirName << std::endl;
		return;
	}
	std::set<int> fileNumericNames; //to store numeric-format names

	while ((dirp = readdir(dp)) != NULL)
	{
		std::string fileName = dirp->d_name;
		//avoiding hidden files . and ..
		if (fileName.compare(".") != 0 && fileName.compare("..") != 0)
		{
			//sorting the files numerically
			try
			{
				fileNumericNames.insert(std::stoi(fileName.replace(fileName.find("."), fileName.length() - 1, "")));

			} catch (const std::exception& e)
			{
				std::cerr << "WARNING: image \"" << fileName << "\" does not have a numeric name" << std::endl;
			}

		}

	}

	//Creating the images names vector
	std::vector<std::string> imageNames;
	for (std::set<int>::iterator it = fileNumericNames.begin(); it != fileNumericNames.end(); ++it)
	{
		imageNames.push_back(dirName + "/" + std::to_string(*it) + "." + fileExt);
	}

	imageSeries = io::readImageSeries(imageNames);

	io::print("Reading image series: " + std::to_string(fileNumericNames.size()), 1);

}
/*
 * This function uses mutual information to register all
 * images with the selected image as fixed sample
 *
 * */
void Image3D::imageSeriesAlignment()
{

	type::grayFloatImagePointer moving;

	if (fixedImageId >= 0) //a fixed image ID was set
	{
		fixedImage = io::imageCast<type::grayImageType, type::grayFloatImageType>(imageSeries[static_cast<unsigned>(fixedImageId)]);

	}

	if (fixedImage.IsNotNull()) //a fixed image was set
	{
		for (unsigned i = 0; i < imageSeries.size(); ++i)
		{
			moving = io::imageCast<type::grayImageType, type::grayFloatImageType>(imageSeries[i]);
			imageSeries[i] = ip::mutualInformationRegistration(fixedImage, moving);
			std::cout << "Aligning images " << math::to_percentage<int>(i + 1, imageSeries.size()) << "%" << std::endl;

		}
	}
	else //no fixed image set, using the i-1 as fixed
	{
		type::grayFloatImagePointer fixed;
		for (unsigned i = 1; i < imageSeries.size(); ++i)
		{
			fixed = io::imageCast<type::grayImageType, type::grayFloatImageType>(imageSeries[i - 1]);
			moving = io::imageCast<type::grayImageType, type::grayFloatImageType>(imageSeries[i]);
			imageSeries[i] = ip::mutualInformationRegistration(fixed, moving);
			std::cout << "Aligning images " << math::to_percentage<int>(i + 1, imageSeries.size()) << "%" << std::endl;
		}
	}

	io::print("Aligning images", 1);

}

void Image3D::fillAbradedSamplesByInterpolation()
{
	//validation
	if (abradedHeightByLayer.size() != imageSeries.size())
	{
		std::cerr << "WARNING: the number of images is not equal to the number of abrading measurements" << std::endl;
		std::cerr << "Using the average for interpolation" << std::endl;
		fillAbradedSamplesByInterpolationAverage();
		return;

	}

	std::vector<type::grayImagePointer> newImageSeries;

	//for all input images
	for (unsigned i = 0; i < imageSeries.size() - 1; ++i)
	{
		double abradedHeightInPixels = math::millimetersToPixels(abradedHeightByLayer[i + 1], conversionFactor);

		int stepSize = 1.0 / abradedHeightInPixels;

		double currentImage = 0;

		//abradedHeightInPixels must be greater than 0
		int abradedHeightInPixelsRounded = (abradedHeightInPixels < 1.0) ? 1 : static_cast<int>(std::round(abradedHeightInPixels));

		//std::cout << abradedHeightInPixels << " " << abradedHeightInPixelsRounded << std::endl;

		for (int j = 0; j < abradedHeightInPixelsRounded; ++j)
		{
			if ((breakPoint-1) == i) //there is a break point
			{
				std::cout<<"break point"<<std::endl;
				newImageSeries.push_back(imageSeries[i + 1]);
			}
			else
			{
				type::grayImagePointer interpolatedImage;
				interpolatedImage = ip::createImageByInterpolation<type::grayImageType>(imageSeries[i], imageSeries[i + 1], currentImage);
				newImageSeries.push_back(interpolatedImage);
			}
			currentImage += stepSize;
		}

	}

	imageSeries = newImageSeries;

	io::print("Interpolating images", 1);
}

/*
 * This function fills the abraded areas by linear interpolation.
 * It is assumed here that the axial slices (input samples) have no thickness (0 millimeters).
 *
 * */
void Image3D::fillAbradedSamplesByInterpolationAverage()
{
	double abradedMeanInPixels = math::millimetersToPixels(abradedMean, conversionFactor);
	double heightInPixels = math::millimetersToPixels(abradedHeight, conversionFactor);

	double onePixelInMm = 1.0 / conversionFactor;
	//std::cout << onePixelInMm << std::endl;

	double abradeBeginMm = 0;
	double abradeEndMm = 0;

	std::vector<type::grayImagePointer> newImageSeries;

//for all input images
	for (unsigned i = 0; i < imageSeries.size() - 1; ++i)
	{
		//std::cout << "samples: " << i << " " << i + 1 << std::endl;

		abradeEndMm = math::minMax<double>(i + 1, 0, imageSeries.size() - 1, 0, abradedHeight);

		//std::cout << abradeBeginMm << " " << abradeEndMm << std::endl;

		unsigned abradedPixels = std::ceil((abradeEndMm - abradeBeginMm) / onePixelInMm);
		double stepSize = 1 / static_cast<double>(abradedPixels);

		double currentImage = 0;

		//creating an image for each empty space of size abradedMean
		for (double j = abradeBeginMm; j < abradeEndMm; j = j + onePixelInMm)
		{
			//std::cout << "current " << currentImage << std::endl;

			type::grayImagePointer interpolatedImage;
			interpolatedImage = ip::createImageByInterpolation<type::grayImageType>(imageSeries[i], imageSeries[i + 1], currentImage);
			newImageSeries.push_back(interpolatedImage);

			currentImage += stepSize;

		}

		//std::cout << "count " << abradedPixels << std::endl;
		abradeBeginMm = abradeEndMm;

	}

	imageSeries = newImageSeries;

	io::print("Creating 3D image by interpolation", 1);

}

/*
 * This function uses a bounding circle approach to close open pits
 *
 * */
void Image3D::closeBorders(double radious)
{

	//moments calculator types declaration
	using momentsCalculatorType = itk::ImageMomentsCalculator<type::grayImageType>;
	momentsCalculatorType::Pointer momentsCalculator = momentsCalculatorType::New();

	for (unsigned i = 0; i < imageSeries.size(); ++i)
	{
		//center of gravity

		//momentsCalculator->SetImage(imageSeries[i]);
		//momentsCalculator->Compute();

		//momentsCalculatorType::VectorType tmpCenter = momentsCalculator->GetCenterOfGravity();

		type::grayImageType::IndexType center = imageSeries[i]->GetLargestPossibleRegion().GetUpperIndex();
		center[0] /= 2;
		center[1] /= 2;

		//define if a point is inside a circle

		itk::ImageRegionIterator<type::grayImageType> it(imageSeries[i], imageSeries[i]->GetLargestPossibleRegion());
		for (it.GoToBegin(); !it.IsAtEnd(); ++it)
		{
			if (math::euclideanDistance<type::grayImageType::IndexType>(it.GetIndex(), center) < radious) //is inside
			{
				it.Set(it.Get() == 0 ? 1 : 0);
			}
			else
			{
				it.Set(0);

			}
		}

	}

	io::print("Closing borders", 1);
}

void Image3D::imageSeriesSegmentation()
{

	type::grayImageType::IndexType seed;

	seed[0] = 800;
	seed[1] = 800;
	//seed[2] = 100;

	for (unsigned i = 0; i < imageSeries.size(); ++i)
	{

		imageSeries[i] = ip::connectedThresholdFilter<type::grayImageType>(imageSeries[i], seed);
	}

}

void Image3D::createImage3D()
{

//standardizing canvas size
	changeCanvasSize();

//image series alignment
	//imageSeriesAlignment();

//interpolation
//fillAbradedSamplesByInterpolationAverage();
	fillAbradedSamplesByInterpolation();

	//imageSeriesSegmentation();

	//closing borders
	//closeBorders(890);

	image3D = imageSeriesTo3DImage();

//updating image properties
	type::grayImageType3D::SpacingType spacing;

	spacing[0] = spacing[1] = spacing[2] = 1 / conversionFactor;

	image3D->SetSpacing(spacing);

	io::print("Create Image 3D", 1);

}

#endif /* SRC_IMAGE3D_H_ */
