/*
 * ImageProcessing.h
 *
 *  Created on: Jan 5, 2018
 *      Author: oscar
 */

#ifndef SRC_COMMON_IMAGEPROCESSING_H_
#define SRC_COMMON_IMAGEPROCESSING_H_

#include "Types.h"

#include "itkScaleTransform.h"
#include "itkResampleImageFilter.h"
#include "itkPasteImageFilter.h"
#include "itkOtsuThresholdImageFilter.h"
#include "Math.h"
#include "itkNeighborhoodConnectedImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkCylinderSpatialObject.h"
#include "itkSpatialObjectToImageFilter.h"
#include "itkConnectedComponentImageFilter.h"
#include "itkLabelImageToLabelMapFilter.h"
#include "itkLabelToRGBImageFilter.h"

#include "itkKittlerIllingworthThresholdImageFilter.h"
#include "itkHistogramThresholdImageFilter.h"
#include "itkHuangThresholdImageFilter.h"
#include "itkLiThresholdImageFilter.h"
#include "itkIntermodesThresholdImageFilter.h"
#include "itkIsoDataThresholdImageFilter.h"
#include "itkMaximumEntropyThresholdImageFilter.h"
#include "itkMomentsThresholdImageFilter.h"
#include "itkRenyiEntropyThresholdImageFilter.h"
#include "itkShanbhagThresholdImageFilter.h"
#include "itkTriangleThresholdImageFilter.h"
#include "itkYenThresholdImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"

#include <thread>

