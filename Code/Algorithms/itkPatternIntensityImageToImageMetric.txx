/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkPatternIntensityImageToImageMetric.txx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) 2000 National Library of Medicine
  All rights reserved.

  See COPYRIGHT.txt for copyright details.

=========================================================================*/
#ifndef _itkPatternIntensityImageToImageMetric_txx
#define _itkPatternIntensityImageToImageMetric_txx

#include "itkPatternIntensityImageToImageMetric.h"

namespace itk
{

/**
 * Constructor
 */
template < class TTarget, class TMapper > 
PatternIntensityImageToImageMetric<TTarget,TMapper>
::PatternIntensityImageToImageMetric()
{
}




/**
 * Get the match Measure
 */
template < class TTarget, class TMapper > 
PatternIntensityImageToImageMetric<TTarget,TMapper>::MeasureType
PatternIntensityImageToImageMetric<TTarget,TMapper>
::GetValue( const ParametersType & parameters )
{

  TargetPointer target = Superclass::GetTarget();

  typename TTarget::RegionType  targetRegion = target->GetLargestPossibleRegion();
  itk::Point<double, TTarget::ImageDimension> Point;  

  double ReferenceValue;
  double TargetValue;

  typedef  itk::SimpleImageRegionIterator<TTarget> TargetIteratorType;


  TargetIteratorType ti( target, targetRegion );
  ti.Begin();

  typename TTarget::IndexType index;

  m_MatchMeasure = 0;
  
  bool insidePoint; 

  unsigned int  count = 0;

  GetMapper()->GetTransformation()->SetParameters( parameters );

  while(!ti.IsAtEnd())
  {
    index = ti.GetIndex();
    for(unsigned int i=0 ; i<TTarget::ImageDimension ; i++)
    {
    Point[i]=index[i];
    }

    insidePoint = true;

    try {
     ReferenceValue = GetMapper()->Evaluate( Point );
    }

    //If the Mapped Voxel is outside the image
    catch (MapperException) 
    {  
      insidePoint = false;
    }

    if(insidePoint) 
    {
      TargetValue = ti.Get();
      count++;
      const double diff = ReferenceValue - TargetValue; 
      m_MatchMeasure += 1.0 / ( 1.0 + diff * diff ); 
    }  
  
   ++ti;
  }

  if(count == 0) 
  {
    std::cout << "All the mapped image is outside !" << std::endl;
    return 100000;
  } 

  // Negative sign to produce a metric to minimize
  m_MatchMeasure = -m_MatchMeasure;     
  std::cout<<"m_MatchMeasure= "<<m_MatchMeasure<<std::endl; 
  return m_MatchMeasure;

}





/**
 * Get the Derivative Measure
 */
template < class TTarget, class TMapper> 
const PatternIntensityImageToImageMetric<TTarget,TMapper>::DerivativeType &
PatternIntensityImageToImageMetric<TTarget,TMapper>
::GetDerivative( const ParametersType & parameters )
{

  const double delta = 0.00011;
  ParametersType testPoint;
  testPoint = parameters;

  for( unsigned int i=0; i<SpaceDimension; i++) 
  {
    testPoint[i] -= delta;
    const MeasureType valuep0 = GetValue( testPoint );
    testPoint[i] += 2*delta;
    const MeasureType valuep1 = GetValue( testPoint );
    m_MatchMeasureDerivatives[i] = (valuep1 - valuep0 ) / ( 2 * delta );
    m_MatchMeasureDerivatives[i];
    testPoint[i] = parameters[i];
  }

  return m_MatchMeasureDerivatives;

}




/**
 * Get both the match Measure and theDerivative Measure 
 */
template < class TTarget, class TMapper > 
void
PatternIntensityImageToImageMetric<TTarget,TMapper>
::GetValueAndDerivative(const ParametersType & parameters, 
                        MeasureType & Value, DerivativeType  & Derivative)
{
  Value      = GetValue( parameters );
  Derivative = GetDerivative( parameters );
}



} // end namespace itk


#endif
