/*
 * Types.h
 *
 *  Created on: Jan 5, 2018
 *      Author: oscar
 */

#ifndef SRC_COMMON_TYPES_H_
#define SRC_COMMON_TYPES_H_

#include "itkImage.h"
#include "itkBinaryMask3DMeshSource.h"
#include "itkLabelImageToLabelMapFilter.h"

#include "itkLabelMap.h"

/*
 *Common types
 *
 * */

const unsigned short DIMENSION_2 = 2;
const unsigned short DIMENSION_3 = 3;

namespace type
{
typedef unsigned int grayType;

typedef itk::Image<grayType, 2> grayImageType;
typedef grayImageType::Pointer grayImagePointer;

typedef itk::Image<grayType, 3> grayImageType3D;
typedef grayImageType3D::Pointer grayImagePointer3D;

typedef unsigned char componentType;
typedef itk::RGBPixel<componentType> rgbType;
typedef itk::Image<rgbType, DIMENSION_2> rgbImageType;
typedef rgbImageType::Pointer rgbImagePointer;

typedef itk::Image<float, 2> grayFloatImageType;
typedef grayFloatImageType::Pointer grayFloatImagePointer;

//mesh
typedef itk::Mesh<double, 3> meshType;
typedef meshType::Pointer meshTypePointer;

typedef itk::LabelObject<grayImageType::PixelType, 3> labelObjectType3D;
typedef itk::LabelMap<labelObjectType3D> labelMapType3D;

}

#endif /* SRC_COMMON_TYPES_H_ */