namespace ip
{
/*
 * test it
 *
 * */
template<typename imageType>
typename imageType::Pointer scale(typename imageType::Pointer inputImage, float factor)
{

	const unsigned dimension = imageType::ImageDimension;

	typedef itk::ScaleTransform<double, dimension> transformType;
	typename transformType::Pointer scaleTransform = transformType::New();
	itk::FixedArray<float, imageType::ImageDimension> scale;
	scale.Fill(factor);
	scaleTransform->SetScale(scale);

	itk::Point<float, dimension> center;
	for (unsigned i = 0; 0 < dimension; ++i)
	{
		center[i] = inputImage->GetLargestPossibleRegion().GetSize()[i] / 2;
	}

	scaleTransform->SetCenter(center);

	typedef itk::ResampleImageFilter<imageType, imageType> ResampleImageFilterType;
	typename ResampleImageFilterType::Pointer resampleFilter = ResampleImageFilterType::New();
	resampleFilter->SetTransform(scaleTransform);
	resampleFilter->SetInput(inputImage);
	resampleFilter->SetSize(inputImage->GetLargestPossibleRegion().GetSize());
	resampleFilter->Update();

	io::print("Scale image", 1);
	return resampleFilter->GetOutput();

}
template<typename imageType>
typename imageType::Pointer changeCanvasImageSize(typename imageType::Pointer inputImage, unsigned outputWith, unsigned outputHeight)
{

	typename imageType::Pointer outputImage = imageType::New();
	typename imageType::RegionType outputRegion;
	typename imageType::IndexType outputIndex, outputUpperIndex;
	outputIndex[0] = 0;
	outputIndex[1] = 0;
	outputUpperIndex[0] = static_cast<unsigned>(static_cast<signed>(outputWith) - 1);
	outputUpperIndex[1] = static_cast<unsigned>(static_cast<signed>(outputHeight) - 1);

	outputRegion.SetIndex(outputIndex);
	outputRegion.SetUpperIndex(outputUpperIndex);

	outputImage->SetRegions(outputRegion);
	outputImage->SetDirection(inputImage->GetDirection());
	outputImage->SetSpacing(inputImage->GetSpacing());
	outputImage->SetOrigin(inputImage->GetOrigin());

	outputImage->Allocate();

	outputImage->FillBuffer(itk::NumericTraits<typename imageType::PixelType>::Zero);

	typedef itk::PasteImageFilter<imageType, imageType> FilterType;
	typename FilterType::Pointer filter = FilterType::New();
	filter->SetSourceImage(inputImage);
	filter->SetSourceRegion(inputImage->GetLargestPossibleRegion());
	filter->SetDestinationImage(outputImage);
	filter->SetDestinationIndex(outputIndex);
	/*
	 //center of gravity
	 typedef itk::ImageMomentsCalculator<TYPE::grayScaleImageType> imageCalculatorType;
	 imageCalculatorType::Pointer calculator = imageCalculatorType::New();
	 calculator->SetImage(inputImage);
	 calculator->Compute();
	 imageCalculatorType::VectorType fixedCenter = calculator->GetCenterOfGravity();
	 //int inputWithCenter = fixedCenter[0];
	 //int inputHeightCenter = fixedCenter[1];
	 */
	int inputWithCenter = inputImage->GetLargestPossibleRegion().GetSize()[0] / 2;
	//int inputHeightCenter = inputImage->GetLargestPossibleRegion().GetSize()[1] / 2;

	type::grayImageType::IndexType destinationIndex;
	destinationIndex[0] = (outputWith / 2) - inputWithCenter;
	destinationIndex[1] = 0;	// (outputHeight / 2) - inputHeightCenter;
	filter->SetDestinationIndex(destinationIndex);

	filter->Update();

	return filter->GetOutput();

}

template<typename imageType>
typename imageType::Pointer otsuThreshold(typename imageType::Pointer inputImage, int insideValue = 0, int outsideValue = 1)
{

	typedef itk::OtsuThresholdImageFilter<imageType, imageType> FilterType;
	typename FilterType::Pointer otsuFilter = FilterType::New();
	otsuFilter->SetInput(inputImage);
	otsuFilter->SetInsideValue(insideValue); //0
	otsuFilter->SetOutsideValue(outsideValue); //1
	otsuFilter->SetNumberOfHistogramBins(10);

	otsuFilter->Update(); // To compute threshold

	//IO::print("Otsu Threshold", 1);

	return otsuFilter->GetOutput();

}

template<typename imageType>
typename imageType::Pointer neighborhoodConnectedFilter(typename imageType::Pointer inputImage, typename imageType::IndexType seed, int insideValue = 0, int outsideValue = 1, unsigned radius = 2)
{

	typedef itk::NeighborhoodConnectedImageFilter<imageType, imageType> ConnectedFilterType;
	typename ConnectedFilterType::Pointer neighborhoodConnected = ConnectedFilterType::New();

	typename imageType::SizeType radiusXYZ;
	radiusXYZ[0] = 2;   // two pixels along X
	radiusXYZ[1] = 2;   // two pixels along Y
	radiusXYZ[2] = 2;   // two pixels along Z

	neighborhoodConnected->SetRadius(radiusXYZ);

	neighborhoodConnected->SetSeed(seed);
	neighborhoodConnected->SetReplaceValue(1);

	neighborhoodConnected->SetInput(inputImage);

	neighborhoodConnected->SetLower(30);
	neighborhoodConnected->SetUpper(255);

	neighborhoodConnected->Update();

	return neighborhoodConnected->GetOutput();

}

type::meshTypePointer extractIsoSurface(type::grayImagePointer3D inputImage, int objectValue)
{

	typedef itk::BinaryMask3DMeshSource<type::grayImageType3D, type::meshType> FilterType;
	FilterType::Pointer filter = FilterType::New();
	filter->SetObjectValue(objectValue);
	filter->SetInput(inputImage);
	filter->Update();

	io::print("Mesh extraction", 1);

	return filter->GetOutput();

}
template<typename imageType>
typename imageType::Pointer createImageByInterpolation(typename imageType::Pointer image1, typename imageType::Pointer image2, double position = 1.0)
{

	typename imageType::Pointer outputImage = imageType::New();
	outputImage->SetRegions(image1->GetLargestPossibleRegion());
	outputImage->SetOrigin(image1->GetOrigin());
	outputImage->SetSpacing(image1->GetSpacing());
	outputImage->SetDirection(image1->GetDirection());

	outputImage->Allocate();

	itk::ImageRegionIterator < imageType > it1(image1, outputImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator < imageType > it2(image2, outputImage->GetLargestPossibleRegion());
	itk::ImageRegionIterator < imageType > itOut(outputImage, outputImage->GetLargestPossibleRegion());

	it1.GoToBegin();
	it2.GoToBegin();
	itOut.GoToBegin();

	for (; !it1.IsAtEnd(); ++it1, ++it2, ++itOut)
	{

		itOut.Set(static_cast<typename imageType::PixelType>(math::linearInterpolator<typename imageType::PixelType>(it1.Get(), it2.Get(), position)));

	}

	return outputImage;

}

template<typename imageType>
typename imageType::Pointer histogramThreshold(typename imageType::Pointer inputImage, const std::string& algorithmName = "otsu", unsigned bins = 100, unsigned insideValue = 255,
		unsigned outsideValue = 0)
{

	typename itk::HistogramThresholdImageFilter<imageType, imageType>::Pointer filter;

	if (algorithmName == "otsu")
	{
		filter = itk::OtsuThresholdImageFilter<imageType, imageType>::New();

	}
	if (algorithmName == "huang")
	{
		filter = itk::HuangThresholdImageFilter<imageType, imageType>::New();

	}
	if (algorithmName == "intermodes")
	{
		filter = itk::IntermodesThresholdImageFilter<imageType, imageType>::New();

	}

	if (algorithmName == "isoData")
	{
		filter = itk::IsoDataThresholdImageFilter<imageType, imageType>::New();

	}
	if (algorithmName == "li")
	{
		filter = itk::LiThresholdImageFilter<imageType, imageType>::New();

	}
	if (algorithmName == "maximumEntropy")
	{
		filter = itk::MaximumEntropyThresholdImageFilter<imageType, imageType>::New();

	}
	if (algorithmName == "moments")
	{
		filter = itk::MomentsThresholdImageFilter<imageType, imageType>::New();

	}
	if (algorithmName == "renyi")
	{
		filter = itk::RenyiEntropyThresholdImageFilter<imageType, imageType>::New();

	}
	if (algorithmName == "shanbhag")
	{
		filter = itk::ShanbhagThresholdImageFilter<imageType, imageType>::New();

	}
	if (algorithmName == "triangle")
	{
		filter = itk::TriangleThresholdImageFilter<imageType, imageType>::New();

	}
	if (algorithmName == "yen")
	{
		filter = itk::YenThresholdImageFilter<imageType, imageType>::New();

	}

	filter->SetInput(inputImage);

	filter->SetInsideValue(insideValue); //255
	filter->SetOutsideValue(outsideValue); //0

	filter->SetNumberOfHistogramBins(bins);

	filter->Update();

	return filter->GetOutput();

}

template<typename imageType>
typename imageType::Pointer connectedThresholdFilter(typename imageType::Pointer inputImage, typename imageType::IndexType seed, int insideValue = 0, int outsideValue = 1, unsigned radius = 2)
{

	using InternalPixelType = float;
	constexpr unsigned int Dimension = imageType::ImageDimension;
	using InternalImageType = itk::Image< InternalPixelType, Dimension >;

	typedef itk::CurvatureFlowImageFilter<imageType, InternalImageType> CurvatureFlowImageFilterType;
	typename CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();
	smoothing->SetInput(inputImage);
	smoothing->SetNumberOfIterations(1);
	smoothing->SetTimeStep(0.125);

	//typedef itk::ConnectedThresholdImageFilter<InternalImageType, InternalImageType> ConnectedFilterType;
	typedef itk::ConnectedThresholdImageFilter<imageType, imageType> ConnectedFilterType;
	typename ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
	connectedThreshold->SetInput(inputImage);
	connectedThreshold->SetLower(90);
	connectedThreshold->SetUpper(255);
	connectedThreshold->SetReplaceValue(1);
	connectedThreshold->SetSeed(seed);

	typedef itk::CastImageFilter<InternalImageType, imageType> CastingFilterType;
	typename CastingFilterType::Pointer caster = CastingFilterType::New();

	//caster->SetInput(connectedThreshold->GetOutput());

	//caster->Update();
	//return caster->GetOutput();

	connectedThreshold->Update();

	//io::print("Connected Threshold Filter", 1);

	return connectedThreshold->GetOutput();

}

template<typename imageType>
itk::CylinderSpatialObject::Pointer createCylinderObject(typename imageType::Pointer image, double radius, double height)
{
	typedef itk::CylinderSpatialObject CylinderType;

	typedef itk::SpatialObjectToImageFilter<CylinderType, imageType> SpatialObjectToImageFilterType;

	typename SpatialObjectToImageFilterType::Pointer imageFilter = SpatialObjectToImageFilterType::New();

	typename imageType::SizeType size = image->GetLargestPossibleRegion().GetSize();

	imageFilter->SetSize(size);

	CylinderType::Pointer myCylinder = CylinderType::New();

	myCylinder->SetRadius(radius);
	myCylinder->SetHeight(height);
	myCylinder->SetDefaultInsideValue(1);
	myCylinder->SetDefaultOutsideValue(0);
	//myCylinder->IsInside()

	typedef CylinderType::TransformType TransformType;

	TransformType::Pointer transform = TransformType::New();

	transform->SetIdentity();

	TransformType::OutputVectorType translation;

	translation[0] = size[0] / 2;
	translation[1] = size[1] / 2;
	translation[2] = size[2] / 2;

	transform->Rotate(1, 2, itk::Math::pi / 2.0);

	transform->Translate(translation, false);

	myCylinder->SetObjectToParentTransform(transform);

	myCylinder->Update();
	io::print("Create cylinder", 1);
	return myCylinder;

}

template<typename imageType>
void cylindricalFilter(typename imageType::Pointer inputImage, double radious)
{

	itk::CylinderSpatialObject::Pointer cylinder = createCylinderObject<imageType>(inputImage, radious, 300);

	itk::ImageRegionIterator < imageType > it(inputImage, inputImage->GetLargestPossibleRegion());

	for (it.GoToBegin(); !it.IsAtEnd(); ++it)
	{
		itk::Point<unsigned, 3> point;
		point[0] = it.GetIndex()[0];
		point[1] = it.GetIndex()[1];
		point[2] = it.GetIndex()[2];

		if (cylinder->IsInside(point))
		{

			it.Set((it.Get() == 1) ? 1 : 0);
		}
		else
		{
			it.Set(0);

		}

	}

	io::print("Cylinder filter", 1);

}

template<typename inputType, typename labelMapType>
typename labelMapType::Pointer connectedComponents(typename inputType::Pointer image, std::string outputFileName = "")
{

	typedef itk::ConnectedComponentImageFilter<inputType, inputType> ConnectedComponentImageFilterType;

	//typedef itk::ConnectedComponentImageFilter<TYPE::grayScaleType, TYPE::grayScaleType> ConnectedComponentImageFilterType;

	typename ConnectedComponentImageFilterType::Pointer connected = ConnectedComponentImageFilterType::New();
	connected->SetInput(image);
	connected->Update();

	std::cout << "Number of objects: " << connected->GetObjectCount() << std::endl;

	typedef itk::LabelImageToLabelMapFilter<inputType> LabelImageToLabelMapFilterType;
	typename LabelImageToLabelMapFilterType::Pointer labelImageToLabelMapFilter = LabelImageToLabelMapFilterType::New();
	labelImageToLabelMapFilter->SetInput(connected->GetOutput());
	labelImageToLabelMapFilter->Update();

	typedef itk::Image<type::rgbType, 3> rgbImageType;
	typedef itk::LabelToRGBImageFilter<inputType, rgbImageType> RGBFilterType;
	typename RGBFilterType::Pointer rgbFilter = RGBFilterType::New();
	rgbFilter->SetInput(connected->GetOutput());

	if (outputFileName != "")
	{
		io::writeImage < rgbImageType > (rgbFilter->GetOutput(), outputFileName);
	}

	io::print("Connected Components", 1);

	return labelImageToLabelMapFilter->GetOutput();

}
//binary images
template<typename imageType>
void invertPixelValue(typename imageType::Pointer inputImage)
{

	itk::ImageRegionIterator < imageType > it(inputImage, inputImage->GetLargestPossibleRegion());

	for (it.GoToBegin(); !it.IsAtEnd(); ++it)
	{
		it.Set((it.Get() == 0) ? 1 : 0);

	}

	io::print("Inverting value", 1);

}

void computeLabelMapStatistics(type::labelMapType3D::Pointer labelMap, std::string outputFileName)
{

	std::ofstream outputFile(outputFileName);

	outputFile << "Number of Objects " << labelMap->GetNumberOfLabelObjects() << std::endl;

	outputFile << "ID, " << "Max Diameter, " << "Max Height, " << "Volume, " << "Position" << std::endl;

	for (unsigned i = 0; i < labelMap->GetNumberOfLabelObjects(); ++i)
	{

		outputFile << i + 1 << ", ";

		int pitPercentage = math::to_percentage<int>(i, labelMap->GetNumberOfLabelObjects());

		std::cout << "Computing metrics " << pitPercentage << "%" << std::endl; //<<std::flush;

		unsigned size = labelMap->GetNthLabelObject(i)->Size();

		//computing centroid and bounding box

		double xMean = 0;
		double yMean = 0;
		double zMean = 0;

		std::vector<long long> min(3, 9000000);
		std::vector<long long> max(3, 0);

		long long xTmp;
		long long yTmp;
		long long zTmp;

		int partialPitPercentage = -1;
		int tmpPercentage = 0;

		type::labelObjectType3D::Pointer labelObject = labelMap->GetNthLabelObject(i);

		type::labelObjectType3D::IndexType index;

		for (unsigned j = 0; j < size; ++j)
		{

			index = labelObject->GetIndex(j);
			xTmp = index[0];
			yTmp = index[1];
			zTmp = index[2];

			//xTmp = labelMap->GetNthLabelObject(i)->GetIndex(j)[0];
			//yTmp = labelMap->GetNthLabelObject(i)->GetIndex(j)[1];
			//zTmp = labelMap->GetNthLabelObject(i)->GetIndex(j)[2];

			/*xMean += xTmp;
			 yMean += yTmp;
			 zMean += zTmp;*/

			xMean = math::timeAverage(xTmp, j + 1, xMean);
			yMean = math::timeAverage(yTmp, j + 1, yMean);
			zMean = math::timeAverage(zTmp, j + 1, zMean);

			min[0] = (xTmp < min[0]) ? xTmp : min[0];
			min[1] = (yTmp < min[1]) ? yTmp : min[1];
			min[2] = (zTmp < min[2]) ? zTmp : min[2];

			max[0] = (xTmp > max[0]) ? xTmp : max[0];
			max[1] = (yTmp > max[1]) ? yTmp : max[1];
			max[2] = (zTmp > max[2]) ? zTmp : max[2];

			tmpPercentage = math::to_percentage<int>(j + 1, size);
			if (partialPitPercentage < tmpPercentage)
			{
				partialPitPercentage = tmpPercentage;
				std::cout << "Partial pit " << partialPitPercentage << "%" << " of " << pitPercentage << "%" << std::endl;
			}
		}

		outputFile << ((std::abs(max[0] - min[0]) > std::abs(max[2] - min[2])) ? std::abs(max[0] - min[0]) : std::abs(max[2] - min[2])) << ", ";
		outputFile << std::abs(max[1] - min[1]) << ", ";

		outputFile << labelMap->GetNthLabelObject(i)->Size() << ", ";

		//std::cout << "[" << std::round(xMean) << "- " << std::round(yMean) << "- " << std::round(zMean) << "]" << std::endl;
		outputFile << "[" << std::round(xMean) << "- " << std::round(yMean) << "- " << std::round(zMean) << "]" << std::endl;

	}

	outputFile.close();

	io::print("Statistics 3D", 1);

}

void computeLabelMapStatisticsThread(const type::labelMapType3D::Pointer& labelMap, std::vector<std::string>& metrics, unsigned lowerIndex, unsigned upperIndex, unsigned threadNumber)
{

	//auxiliary variables
	std::stringstream lineMetric;
	int pitPercentage = 0;
	unsigned objectSize = 0;
	//unsigned numberOfObjects = labelMap->GetNumberOfLabelObjects();
	int tmpPercentage = 0;

	std::cout << "lower " << lowerIndex << " upper " << upperIndex << std::endl;
	for (unsigned i = lowerIndex; i < upperIndex; ++i)
	{
		lineMetric << i + 1 << ", ";

		//echo
		tmpPercentage = math::to_percentage<int>(static_cast<int>(i - lowerIndex), static_cast<int>(upperIndex - lowerIndex));
		if (pitPercentage < tmpPercentage)
		{
			pitPercentage = tmpPercentage;
			//std::cout << "Metrics 3D thread " << threadNumber + 1 << ": " << pitPercentage << "%" << std::endl; //<<std::flush;
		}

		objectSize = labelMap->GetNthLabelObject(i)->Size();

		//computing centroid and bounding box

		double xMean = 0;
		double yMean = 0;
		double zMean = 0;

		std::vector<int> min(3, 100000000);
		std::vector<int> max(3, 0);

		long long int xTmp;
		long long int yTmp;
		long long int zTmp;

		int partialPitPercentage = -1;

		type::labelObjectType3D::Pointer labelObject = labelMap->GetNthLabelObject(i);

		type::labelObjectType3D::IndexType index;

		for (long unsigned j = 0; j < objectSize; ++j)
		{

			index = labelObject->GetIndex(j);
			xTmp = index[0];
			yTmp = index[1];
			zTmp = index[2];

			/*xMean += xTmp;
			 yMean += yTmp;
			 zMean += zTmp;*/

			xMean = math::timeAverage(xTmp, j + 1, xMean);
			yMean = math::timeAverage(yTmp, j + 1, yMean);
			zMean = math::timeAverage(zTmp, j + 1, zMean);

			min[0] = (xTmp < min[0]) ? xTmp : min[0];
			min[1] = (yTmp < min[1]) ? yTmp : min[1];
			min[2] = (zTmp < min[2]) ? zTmp : min[2];

			max[0] = (xTmp > max[0]) ? xTmp : max[0];
			max[1] = (yTmp > max[1]) ? yTmp : max[1];
			max[2] = (zTmp > max[2]) ? zTmp : max[2];
			/*
			 tmpPercentage = math::to_percentage<int>(j + 1, objectSize);
			 if (partialPitPercentage < tmpPercentage)
			 {
			 partialPitPercentage = tmpPercentage;
			 std::cout <<"Thread "<< threadNumber<< ": Partial pit " << partialPitPercentage << "%" << " of " << pitPercentage << "%" << std::endl;
			 }*/
		}

		lineMetric << ((std::abs(max[0] - min[0]) > std::abs(max[2] - min[2])) ? std::abs(max[0] - min[0]) : std::abs(max[2] - min[2])) << ", ";
		lineMetric << std::abs(max[1] - min[1]) << ", ";

		//lineMetric << labelMap->GetNthLabelObject(i)->Size() << ", ";
		lineMetric << objectSize << ", ";

		//lineMetric << "[" << xMean / objectSize << "- " << yMean / objectSize << "- " << zMean / objectSize << "] thread" << threadNumber << std::endl;
		lineMetric << "[" << xMean << "- " << yMean << "- " << zMean << "] thread" << threadNumber << std::endl;

		metrics[i] = lineMetric.str();
		//std::cout<<metrics[i]<<std::endl;

	}

	std::cout << "Thread " << threadNumber + 1 << ": end" << std::endl;

}

void computeLabelMapStatisticsMuiltiThread(type::labelMapType3D::Pointer labelMap, std::string outputFileName)
{

	std::vector<std::string> metrics(labelMap->GetNumberOfLabelObjects());

	const unsigned numberOfThreads = 3;
	std::thread threads[numberOfThreads];

	unsigned step = metrics.size() / numberOfThreads;

	//some auxiliary variables
	int incrementControl = metrics.size();
	unsigned pivot = 0;
	unsigned lowerIndex = 0;
	unsigned partialSize = 0;

	std::cout << "Computing Metrics using: " << numberOfThreads << " threads" << std::endl;

	for (unsigned i = 0; i < numberOfThreads; ++i)
	{

		if (step < incrementControl)
		{
			lowerIndex = pivot;
			pivot += step;
			partialSize = pivot;
			incrementControl -= step;
			threads[i] = std::thread(computeLabelMapStatisticsThread, std::cref(labelMap), std::ref(metrics), lowerIndex, partialSize, i);

			std::cout << lowerIndex << " " << partialSize << std::endl;

		}
		else
		{
			lowerIndex = pivot;
			partialSize = metrics.size();
			threads[i] = std::thread(computeLabelMapStatisticsThread, std::cref(labelMap), std::ref(metrics), lowerIndex, partialSize, i);

			std::cout << lowerIndex << " last " << partialSize << std::endl;
		}

	}

	for (unsigned i = 0; i < numberOfThreads; ++i)
	{

		threads[i].join();

	}

	std::ofstream outputFile(outputFileName);

	outputFile << "Number of Objects " << labelMap->GetNumberOfLabelObjects() << std::endl;

	outputFile << "ID, " << "Max Diameter, " << "Max Height, " << "Volume, " << "Position" << std::endl;
	for (unsigned i = 0; i < metrics.size(); ++i)
	{

		std::cout << metrics[i] << std::endl;
		outputFile << metrics[i];
	}

	io::print("Metrics 3D", true);

}

}

#endif /* SRC_COMMON_IMAGEPROCESSING_H_ */
