#ifndef UTILS_H_
#define UTILS_H_


#include "Types.h"
#include "itkLabelMap.h"



namespace utils
{

void computeStatistics2D( const  std::vector<type::grayImagePointer> & imageSeries, std::string outputFileName )
{

	std::ofstream outputFile(outputFileName);
 	
			
	typedef itk::LabelObject<type::grayImageType::PixelType, 2> labelObjectType;
        typedef itk::LabelMap<labelObjectType> labelMapType;
   
	
	labelMapType::Pointer labelMap;
	for(unsigned img=0 ; img < imageSeries.size(); ++img)
	{	
		 labelMap = ip::connectedComponents<type::grayImageType, labelMapType>(imageSeries[img]);

		//compute statistics here		
		outputFile << "Slice "<< img+1 <<" Number of Objects " << labelMap->GetNumberOfLabelObjects() << std::endl;
         
		outputFile << "ID, " << "Max Diameter, " << "Area, " << "Position" << std::endl;


		for (unsigned i = 1; i < labelMap->GetNumberOfLabelObjects(); ++i)
         	{
 			//id
                	outputFile<< i << ", ";
 
                 	int  pitPercentage = math::to_percentage<int>(i,  labelMap->GetNumberOfLabelObjects());
 
                 	std::cout << "Computing metrics " << pitPercentage << "%" << std::endl<<std::flush;
 
                 	unsigned size = labelMap->GetNthLabelObject(i)->Size();
 
                 	//computing centroid and bounding box
 
                 	int xMean = 0;
                 	int yMean = 0;
                 	
 
                 	std::vector<int> min(2, 1000);
                 	std::vector<int> max(2, 0);
 
                 	int xTmp;
                 	int yTmp;
                 	
        	   	int partialPitPercentage = -1;
	                int tmpPercentage = 0;
 
 
               		labelObjectType::Pointer labelObject = labelMap->GetNthLabelObject(i);
 
                 	labelObjectType::IndexType index;

			for (unsigned j = 0; j < size; ++j)
                 	{
 
                        	index = labelObject->GetIndex(j);
                         	xTmp = index[0];
                         	yTmp = index[1];
                          
                         	xMean += xTmp;
                         	yMean += yTmp;
                                min[0] = (xTmp < min[0]) ? xTmp : min[0];
                   	        min[1] = (yTmp < min[1]) ? yTmp : min[1];
                         	
 
                         	max[0] = (xTmp > max[0]) ? xTmp : max[0];
                         	max[1] = (yTmp > max[1]) ? yTmp : max[1];
                                tmpPercentage = math::to_percentage<int>(j+1, size);
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
        	        outputFile << "[" << xMean / size << "- " << yMean / size <<  "]" << std::endl;


  		}



	}
	outputFile.close();
	io::print("Statistics by slice", 1);



}


}














#endif
