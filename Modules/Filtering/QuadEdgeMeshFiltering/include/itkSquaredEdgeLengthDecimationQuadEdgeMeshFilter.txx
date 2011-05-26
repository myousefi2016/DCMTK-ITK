/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkSquaredEdgeLengthDecimationQuadEdgeMeshFilter_txx
#define __itkSquaredEdgeLengthDecimationQuadEdgeMeshFilter_txx

#include "itkSquaredEdgeLengthDecimationQuadEdgeMeshFilter.h"

namespace itk
{
template< class TInput, class TOutput, class TCriterion >
SquaredEdgeLengthDecimationQuadEdgeMeshFilter< TInput, TOutput,
                                         TCriterion >::SquaredEdgeLengthDecimationQuadEdgeMeshFilter():Superclass()
{}

template< class TInput, class TOutput, class TCriterion >
SquaredEdgeLengthDecimationQuadEdgeMeshFilter< TInput, TOutput, TCriterion >::
~SquaredEdgeLengthDecimationQuadEdgeMeshFilter()
{}

template< class TInput, class TOutput, class TCriterion >
typename
SquaredEdgeLengthDecimationQuadEdgeMeshFilter< TInput, TOutput, TCriterion
                                         >::OutputPointType
SquaredEdgeLengthDecimationQuadEdgeMeshFilter< TInput, TOutput, TCriterion >::Relocate(OutputQEType *iEdge)
{
  OutputMeshPointer     output = this->GetOutput();
  OutputPointIdentifier id_org = iEdge->GetOrigin();
  OutputPointIdentifier id_dest = iEdge->GetDestination();

  OutputPointType oPt;

  oPt.SetToMidPoint( output->GetPoint(id_org),
                     output->GetPoint(id_dest) );

  return oPt;
}
}
#endif
