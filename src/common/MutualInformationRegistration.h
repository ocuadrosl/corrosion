/*
 * MutualInformationRegistration.h
 *
 *  Created on: Jan 5, 2018
 *      Author: oscar
 */

#ifndef SRC_COMMON_MUTUALINFORMATIONREGISTRATION_H_
#define SRC_COMMON_MUTUALINFORMATIONREGISTRATION_H_

#include "Types.h"
#include "itkImageRegistrationMethodv4.h"
#include "itkTranslationTransform.h"
#include "itkMattesMutualInformationImageToImageMetricv4.h"
#include "itkRegularStepGradientDescentOptimizerv4.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCheckerBoardImageFilter.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"

#include "InputOutput.h"

namespace ip
{

type::grayImagePointer mutualInformationRegistration(type::grayFloatImagePointer fixedImage, type::grayFloatImagePointer movingImage)
{

	typedef itk::TranslationTransform<double, 2> TransformType;
	typedef itk::RegularStepGradientDescentOptimizerv4<double> OptimizerType;
	typedef itk::ImageRegistrationMethodv4<type::grayFloatImageType, type::grayFloatImageType, TransformType> RegistrationType;

	typedef itk::MattesMutualInformationImageToImageMetricv4<type::grayFloatImageType, type::grayFloatImageType> MetricType;

	OptimizerType::Pointer optimizer = OptimizerType::New();
	RegistrationType::Pointer registration = RegistrationType::New();
	registration->SetOptimizer(optimizer);

	MetricType::Pointer metric = MetricType::New();
	registration->SetMetric(metric);

	unsigned int numberOfBins = 24;
	metric->SetNumberOfHistogramBins(numberOfBins);
	metric->SetUseMovingImageGradientFilter(false);
	metric->SetUseFixedImageGradientFilter(false);

	registration->SetFixedImage(fixedImage);
	registration->SetMovingImage(movingImage);

	optimizer->SetLearningRate(8.00);
	optimizer->SetMinimumStepLength(0.001);
	optimizer->SetNumberOfIterations(200);
	optimizer->ReturnBestParametersAndValueOn();

	optimizer->SetRelaxationFactor(0.8);

	const unsigned int numberOfLevels = 1;
	RegistrationType::ShrinkFactorsArrayType shrinkFactorsPerLevel;
	shrinkFactorsPerLevel.SetSize(1);
	shrinkFactorsPerLevel[0] = 1;
	RegistrationType::SmoothingSigmasArrayType smoothingSigmasPerLevel;
	smoothingSigmasPerLevel.SetSize(1);
	smoothingSigmasPerLevel[0] = 0;
	registration->SetNumberOfLevels(numberOfLevels);
	registration->SetSmoothingSigmasPerLevel(smoothingSigmasPerLevel);
	registration->SetShrinkFactorsPerLevel(shrinkFactorsPerLevel);

	RegistrationType::MetricSamplingStrategyType samplingStrategy = RegistrationType::RANDOM;

	double samplingPercentage = 0.20;
	registration->SetMetricSamplingStrategy(samplingStrategy);
	registration->SetMetricSamplingPercentage(samplingPercentage);
	registration->MetricSamplingReinitializeSeed(121213);

	try
	{
		registration->Update();
		//std::cout << "Optimizer stop condition: " << registration->GetOptimizer()->GetStopConditionDescription() << std::endl;
	} catch (itk::ExceptionObject & err)
	{
		std::cerr << "ExceptionObject caught !" << std::endl;
		std::cerr << err << std::endl;

	}

	TransformType::ParametersType finalParameters = registration->GetOutput()->Get()->GetParameters();
	double TranslationAlongX = finalParameters[0];
	double TranslationAlongY = finalParameters[1];

	unsigned int numberOfIterations = optimizer->GetCurrentIteration();
	double bestValue = optimizer->GetValue();
	// Print out results
	//
/*	std::cout << std::endl;
	std::cout << "Result = " << std::endl;
	std::cout << " Translation X = " << TranslationAlongX << std::endl;
	std::cout << " Translation Y = " << TranslationAlongY << std::endl;
	std::cout << " Iterations    = " << numberOfIterations << std::endl;
	std::cout << " Metric value  = " << bestValue << std::endl;
	std::cout << " Stop Condition  = " << optimizer->GetStopConditionDescription() << std::endl;*/

	typedef itk::ResampleImageFilter<type::grayFloatImageType, type::grayFloatImageType> ResampleFilterType;
	ResampleFilterType::Pointer resample = ResampleFilterType::New();
	resample->SetTransform(registration->GetTransform());
	resample->SetInput(movingImage);

	float defaultPixelValue = 0;

	resample->SetSize(fixedImage->GetLargestPossibleRegion().GetSize());
	resample->SetOutputOrigin(fixedImage->GetOrigin());
	resample->SetOutputSpacing(fixedImage->GetSpacing());
	resample->SetOutputDirection(fixedImage->GetDirection());
	resample->SetDefaultPixelValue(defaultPixelValue);
	resample->Update();

	//io::print("Mutual Information Registration", 1);

	return io::imageCast<type::grayFloatImageType, type::grayImageType>(resample->GetOutput());



}

}

#endif /* SRC_COMMON_MUTUALINFORMATIONREGISTRATION_H_ */
