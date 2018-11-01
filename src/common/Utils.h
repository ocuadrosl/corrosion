#ifndef UTILS_H_
#define UTILS_H_

#include "Types.h"
#include "itkLabelMap.h"

namespace utils
{

void computeStatistics2D(const std::vector<type::grayImagePointer> & imageSeries, std::string outputFileName)
{

	std::ofstream outputFile(outputFileName);

	typedef itk::LabelObject<type::grayImageType::PixelType, 2> labelObjectType;
	typedef itk::LabelMap<labelObjectType> labelMapType;

	labelMapType::Pointer labelMap;
	for (unsigned img = 0; img < imageSeries.size(); ++img)
	{
		labelMap = ip::connectedComponents<type::grayImageType, labelMapType>(imageSeries[img]);

		//compute statistics here		
		outputFile << "SLICE " << img + 1 << ": Number of Objects " << labelMap->GetNumberOfLabelObjects() << std::endl;

		outputFile << "Id, " << "Max Diameter, " << "Area, " << "Position" << std::endl;

		for (unsigned i = 0; i < labelMap->GetNumberOfLabelObjects(); ++i)
		{
			//id
			outputFile << i + 1 << ", ";

			int pitPercentage = math::to_percentage<int>(i, labelMap->GetNumberOfLabelObjects());

			std::cout << "Computing metrics by slice " << pitPercentage << "%" << std::endl << std::flush;

			unsigned size = labelMap->GetNthLabelObject(i)->Size();

			//computing centroid and bounding box

			double xMean = 0;
			double yMean = 0;

			std::vector<long long int> min(2, 10000000);
			std::vector<long long int> max(2, 0);

			long long int xTmp;
			long long int yTmp;

			int partialPitPercentage = -1;
			int tmpPercentage = 0;

			labelObjectType::Pointer labelObject = labelMap->GetNthLabelObject(i);

			labelObjectType::IndexType index;

			for (unsigned j = 0; j < size; ++j)
			{
				index = labelObject->GetIndex(j);
				xTmp = index[0];
				yTmp = index[1];

				//xMean += xTmp;
				//yMean += yTmp;
				xMean = math::timeAverage(xTmp, j + 1, xMean);
				yMean = math::timeAverage(yTmp, j + 1, yMean);

				min[0] = (xTmp < min[0]) ? xTmp : min[0];
				min[1] = (yTmp < min[1]) ? yTmp : min[1];

				max[0] = (xTmp > max[0]) ? xTmp : max[0];
				max[1] = (yTmp > max[1]) ? yTmp : max[1];
				tmpPercentage = math::to_percentage<int>(j + 1, size);
				//if(partialPitPercentage < tmpPercentage)
				///{
				////	partialPitPercentage = tmpPercentage;
				//	std::cout << "Partial pit 2D "<< partialPitPercentage << "%" <<" of "<<pitPercentage<<"%"<<std::endl;
				//}

			}

			//max diameter
			outputFile << ((std::abs(max[0] - min[0]) > std::abs(max[1] - min[1])) ? std::abs(max[0] - min[0]) : std::abs(max[1] - min[1])) << ", ";
			//outputFile << std::abs(max[1] - min[1]) << ", ";

			//volume
			outputFile << labelMap->GetNthLabelObject(i)->Size() << ", ";

			//position
			outputFile << "[" << std::round(xMean) << "- " << std::round(yMean) << "]" << std::endl;

		}
		std::cout << std::endl;

	}
	outputFile.close();
	io::print("Statistics by slice", 1);

}

}

#endif
