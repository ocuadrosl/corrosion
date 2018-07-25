//============================================================================
// Name        : CorrosionAxial.cpp
// Author      : Oscar
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C, Ansi-style
//============================================================================

#include <iostream>
#include "Image3D.h"
#include "common/InputOutput.h"
#include "common/ImageProcessing.h"
#include "Interface.h"

#include "itkLabelMap.h"

int main(void)
{

	Interface interface("input/electrode5Tratadas.txt");
	interface.readInputFile();


	//type::grayImagePointer fixedImage = io::readImage<type::grayImageType>(interface.getFixedImageName());

	Image3D image3D;
	image3D.readImageSeries(interface.getInputDir(), "", interface.getImageFormat());
	//image3D.setFixedImage(fixedImage);
	image3D.setBreakPoint(interface.getBreakPoint());

	//setting parameters
	image3D.setConversionFactor(interface.getConversionFactor()); // 1866.876435473  4420*0.25 (due to image size reduction)
	image3D.setAbradedHeightByLayer(interface.getAbradedHeightByLayer());

	image3D.createImage3D();

	type::grayImagePointer3D outputImage = image3D.getImage3D();

	io::writeImage<type::grayImageType3D>(outputImage, interface.getOutputDir()+"/"+ interface.getTestName() +".nii");
	return 0;




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
	labelMapType3D::Pointer labelMap3D = ip::connectedComponents<type::grayImageType3D, labelMapType3D>(outputImage);

	//ip::computeLabelMapStatistics(labelMap3D);

	io::writeImage<type::grayImageType3D>(outputImage, "output/neighborhoodImage.nii");

	type::meshTypePointer mesh = ip::extractIsoSurface(outputImage, 1);
	io::writeMesh(mesh, "output/corrosion.vtk");

	std::cout << "DONE" << std::endl;

	return 0;
}
