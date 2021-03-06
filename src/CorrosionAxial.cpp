//============================================================================
// Name        : CorrosionAxial.cpp
// Author      : Oscar Cuadros Linares
// Version     :
// Copyright   : 
// Description : 
//============================================================================

#include <iostream>
#include "Image3D.h"
#include "common/InputOutput.h"
#include "common/ImageProcessing.h"
#include "Interface.h"
#include "common/Utils.h"
#include "itkLabelMap.h"

int main(int argc, const char* argv[])
{

	std::string inputFile;
	if (argc == 2)
	{
		inputFile = argv[1];
	}
	else
	{
		//inputFile = "input/electrode5_Imagens_tratadas/electrode5Tratadas.txt";
		std::cerr << "ERROR: Configuration file is required" << std::endl;
		return 0;
	}

	Interface interface(inputFile);
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

	//writing images
	io::writeImage<type::grayImageType3D>(outputImage, interface.getOutputDir() + "/" + interface.getTestName() + ".nii");
	io::writeImage<type::grayImageType3D>(outputSegmentedImage, interface.getOutputDir() + "/segmented" + interface.getTestName() + ".nii");

	//creating and writing mesh
	/*std::string vtkName = interface.getOutputDir() + "/" + interface.getTestName() + ".vtk";
	type::meshTypePointer mesh = ip::extractIsoSurface(outputSegmentedImage, 0);
	io::writeMesh(mesh, vtkName);*/

	//computing metrics
	if (interface.getStatistics3D())
	{

		//computing connected components
		typedef itk::LabelObject<type::grayImageType3D::PixelType, 3> labelObjectType3D;
		typedef itk::LabelMap<labelObjectType3D> labelMapType3D;

		//connected component
		std::string rgbName = interface.getOutputDir() + "/" + interface.getTestName() + "rgb.nii";
		typename labelMapType3D::Pointer labelMap3D = ip::connectedComponents<type::grayImageType3D, labelMapType3D>(outputSegmentedImage, rgbName);

		ip::computeLabelMapStatistics(labelMap3D, interface.getOutputDir() + "/" + interface.getTestName() + "Metrics.csv");

	}

	if (interface.getStatistics2D())
	{

		//computing connected components slice by slice
		utils::computeStatistics2D(image3D.getImageSeries(), interface.getOutputDir() + "/" + interface.getTestName() + "MetricsBySlice.csv");

	}

	if (interface.getStatistics2DInput())
	{
		utils::computeStatistics2D(image3D.getInputImageSeries(), interface.getOutputDir() + "/" + interface.getTestName() + "MetricsBySliceInput.csv");
	}

//	std::cout << "DONE" << std::endl;

	std::cout<<"Trying to create a VTK mesh, it may fail due to the input image resolution"<<std::endl;
	std::cout<<"Notice all volumes and metrics were already created and computed"<<std::endl;

	 //creating and writing mesh
        std::string vtkName = interface.getOutputDir() + "/" + interface.getTestName() + ".vtk";
        type::meshTypePointer mesh = ip::extractIsoSurface(outputSegmentedImage, 0);
        io::writeMesh(mesh, vtkName);


	std::cout<<"DONE"<<std::endl;







	return 0;
}
