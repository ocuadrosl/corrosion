//============================================================================
// Name        : CorrosionAxial.cpp
// Author      : Oscar
// Version     :
// Copyright   : 
// Description : 
//============================================================================

#include <iostream>
#include "Image3D.h"
#include "common/InputOutput.h"
#include "common/ImageProcessing.h"
#include "Interface.h"

#include "itkLabelMap.h"

int main(int argc, const char* argv[])
{

	std::string inputFile;
	if(argc == 2)
	{		
		std::string inputFile = argv[1];
		
	}
	else
	{
		std::string inputFile = "input/electrode5_Imagens_tratadas/electrode5Tratadas.txt";
	}

	Interface interface("input/eletrodo5_Imagens_tratadas/electrode5Tratadas.txt");
	interface.readInputFile();

	Image3D image3D;
	image3D.readImageSeries(interface.getInputDir(), "", interface.getImageFormat());
	image3D.setFixedImage(interface.getFixedImageName());
	image3D.setBreakPoint(interface.getBreakPoint());
	image3D.setReducedRadius(interface.getReducedRadius());

//setting parameters
	image3D.setConversionFactor(interface.getConversionFactor()); // 1866.876435473  4420*0.25 (due to image size reduction)
	image3D.setAbradedHeightByLayer(interface.getAbradedHeightByLayer());

	image3D.createImage3D();

	type::grayImagePointer3D outputImage = image3D.getImage3D();
	type::grayImagePointer3D outputSegmentedImage = image3D.getSegmentedImage3D();

	io::writeImage<type::grayImageType3D>(outputImage, interface.getOutputDir() + "/" + interface.getTestName() + ".nii");
	io::writeImage<type::grayImageType3D>(outputSegmentedImage, interface.getOutputDir() + "/segmented" + interface.getTestName() + ".nii");


//reading volume tmp
//type::grayImagePointer3D outputImage = io::readImage<type::grayImageType3D>("output/volume.nii");

//type::grayImagePointer3D segmentedImage = ip::otsuThreshold<type::grayImageType3D>(outputImage);

//image segmentation
	/*type::grayImageType3D::IndexType seed;

	 seed[0] = 800;
	 seed[1] = 800;
	 seed[2] = 100;

	 //type::grayImagePointer3D neighborhoodImage = ip::neighborhoodConnectedFilter<type::grayImageType3D>(outputImage, seed);
	 type::grayImagePointer3D segmentedImage = ip::connectedThresholdFilter<type::grayImageType3D>(outputImage, seed);
	 */

//ip::invertPixelValue<type::grayImageType3D>(segmentedImage); //by reference
//cylindrical filter
//ip::cylindricalFilter<type::grayImageType3D>(segmentedImage, 930); //by reference...
//computing connected components
	typedef itk::LabelObject<type::grayImageType3D::PixelType, 3> labelObjectType3D;
	typedef itk::LabelMap<labelObjectType3D> labelMapType3D;

//connected component
	std::string rgbName = interface.getOutputDir() + "/" + interface.getTestName() + "rgb.nii";
	labelMapType3D::Pointer labelMap3D = ip::connectedComponents<type::grayImageType3D, labelMapType3D>(outputSegmentedImage, rgbName);

	ip::computeLabelMapStatistics(labelMap3D, interface.getOutputDir() + "/" + interface.getTestName()+"Metrics.txt" );

	
	

	//creating mesh
	std::string vtkName = interface.getOutputDir() + "/" + interface.getTestName() + ".vtk";
	type::meshTypePointer mesh = ip::extractIsoSurface(outputSegmentedImage, 1);
	io::writeMesh(mesh, vtkName);

	std::cout << "DONE" << std::endl;

	return 0;
}
