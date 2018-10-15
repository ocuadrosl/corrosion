/*
 * Math.h
 *
 *  Created on: Mar 6, 2018
 *      Author: oscar
 */

#ifndef SRC_COMMON_MATH_H_
#define SRC_COMMON_MATH_H_

namespace math
{
/*
 * factor = how many pixels are there in a millimeter.
 *
 * */
inline double millimetersToPixels(double millimeters, double factor)
{
	return millimeters * factor;
}

template<typename type>
inline double minMax(type valueIn, type originalMin, type originalMax, type newMin, type newMax)
{
	double den;

	den = (originalMax == originalMin) ? 0.000000001 : static_cast<double>(originalMax) - static_cast<double>(originalMin);

	return ((static_cast<double>(newMax) - static_cast<double>(newMin)) * (static_cast<double>(valueIn) - static_cast<double>(originalMin)) / den) + static_cast<double>(newMin);
}

template<typename type>
inline double linearInterpolator(type x1, type x2, double position = 1.0)
{
	return (1.0 - position) * static_cast<double>(x1) + position * static_cast<double>(x2);
}

template<typename type>
inline type to_percentage(type value, type size)
{
	return (value * static_cast<type>(100)) / size;

}

template<typename type> //vector type
inline double squaredEuclideanDistance(type const& p1, type const& p2, unsigned size = 2)
{
	double squaredSum = 0.0;
	for (unsigned i = 0; i < size; ++i)
	{
		squaredSum += std::pow(static_cast<double>(p2[i]) - static_cast<double>(p1[i]), 2.0);
	}

	return squaredSum;
}

template<typename type>
inline double euclideanDistance(type const& p1, type const& p2, unsigned size = 2)
{
	return std::sqrt(squaredEuclideanDistance<type>(p1, p2));

}

inline double timeAverage(double value, int time, double AvgPrev)
{

	return (((time - 1) / time) * AvgPrev) + ((1 / time) * value);

}

}
#endif /* SRC_COMMON_MATH_H_ */
