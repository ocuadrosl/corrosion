/*
 * Interface.h
 *
 *  Created on: Jul 23, 2018
 *      Author: oscar
 */

#ifndef SRC_INTERFACE_H_
#define SRC_INTERFACE_H_

class Interface
{
public:
	Interface(std::string inputFileName);
	~Interface()
	{
	}
	;
	//getters
	std::string getInputFileName() const;
	std::string getFixedImageName() const;
	double getConversionFactor() const;
	std::string getInputDir() const;
	std::string getOutputDir() const;
	std::vector<double> getAbradedHeightByLayer() const;
	std::string getImageFormat() const;
	std::string getTestName() const;
	int getBreakPoint() const;

	void readInputFile();
	void readInputFileOld();

private:
	std::string inputFileName;
	std::string fixedImage;
	double conversionFactor;
	std::string inputDir; //input images directory
	std::string outputDir;
	std::string imageFormat;
	std::string testName;
	std::vector<double> abradedHeightByLayer;
	int breakPoint;

	void cases(std::string tagName, std::string value);

};

Interface::Interface(std::string inputFileName)
{
	this->inputFileName = inputFileName;
	this->fixedImage = "";
	this->conversionFactor = 1;
	this->inputDir = ".";
	this->outputDir = ".";
	this->imageFormat = "tif";
	this->testName = "corrosion";
	this->breakPoint = -1; //no break point

}

void Interface::cases(std::string tagName, std::string value)
{
	if (tagName == "fixedImage")
	{
		this->fixedImage = value;
		return;
	}
	if (tagName == "conversionFactor")
	{
		this->conversionFactor = std::stod(value);
		return;
	}
	if (tagName == "inputDir")
	{
		this->inputDir = value;
		return;
	}
	if (tagName == "outputDir")
	{
		this->outputDir = value;
		return;
	}
	if (tagName == "imageFormat")
	{
		this->imageFormat = value;
		return;
	}
	if (tagName == "testName")
	{
		this->testName = value;

	}
	if (tagName == "breakPoint")
	{
		this->breakPoint = std::stoi(value);
	}

}

void Interface::readInputFileOld()
{

	std::ifstream inputFile;

	inputFile.open(this->inputFileName.c_str(), std::ifstream::in);

	if (inputFile.is_open())
	{
		std::string tag;
		std::getline(inputFile, tag);
		std::string value;
		while (tag != "abradedHeightByLayer")
		{
			inputFile >> value;
			cases(tag, value);
			inputFile >> tag;
		}

		while (inputFile >> value)
		{
			abradedHeightByLayer.push_back(std::stod(value));
			std::cout << value << std::endl;
		}

	}
	else
	{
		std::cerr << "Corrosion file is not found" << std::endl;

	}

}

void Interface::readInputFile()
{

	std::ifstream inputFile;

	inputFile.open(this->inputFileName.c_str(), std::ifstream::in);

	if (inputFile.is_open())
	{
		std::istringstream line;
		std::string auxLine;
		std::string tag;
		std::string value;
		while (!inputFile.eof())
		{
			std::getline(inputFile, auxLine);

			if (auxLine.front() != '#')
			{
				line.str(auxLine);
				line >> tag;
				if (tag != "abradedHeightByLayer")
				{
					line >> value;
					std::cout << value << std::endl;
					cases(tag, value);
				}
				else
				{
					while (inputFile >> value)
					{
						abradedHeightByLayer.push_back(std::stod(value));
						std::cout << value << std::endl;
					}
				}
				line.clear();
			}

		}

	}
	else
	{
		std::cerr << "Corrosion file is not found" << std::endl;

	}

}

std::string Interface::getInputFileName() const
{
	return this->inputFileName;

}
std::string Interface::getFixedImageName() const
{
	return this->fixedImage;
}

double Interface::getConversionFactor() const
{
	return this->conversionFactor;

}

std::string Interface::getInputDir() const
{
	return this->inputDir;
}

std::string Interface::getOutputDir() const
{
	return this->outputDir;
}

std::vector<double> Interface::getAbradedHeightByLayer() const
{
	return this->abradedHeightByLayer;
}

std::string Interface::getImageFormat() const
{
	return this->imageFormat;
}
std::string Interface::getTestName() const
{
	return this->testName;
}
int Interface::getBreakPoint() const
{
	return this->breakPoint;
}

#endif /* SRC_INTERFACE_H_ */
