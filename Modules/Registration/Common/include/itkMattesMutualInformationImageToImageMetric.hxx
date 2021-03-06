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
#ifndef __itkMattesMutualInformationImageToImageMetric_hxx
#define __itkMattesMutualInformationImageToImageMetric_hxx

#include "itkMattesMutualInformationImageToImageMetric.h"
#include "itkImageRandomConstIteratorWithIndex.h"
#include "itkImageRegionIterator.h"
#include "itkImageIterator.h"
#include "vnl/vnl_math.h"
#include "itkStatisticsImageFilter.h"

#include "vnl/vnl_vector.h"
#include "vnl/vnl_c_vector.h"

namespace itk
{
/**
 * Constructor
 */
template <class TFixedImage, class TMovingImage>
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::MattesMutualInformationImageToImageMetric() :
  m_NumberOfHistogramBins(50),
  m_MovingImageNormalizedMin(0.0),
  m_FixedImageNormalizedMin(0.0),
  m_MovingImageTrueMin(0.0),
  m_MovingImageTrueMax(0.0),
  m_FixedImageBinSize(0.0),
  m_MovingImageBinSize(0.0),

  m_CubicBSplineKernel(NULL),
  m_CubicBSplineDerivativeKernel(NULL),

  m_PRatioArray(),

  m_ThreaderMetricDerivative(),

  // Initialize memory
  m_MovingImageMarginalPDF(),
  m_ThreaderFixedImageMarginalPDF(0),

  // For multi-threading the metric
  m_ThreaderJointPDF(0),
  m_ThreaderJointPDFDerivatives(0),
  m_ThreaderJointPDFStartBin(0),
  m_ThreaderJointPDFEndBin(0),
  m_ThreaderJointPDFSum(0),

  m_UseExplicitPDFDerivatives(true),
  m_ImplicitDerivativesSecondPass(false)
{
  this->SetComputeGradient(false); // don't use the default gradient for now
  this->m_WithinThreadPreProcess = true;
  this->m_WithinThreadPostProcess = false;
  this->m_ComputeGradient = false;
}

template <class TFixedImage, class TMovingImage>
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::~MattesMutualInformationImageToImageMetric()
{
}

/**
 * Print out internal information about this class
 */
template <class TFixedImage, class TMovingImage>
void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "NumberOfHistogramBins: ";
  os << this->m_NumberOfHistogramBins << std::endl;

  // Debugging information
  os << indent << "FixedImageNormalizedMin: ";
  os << this->m_FixedImageNormalizedMin << std::endl;
  os << indent << "MovingImageNormalizedMin: ";
  os << this->m_MovingImageNormalizedMin << std::endl;
  os << indent << "MovingImageTrueMin: ";
  os << this->m_MovingImageTrueMin << std::endl;
  os << indent << "MovingImageTrueMax: ";
  os << this->m_MovingImageTrueMax << std::endl;
  os << indent << "FixedImageBinSize: ";
  os << this->m_FixedImageBinSize << std::endl;
  os << indent << "MovingImageBinSize: ";
  os << this->m_MovingImageBinSize << std::endl;
  os << indent << "UseExplicitPDFDerivatives: ";
  os << this->m_UseExplicitPDFDerivatives << std::endl;
  os << indent << "ImplicitDerivativesSecondPass: ";
  os << this->m_ImplicitDerivativesSecondPass << std::endl;
  if( this->m_ThreaderJointPDF.size() > 0 && this->m_ThreaderJointPDF[0].IsNotNull() )
    {
    os << indent << "JointPDF: ";
    os << this->m_ThreaderJointPDF[0] << std::endl;
    }
  if( this->m_ThreaderJointPDFDerivatives.size() > 0 && this->m_ThreaderJointPDFDerivatives[0].IsNotNull() )
    {
    os << indent << "JointPDFDerivatives: ";
    os << this->m_ThreaderJointPDFDerivatives[0];
    }
}

/**
 * Initialize
 */
template <class TFixedImage, class TMovingImage>
void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::Initialize(void)
throw ( ExceptionObject )
{
  this->Superclass::Initialize();
  this->Superclass::MultiThreadingInitialize();
    {
    /**
     * Compute the minimum and maximum within the specified mask
     * region for creating the size of the 2D joint histogram.
     * Areas outside the masked region should be ignored
     * in computing the range of intensity values.
     */

    this->m_FixedImageTrueMin = vcl_numeric_limits<typename TFixedImage::PixelType>::max();
    this->m_FixedImageTrueMax = vcl_numeric_limits<typename TFixedImage::PixelType>::min();
    this->m_MovingImageTrueMin = vcl_numeric_limits<typename TMovingImage::PixelType>::max();
    this->m_MovingImageTrueMax = vcl_numeric_limits<typename TMovingImage::PixelType>::min();

    // We need to make robust measures only over the requested mask region
    itk::ImageRegionConstIteratorWithIndex<TFixedImage> fi(this->m_FixedImage, this->m_FixedImage->GetBufferedRegion() );
    while( !fi.IsAtEnd() )
      {
      typename TFixedImage::PointType fixedSpacePhysicalPoint;
      this->m_FixedImage->TransformIndexToPhysicalPoint(fi.GetIndex(), fixedSpacePhysicalPoint);
      if( this->m_FixedImageMask.IsNull()  // A null mask implies entire space is to be used.
          || this->m_FixedImageMask->IsInside(fixedSpacePhysicalPoint)
          )
        {
        const typename TFixedImage::PixelType currValue = fi.Get();
        this->m_FixedImageTrueMin = (m_FixedImageTrueMin < currValue) ? this->m_FixedImageTrueMin : currValue;
        this->m_FixedImageTrueMax = (m_FixedImageTrueMax > currValue) ? this->m_FixedImageTrueMax : currValue;
        }
      ++fi;
      }

      {
      itk::ImageRegionConstIteratorWithIndex<TMovingImage> mi(this->m_MovingImage,
                                                              this->m_MovingImage->GetBufferedRegion() );
      while( !mi.IsAtEnd() )
        {
        typename TMovingImage::PointType movingSpacePhysicalPoint;
        this->m_MovingImage->TransformIndexToPhysicalPoint(mi.GetIndex(), movingSpacePhysicalPoint);
        if( this->m_MovingImageMask.IsNull()  // A null mask implies entire space is to be used.
            || this->m_MovingImageMask->IsInside(movingSpacePhysicalPoint)
            )
          {
          const typename TMovingImage::PixelType currValue = mi.Get();
          this->m_MovingImageTrueMin = (m_MovingImageTrueMin < currValue) ? this->m_MovingImageTrueMin : currValue;
          this->m_MovingImageTrueMax = (m_MovingImageTrueMax > currValue) ? this->m_MovingImageTrueMax : currValue;
          }
        ++mi;
        }
      }

    itkDebugMacro(" FixedImageMin: " << this->m_FixedImageTrueMin
                                     << " FixedImageMax: " << this->m_FixedImageTrueMax << std::endl);
    itkDebugMacro(" MovingImageMin: " << this->m_MovingImageTrueMin
                                      << " MovingImageMax: " << this->m_MovingImageTrueMax << std::endl);
    }

  /**
   * Compute binsize for the histograms.
   *
   * The binsize for the image intensities needs to be adjusted so that
   * we can avoid dealing with boundary conditions using the cubic
   * spline as the Parzen window.  We do this by increasing the size
   * of the bins so that the joint histogram becomes "padded" at the
   * borders. Because we are changing the binsize,
   * we also need to shift the minimum by the padded amount in order to
   * avoid minimum values filling in our padded region.
   *
   * Note that there can still be non-zero bin values in the padded region,
   * it's just that these bins will never be a central bin for the Parzen
   * window.
   *
   */
  const int padding = 2;  // this will pad by 2 bins

  this->m_FixedImageBinSize = ( this->m_FixedImageTrueMax - this->m_FixedImageTrueMin )
    / static_cast<double>( this->m_NumberOfHistogramBins
                           - 2 * padding );
  this->m_FixedImageNormalizedMin = this->m_FixedImageTrueMin / this->m_FixedImageBinSize
    - static_cast<double>( padding );

  this->m_MovingImageBinSize = ( this->m_MovingImageTrueMax - this->m_MovingImageTrueMin )
    / static_cast<double>( this->m_NumberOfHistogramBins
                           - 2 * padding );
  this->m_MovingImageNormalizedMin = this->m_MovingImageTrueMin / this->m_MovingImageBinSize
    - static_cast<double>( padding );

  itkDebugMacro("FixedImageNormalizedMin: " << this->m_FixedImageNormalizedMin);
  itkDebugMacro("MovingImageNormalizedMin: " << this->m_MovingImageNormalizedMin);
  itkDebugMacro("FixedImageBinSize: " << this->m_FixedImageBinSize);
  itkDebugMacro("MovingImageBinSize; " << this->m_MovingImageBinSize);

  /**
   * Allocate memory for the marginal PDF and initialize values
   * to zero. The marginal PDFs are stored as std::vector.
   */
  this->m_MovingImageMarginalPDF.resize(m_NumberOfHistogramBins, 0.0F);
  // Assumes number of threads doesn't change between calls to Initialize
  this->m_ThreaderFixedImageMarginalPDF.resize(this->m_NumberOfThreads,
                                         std::vector<PDFValueType>(m_NumberOfHistogramBins, 0.0F) );

    {
    this->m_ThreaderJointPDFStartBin.resize( this->m_NumberOfThreads );
    this->m_ThreaderJointPDFEndBin.resize(this->m_NumberOfThreads );
    const int binRange = this->m_NumberOfHistogramBins / this->m_NumberOfThreads;
    for( ThreadIdType threadID = 0; threadID < this->m_NumberOfThreads; threadID++ )
      {
      this->m_ThreaderJointPDFStartBin[threadID] = threadID * binRange;
      this->m_ThreaderJointPDFEndBin[threadID] = ( threadID + 1 ) * binRange - 1;
      }
    // Ensure that the last EndBin range contains the last histogram bin
    this->m_ThreaderJointPDFEndBin[this->m_NumberOfThreads - 1] = this->m_NumberOfHistogramBins - 1;
    }

  this->m_ThreaderJointPDFSum.resize(this->m_NumberOfThreads);

    {
    JointPDFRegionType jointPDFRegion;
      {
      // For the joint PDF define a region starting from {0,0}
      // with size {m_NumberOfHistogramBins, this->m_NumberOfHistogramBins}.
      // The dimension represents fixed image parzen window index
      // and moving image parzen window index, respectively.
      JointPDFIndexType jointPDFIndex;
      jointPDFIndex.Fill(0);
      JointPDFSizeType jointPDFSize;
      jointPDFSize.Fill(m_NumberOfHistogramBins);

      jointPDFRegion.SetIndex(jointPDFIndex);
      jointPDFRegion.SetSize(jointPDFSize);
      }

    // By setting these values, the joint histogram physical locations will correspond to intensity values.
    typename JointPDFType::PointType origin;
    origin[0] = this->m_FixedImageTrueMin;
    origin[1] = this->m_MovingImageTrueMin;
    typename JointPDFType::SpacingType spacing;
    spacing[0] = this->m_FixedImageBinSize;
    spacing[1] = this->m_MovingImageBinSize;
    /**
     * Allocate memory for the joint PDF and joint PDF derivatives.
     * The joint PDF and joint PDF derivatives are store as itk::Image.
     */
    this->m_ThreaderJointPDF.resize(this->m_NumberOfThreads);
    for( ThreadIdType threadID = 0; threadID < this->m_NumberOfThreads; ++threadID )
      {
      this->m_ThreaderJointPDF[threadID] = JointPDFType::New();
      this->m_ThreaderJointPDF[threadID]->SetRegions(jointPDFRegion);
      this->m_ThreaderJointPDF[threadID]->SetOrigin(origin);
      this->m_ThreaderJointPDF[threadID]->SetSpacing(spacing);
      this->m_ThreaderJointPDF[threadID]->Allocate();
      }
    }

  if( this->m_UseExplicitPDFDerivatives )
    {
    // Deallocate the memory that may have been allocated for
    // previous runs of the metric.
    // and by allocating very small the static ones
    this->m_PRatioArray.SetSize(1, 1);            // Not needed if this->m_UseExplicitPDFDerivatives
    this->m_ThreaderMetricDerivative.resize(1);

      {
      JointPDFDerivativesRegionType jointPDFDerivativesRegion;

        {
        // For the derivatives of the joint PDF define a region starting from
        // {0,0,0}
        // with size {m_NumberOfParameters,m_NumberOfHistogramBins,
        // this->m_NumberOfHistogramBins}. The dimension represents transform parameters,
        // fixed image parzen window index and moving image parzen window index,
        // respectively.
        JointPDFDerivativesIndexType jointPDFDerivativesIndex;
        jointPDFDerivativesIndex.Fill(0);
        JointPDFDerivativesSizeType jointPDFDerivativesSize;
        jointPDFDerivativesSize[0] = this->m_NumberOfParameters;
        jointPDFDerivativesSize[1] = this->m_NumberOfHistogramBins;
        jointPDFDerivativesSize[2] = this->m_NumberOfHistogramBins;

        jointPDFDerivativesRegion.SetIndex(jointPDFDerivativesIndex);
        jointPDFDerivativesRegion.SetSize(jointPDFDerivativesSize);
        }

      this->m_ThreaderJointPDFDerivatives.resize(this->m_NumberOfThreads);
      // Set the regions and allocate
      for( ThreadIdType threadID = 0; threadID < this->m_NumberOfThreads; threadID++ )
        {
        this->m_ThreaderJointPDFDerivatives[threadID] = JointPDFDerivativesType::New();
        this->m_ThreaderJointPDFDerivatives[threadID]->SetRegions( jointPDFDerivativesRegion);
        this->m_ThreaderJointPDFDerivatives[threadID]->Allocate();
        }
      }
    }
  else
    {
    // Deallocate the memory that may have been allocated for
    // previous runs of the metric.
    this->m_ThreaderJointPDFDerivatives.resize(0); // Not needed if this->m_UseExplicitPDFDerivatives=false

    /** Allocate memory for helper array that will contain the pRatios
     *  for each bin of the joint histogram. This is part of the effort
     *  for flattening the computation of the PDF Jacobians.
     */
    this->m_PRatioArray.SetSize(this->m_NumberOfHistogramBins, this->m_NumberOfHistogramBins);
    this->m_ThreaderMetricDerivative.resize(this->m_NumberOfThreads, DerivativeType( this->GetNumberOfParameters() ) );
    for( ThreadIdType threadID = 0; threadID < this->m_NumberOfThreads; threadID++ )
      {
      this->m_ThreaderMetricDerivative[threadID].Fill(NumericTraits<MeasureType>::Zero);
      }
    }
  /**
   * Setup the kernels used for the Parzen windows.
   */
  this->m_CubicBSplineKernel = CubicBSplineFunctionType::New();
  this->m_CubicBSplineDerivativeKernel = CubicBSplineDerivativeFunctionType::New();

  /**
   * Pre-compute the fixed image parzen window index for
   * each point of the fixed image sample points list.
   */
  // NOTE:  Need to have computed this->m_FisedImageBinSize here.
  this->ComputeFixedImageParzenWindowIndices(this->m_FixedImageSamples);

}

/**
 * From the pre-computed samples, now
 * fill in the parzen window index locations
 */
template <class TFixedImage, class TMovingImage>
void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::ComputeFixedImageParzenWindowIndices( FixedImageSampleContainer & samples)
{
  const typename FixedImageSampleContainer::const_iterator end = samples.end();
  for( typename FixedImageSampleContainer::iterator iter = samples.begin();
       iter != end; ++iter )
    {
    // Determine parzen window arguments (see eqn 6 of Mattes paper [2]).
    const double windowTerm = static_cast<double>( ( *iter ).value )
      / this->m_FixedImageBinSize
      - this->m_FixedImageNormalizedMin;
    OffsetValueType pindex = static_cast<OffsetValueType>( windowTerm );

    // Make sure the extreme values are in valid bins
    if( pindex < 2 )
      {
      pindex = 2;
      }
    else
      {
      const OffsetValueType nindex =
        static_cast<OffsetValueType>( this->m_NumberOfHistogramBins ) - 3;
      if( pindex > nindex )
        {
        pindex = nindex;
        }
      }

    ( *iter ).valueIndex = pindex;
    }
}

template <class TFixedImage, class TMovingImage>
inline void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::GetValueThreadPreProcess(ThreadIdType threadID,
                           bool withinSampleThread) const
{
  this->Superclass::GetValueThreadPreProcess(threadID, withinSampleThread);

  this->m_ThreaderJointPDF[threadID]->FillBuffer(0.0F);
  std::fill(
    this->m_ThreaderFixedImageMarginalPDF[threadID].begin(),
    this->m_ThreaderFixedImageMarginalPDF[threadID].end(), 0.0F);
}

template <class TFixedImage, class TMovingImage>
inline bool
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::GetValueThreadProcessSample(ThreadIdType threadID,
                              SizeValueType fixedImageSample,
                              const MovingImagePointType & itkNotUsed(mappedPoint),
                              double movingImageValue) const
{
  /**
   * Compute this sample's contribution to the marginal and
   * joint distributions.
   *
   */

  if( movingImageValue < this->m_MovingImageTrueMin )
    {
    return false;
    }
  else if( movingImageValue > this->m_MovingImageTrueMax )
    {
    return false;
    }

  // Determine parzen window arguments (see eqn 6 of Mattes paper [2]).
  const double movingImageParzenWindowTerm = movingImageValue
    / this->m_MovingImageBinSize
    - this->m_MovingImageNormalizedMin;
  // Same as floor
  OffsetValueType movingImageParzenWindowIndex =
    static_cast<OffsetValueType>( movingImageParzenWindowTerm );
  if( movingImageParzenWindowIndex < 2 )
    {
    movingImageParzenWindowIndex = 2;
    }
  else
    {
    const OffsetValueType nindex =
      static_cast<OffsetValueType>( this->m_NumberOfHistogramBins ) - 3;
    if( movingImageParzenWindowIndex > nindex )
      {
      movingImageParzenWindowIndex = nindex;
      }
    }

  const unsigned int fixedImageParzenWindowIndex = this->m_FixedImageSamples[fixedImageSample].valueIndex;

  this->m_ThreaderFixedImageMarginalPDF[threadID][fixedImageParzenWindowIndex] += 1;

  // Pointer to affected bin to be updated
  JointPDFValueType *pdfPtr = this->m_ThreaderJointPDF[threadID]->GetBufferPointer()
    + ( fixedImageParzenWindowIndex * this->m_ThreaderJointPDF[threadID]->GetOffsetTable()[1] );

  // Move the pointer to the first affected bin
  int pdfMovingIndex = static_cast<int>( movingImageParzenWindowIndex ) - 1;
  pdfPtr += pdfMovingIndex;
  const int pdfMovingIndexMax = static_cast<int>( movingImageParzenWindowIndex ) + 2;

  double movingImageParzenWindowArg =
    static_cast<double>( pdfMovingIndex )
    - movingImageParzenWindowTerm;

  while( pdfMovingIndex <= pdfMovingIndexMax )
    {
    *( pdfPtr++ ) += static_cast<PDFValueType>( this->m_CubicBSplineKernel
                                                ->Evaluate(
                                                  movingImageParzenWindowArg) );
    movingImageParzenWindowArg += 1;
    ++pdfMovingIndex;
    }

  return true;
}

template <class TFixedImage, class TMovingImage>
inline void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::GetValueThreadPostProcess( ThreadIdType threadID,
                             bool itkNotUsed(withinSampleThread) ) const
{
  const int maxI = this->m_NumberOfHistogramBins
    * ( this->m_ThreaderJointPDFEndBin[threadID] - this->m_ThreaderJointPDFStartBin[threadID] + 1 );

  JointPDFValueType * const pdfPtrStart = this->m_ThreaderJointPDF[0]->GetBufferPointer()
    + ( this->m_ThreaderJointPDFStartBin[threadID] * this->m_ThreaderJointPDF[0]->GetOffsetTable()[1] );
  const unsigned int tPdfPtrOffset =
    ( this->m_ThreaderJointPDFStartBin[threadID] * this->m_ThreaderJointPDF[0]->GetOffsetTable()[1] );
  // The PDF domain is chunked based on thread.  Each thread consolodates independant parts of the PDF.
  for( unsigned int t = 1; t < this->m_NumberOfThreads; t++ )
    {
    JointPDFValueType * pdfPtr = pdfPtrStart;
    JointPDFValueType const *       tPdfPtr = this->m_ThreaderJointPDF[t]->GetBufferPointer() + tPdfPtrOffset;
    JointPDFValueType const * const tPdfPtrEnd = tPdfPtr + maxI;
    // for(i=0; i < maxI; i++)
    while( tPdfPtr < tPdfPtrEnd )
      {
      *( pdfPtr++ ) += *( tPdfPtr++ );
      }
    for( int i = this->m_ThreaderJointPDFStartBin[threadID]; i <= this->m_ThreaderJointPDFEndBin[threadID]; i++ )
      {
      this->m_ThreaderFixedImageMarginalPDF[0][i] += this->m_ThreaderFixedImageMarginalPDF[t][i];
      }
    }
  // Sum of this threads domain into the this->m_ThreaderJointPDFSum that covers that part of the domain.
  double                    jointPDFSum = 0.0;
  JointPDFValueType const * pdfPtr = pdfPtrStart;
  for( int i = 0; i < maxI; i++ )
    {
    jointPDFSum += *( pdfPtr++ );
    }
  this->m_ThreaderJointPDFSum[threadID] = jointPDFSum;
}

template <class TFixedImage, class TMovingImage>
typename MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::MeasureType
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::GetValue(const ParametersType & parameters) const
{
  // Set up the parameters in the transform
  this->m_Transform->SetParameters(parameters);

  // MUST BE CALLED TO INITIATE PROCESSING
  this->GetValueMultiThreadedInitiate();

  // MUST BE CALLED TO INITIATE PROCESSING
  this->GetValueMultiThreadedPostProcessInitiate();
  // Consolidate to the first element in the vector
  for( ThreadIdType threadID = 1; threadID < this->m_NumberOfThreads; threadID++ )
    {
    this->m_ThreaderJointPDFSum[0] += this->m_ThreaderJointPDFSum[threadID];
    }
  if( this->m_ThreaderJointPDFSum[0] == 0.0 )
    {
    itkExceptionMacro("Joint PDF summed to zero\n" << this->m_ThreaderJointPDF[0] );
    }

  std::fill(this->m_MovingImageMarginalPDF.begin(), this->m_MovingImageMarginalPDF.end(), 0.0F);

  const double       nFactor = 1.0 / this->m_ThreaderJointPDFSum[0];
  JointPDFValueType *pdfPtr = this->m_ThreaderJointPDF[0]->GetBufferPointer();
  double             fixedPDFSum = 0.0;
  for( unsigned int i = 0; i < this->m_NumberOfHistogramBins; i++ )
    {
    fixedPDFSum += this->m_ThreaderFixedImageMarginalPDF[0][i];
    PDFValueType * movingMarginalPtr = &(m_MovingImageMarginalPDF[0]);
    for( unsigned int j = 0; j < this->m_NumberOfHistogramBins; j++ )
      {
      *( pdfPtr ) *= nFactor;
      *( movingMarginalPtr++ ) += *( pdfPtr++ );
      }
    }

  if( this->m_NumberOfPixelsCounted <
      this->m_NumberOfFixedImageSamples / 16 )
    {
    itkExceptionMacro("Too many samples map outside moving image buffer: "
                      << this->m_NumberOfPixelsCounted << " / "
                      << this->m_NumberOfFixedImageSamples
                      << std::endl);
    }

  // Normalize the fixed image marginal PDF
  if( fixedPDFSum == 0.0 )
    {
    itkExceptionMacro("Fixed image marginal PDF summed to zero");
    }
  for( unsigned int bin = 0; bin < this->m_NumberOfHistogramBins; bin++ )
    {
    this->m_ThreaderFixedImageMarginalPDF[0][bin] /= fixedPDFSum;
    }

  /**
   * Compute the metric by double summation over histogram.
   */

  // Setup pointer to point to the first bin
  JointPDFValueType *jointPDFPtr = this->m_ThreaderJointPDF[0]->GetBufferPointer();

  double sum = 0.0;
  for( unsigned int fixedIndex = 0;
       fixedIndex < this->m_NumberOfHistogramBins;
       ++fixedIndex )
    {
    const double fixedImagePDFValue = this->m_ThreaderFixedImageMarginalPDF[0][fixedIndex];
    for( unsigned int movingIndex = 0;
         movingIndex < this->m_NumberOfHistogramBins;
         ++movingIndex, jointPDFPtr++ )
      {
      const double movingImagePDFValue = this->m_MovingImageMarginalPDF[movingIndex];
      const double jointPDFValue = *( jointPDFPtr );

      // check for non-zero bin contribution
      if( jointPDFValue > 1e-16 &&  movingImagePDFValue > 1e-16 )
        {
        const double pRatio = vcl_log(jointPDFValue / movingImagePDFValue);
        if( fixedImagePDFValue > 1e-16 )
          {
          sum += jointPDFValue * ( pRatio - vcl_log(fixedImagePDFValue) );
          }
        } // end if-block to check non-zero bin contribution
      }   // end for-loop over moving index
    }     // end for-loop over fixed index

  return static_cast<MeasureType>( -1.0 * sum );
}

template <class TFixedImage, class TMovingImage>
inline void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::GetValueAndDerivativeThreadPreProcess( ThreadIdType threadID,
                                         bool itkNotUsed(withinSampleThread) ) const
{
  std::fill(
    this->m_ThreaderFixedImageMarginalPDF[threadID].begin(),
    this->m_ThreaderFixedImageMarginalPDF[threadID].end(), 0.0F);
  this->m_ThreaderJointPDF[threadID]->FillBuffer(0.0F);
  if( this->m_UseExplicitPDFDerivatives )
    {
    this->m_ThreaderJointPDFDerivatives[threadID]->FillBuffer(0.0F);
    }
}

template <class TFixedImage, class TMovingImage>
inline bool
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::GetValueAndDerivativeThreadProcessSample(ThreadIdType threadID,
                                           SizeValueType fixedImageSample,
                                           const MovingImagePointType & itkNotUsed(mappedPoint),
                                           double movingImageValue,
                                           const ImageDerivativesType &
                                           movingImageGradientValue) const
{
  /**
   * Compute this sample's contribution to the marginal
   *   and joint distributions.
   *
   */
  if( movingImageValue < this->m_MovingImageTrueMin )
    {
    return false;
    }
  else if( movingImageValue > this->m_MovingImageTrueMax )
    {
    return false;
    }

  unsigned int fixedImageParzenWindowIndex =
    this->m_FixedImageSamples[fixedImageSample].valueIndex;

  // Determine parzen window arguments (see eqn 6 of Mattes paper [2]).
  double movingImageParzenWindowTerm = movingImageValue
    / this->m_MovingImageBinSize
    - this->m_MovingImageNormalizedMin;
  OffsetValueType movingImageParzenWindowIndex =
    static_cast<OffsetValueType>( movingImageParzenWindowTerm );

  // Make sure the extreme values are in valid bins
  if( movingImageParzenWindowIndex < 2 )
    {
    movingImageParzenWindowIndex = 2;
    }
  else
    {
    const OffsetValueType nindex =
      static_cast<OffsetValueType>( this->m_NumberOfHistogramBins ) - 3;
    if( movingImageParzenWindowIndex > nindex )
      {
      movingImageParzenWindowIndex = nindex;
      }
    }

  // Since a zero-order BSpline (box car) kernel is used for
  // the fixed image marginal pdf, we need only increment the
  // fixedImageParzenWindowIndex by value of 1.0.
  this->m_ThreaderFixedImageMarginalPDF[threadID][fixedImageParzenWindowIndex] += 1;

  /**
    * The region of support of the parzen window determines which bins
    * of the joint PDF are effected by the pair of image values.
    * Since we are using a cubic spline for the moving image parzen
    * window, four bins are effected.  The fixed image parzen window is
    * a zero-order spline (box car) and thus effects only one bin.
    *
    *  The PDF is arranged so that moving image bins corresponds to the
    * zero-th (column) dimension and the fixed image bins corresponds
    * to the first (row) dimension.
    *
    */

  // Pointer to affected bin to be updated
  JointPDFValueType *pdfPtr = this->m_ThreaderJointPDF[threadID]->GetBufferPointer()
    + ( fixedImageParzenWindowIndex * this->m_NumberOfHistogramBins );

  // Move the pointer to the fist affected bin
  int pdfMovingIndex = static_cast<int>( movingImageParzenWindowIndex ) - 1;
  pdfPtr += pdfMovingIndex;
  const int pdfMovingIndexMax = static_cast<int>( movingImageParzenWindowIndex ) + 2;

  double movingImageParzenWindowArg = static_cast<double>( pdfMovingIndex )
    - static_cast<double>( movingImageParzenWindowTerm );

  while( pdfMovingIndex <= pdfMovingIndexMax )
    {
    *( pdfPtr++ ) += static_cast<PDFValueType>( this->m_CubicBSplineKernel
                                                ->Evaluate(
                                                  movingImageParzenWindowArg) );

    if( this->m_UseExplicitPDFDerivatives || this->m_ImplicitDerivativesSecondPass )
      {
      // Compute the cubicBSplineDerivative for later repeated use.
      const double cubicBSplineDerivativeValue =
        this->m_CubicBSplineDerivativeKernel->Evaluate(movingImageParzenWindowArg);

      // Compute PDF derivative contribution.
      this->ComputePDFDerivatives(threadID,
                                  fixedImageSample,
                                  pdfMovingIndex,
                                  movingImageGradientValue,
                                  cubicBSplineDerivativeValue);
      }

    movingImageParzenWindowArg += 1;
    ++pdfMovingIndex;
    }

  return true;
}

template <class TFixedImage, class TMovingImage>
inline void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::GetValueAndDerivativeThreadPostProcess(ThreadIdType threadID,
                                         bool withinSampleThread) const
{
  this->GetValueThreadPostProcess(threadID, withinSampleThread);

  if( this->m_UseExplicitPDFDerivatives )
    {
    const unsigned int rowSize = this->m_NumberOfParameters * this->m_NumberOfHistogramBins;

    const unsigned int maxI =
      rowSize * ( this->m_ThreaderJointPDFEndBin[threadID]
                  - this->m_ThreaderJointPDFStartBin[threadID] + 1 );

    JointPDFDerivativesValueType *const pdfDPtrStart = this->m_ThreaderJointPDFDerivatives[0]->GetBufferPointer()
      + ( this->m_ThreaderJointPDFStartBin[threadID] * rowSize );
    const unsigned int tPdfDPtrOffset = this->m_ThreaderJointPDFStartBin[threadID] *  rowSize;
    for( unsigned int t = 1; t < this->m_NumberOfThreads; t++ )
      {
      JointPDFDerivativesValueType *      pdfDPtr = pdfDPtrStart;
      JointPDFDerivativesValueType const *tPdfDPtr = this->m_ThreaderJointPDFDerivatives[t]->GetBufferPointer()
        + tPdfDPtrOffset;
      JointPDFDerivativesValueType const * const tPdfDPtrEnd = tPdfDPtr + maxI;
      // for(i = 0; i < maxI; i++)
      while( tPdfDPtr < tPdfDPtrEnd )
        {
        *( pdfDPtr++ ) += *( tPdfDPtr++ );
        }
      }

    const double nFactor = 1.0 / ( this->m_MovingImageBinSize
                                   * this->m_NumberOfPixelsCounted );

    JointPDFDerivativesValueType *             pdfDPtr = pdfDPtrStart;
    JointPDFDerivativesValueType const * const tPdfDPtrEnd = pdfDPtrStart + maxI;
    // for(int i = 0; i < maxI; i++)
    while( pdfDPtr < tPdfDPtrEnd )
      {
      *( pdfDPtr++ ) *= nFactor;
      }
    }
}

/**
 * Get the both Value and Derivative Measure
 */
template <class TFixedImage, class TMovingImage>
void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::GetValueAndDerivative(const ParametersType & parameters,
                        MeasureType & value,
                        DerivativeType & derivative) const
{
  // Set output values to zero
  value = NumericTraits<MeasureType>::Zero;

  if( this->m_UseExplicitPDFDerivatives )
    {
    // Set output values to zero
    if( derivative.GetSize() != this->m_NumberOfParameters )
      {
      derivative = DerivativeType(this->m_NumberOfParameters);
      }
    memset( derivative.data_block(),
            0,
            this->m_NumberOfParameters * sizeof( double ) );
    }
  else
    {
    this->m_PRatioArray.Fill(0.0);
    for( ThreadIdType threadID = 0; threadID < this->m_NumberOfThreads; threadID++ )
      {
      this->m_ThreaderMetricDerivative[threadID].Fill(NumericTraits<MeasureType>::Zero);
      }
    this->m_ImplicitDerivativesSecondPass = false;
    }

  // Set up the parameters in the transform
  this->m_Transform->SetParameters(parameters);

  // MUST BE CALLED TO INITIATE PROCESSING ON SAMPLES
  this->GetValueAndDerivativeMultiThreadedInitiate();

  // CALL IF DOING THREADED POST PROCESSING
  this->GetValueAndDerivativeMultiThreadedPostProcessInitiate();
  for( ThreadIdType threadID = 1; threadID < this->m_NumberOfThreads; threadID++ )
    {
    this->m_ThreaderJointPDFSum[0] += this->m_ThreaderJointPDFSum[threadID];
    }
  if( this->m_ThreaderJointPDFSum[0] == 0.0 )
    {
    itkExceptionMacro("Joint PDF summed to zero");
    }

  std::fill(this->m_MovingImageMarginalPDF.begin(), this->m_MovingImageMarginalPDF.end(), 0.0F);

  double       fixedPDFSum = 0.0;
  const double normalizationFactor = 1.0 / this->m_ThreaderJointPDFSum[0];

  JointPDFValueType *pdfPtr = this->m_ThreaderJointPDF[0]->GetBufferPointer();
  for( unsigned int i = 0; i < this->m_NumberOfHistogramBins; i++ )
    {
    fixedPDFSum += this->m_ThreaderFixedImageMarginalPDF[0][i];
    PDFValueType * movingMarginalPtr = &(m_MovingImageMarginalPDF[0]);
    for( unsigned int j = 0; j < this->m_NumberOfHistogramBins; j++ )
      {
      *( pdfPtr ) *= normalizationFactor;
      *( movingMarginalPtr++ ) += *( pdfPtr++ );
      }
    }

  if( this->m_NumberOfPixelsCounted <
      this->m_NumberOfFixedImageSamples / 16 )
    {
    itkExceptionMacro("Too many samples map outside moving image buffer: "
                      << this->m_NumberOfPixelsCounted << " / "
                      << this->m_NumberOfFixedImageSamples
                      << std::endl);
    }

  // Normalize the fixed image marginal PDF
  if( fixedPDFSum == 0.0 )
    {
    itkExceptionMacro("Fixed image marginal PDF summed to zero");
    }
  for( unsigned int bin = 0; bin < this->m_NumberOfHistogramBins; bin++ )
    {
    this->m_ThreaderFixedImageMarginalPDF[0][bin] /= fixedPDFSum;
    }

  /**
   * Compute the metric by double summation over histogram.
   */

  // Setup pointer to point to the first bin
  JointPDFValueType *jointPDFPtr = this->m_ThreaderJointPDF[0]->GetBufferPointer();

  // Initialize sum to zero
  double sum = 0.0;

  const double nFactor = 1.0 / ( this->m_MovingImageBinSize
                                 * this->m_NumberOfPixelsCounted );
  for( unsigned int fixedIndex = 0;
       fixedIndex < this->m_NumberOfHistogramBins;
       ++fixedIndex )
    {
    const double fixedImagePDFValue = this->m_ThreaderFixedImageMarginalPDF[0][fixedIndex];
    for( unsigned int movingIndex = 0;
         movingIndex < this->m_NumberOfHistogramBins;
         ++movingIndex, jointPDFPtr++ )
      {
      const double movingImagePDFValue = this->m_MovingImageMarginalPDF[movingIndex];
      const double jointPDFValue = *( jointPDFPtr );

      // check for non-zero bin contribution
      if( jointPDFValue > 1e-16 &&  movingImagePDFValue > 1e-16 )
        {
        const double pRatio = vcl_log(jointPDFValue / movingImagePDFValue);

        if( fixedImagePDFValue > 1e-16 )
          {
          sum += jointPDFValue * ( pRatio - vcl_log(fixedImagePDFValue) );
          }

        if( this->m_UseExplicitPDFDerivatives )
          {
          // move joint pdf derivative pointer to the right position
          JointPDFValueType const * derivPtr = this->m_ThreaderJointPDFDerivatives[0]->GetBufferPointer()
            + ( fixedIndex  * this->m_ThreaderJointPDFDerivatives[0]->GetOffsetTable()[2] )
            + ( movingIndex * this->m_ThreaderJointPDFDerivatives[0]->GetOffsetTable()[1] );
          for( unsigned int parameter = 0; parameter < this->m_NumberOfParameters; ++parameter, derivPtr++ )
            {
            // Ref: eqn 23 of Thevenaz & Unser paper [3]
            derivative[parameter] -= ( *derivPtr ) * pRatio;
            }  // end for-loop over parameters
          }
        else
          {
          this->m_PRatioArray[fixedIndex][movingIndex] = pRatio * nFactor;
          }
        } // end if-block to check non-zero bin contribution
      }   // end for-loop over moving index
    }     // end for-loop over fixed index

  if( !( this->m_UseExplicitPDFDerivatives ) )
    {
    // Second pass: This one is done for accumulating the contributions
    //              to the derivative array.
    //
    this->m_ImplicitDerivativesSecondPass = true;
    //
    // MUST BE CALLED TO INITIATE PROCESSING ON SAMPLES
    this->GetValueAndDerivativeMultiThreadedInitiate();

    // CALL IF DOING THREADED POST PROCESSING
    this->GetValueAndDerivativeMultiThreadedPostProcessInitiate();
    // Consolidate the contributions from each one of the threads to the total
    // derivative.
    for( unsigned int t = 1; t < this->m_NumberOfThreads; t++ )
      {
      DerivativeType const * const source = &( this->m_ThreaderMetricDerivative[t] );
      for( unsigned int pp = 0; pp < this->m_NumberOfParameters; pp++ )
        {
        this->m_ThreaderMetricDerivative[0][pp] += ( *source )[pp];
        }
      }

    derivative = this->m_ThreaderMetricDerivative[0];
    }

  value = static_cast<MeasureType>( -1.0 * sum );
}

/**
 * Get the match measure derivative
 */
template <class TFixedImage, class TMovingImage>
void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::GetDerivative(const ParametersType & parameters,
                DerivativeType & derivative) const
{
  MeasureType value;

  // call the combined version
  this->GetValueAndDerivative(parameters, value, derivative);
}

/**
 * Compute PDF derivatives contribution for each parameter
 */
template <class TFixedImage, class TMovingImage>
void
MattesMutualInformationImageToImageMetric<TFixedImage, TMovingImage>
::ComputePDFDerivatives(ThreadIdType threadID,
                        unsigned int sampleNumber,
                        int pdfMovingIndex,
                        const ImageDerivativesType & movingImageGradientValue,
                        double cubicBSplineDerivativeValue) const
{
  // Update bins in the PDF derivatives for the current intensity pair
  // Could pre-compute
  JointPDFDerivativesValueType *derivPtr;

  double precomputedWeight = 0.0;

  const int pdfFixedIndex = this->m_FixedImageSamples[sampleNumber].valueIndex;

  if( this->m_UseExplicitPDFDerivatives )
    {
    derivPtr = this->m_ThreaderJointPDFDerivatives[threadID]->GetBufferPointer()
      + ( pdfFixedIndex  * this->m_ThreaderJointPDFDerivatives[threadID]->GetOffsetTable()[2] )
      + ( pdfMovingIndex * this->m_ThreaderJointPDFDerivatives[threadID]->GetOffsetTable()[1] );
    }
  else
    {
    derivPtr = 0;
    // Recover the precomputed weight for this specific PDF bin
    precomputedWeight = this->m_PRatioArray[pdfFixedIndex][pdfMovingIndex];
    }

  if( !this->m_TransformIsBSpline )
    {
    /**
     * Generic version which works for all transforms.
     */

    // Compute the transform Jacobian.
    // Should pre-compute
    typedef typename TransformType::JacobianType JacobianType;

    // Need to use one of the threader transforms if we're
    // not in thread 0.
    //
    // Use a raw pointer here to avoid the overhead of smart pointers.
    // For instance, Register and UnRegister have mutex locks around
    // the reference counts.
    TransformType *transform;

    if( threadID > 0 )
      {
      transform = this->m_ThreaderTransform[threadID - 1];
      }
    else
      {
      transform = this->m_Transform;
      }

    JacobianType jacobian;
    transform->ComputeJacobianWithRespectToParameters(
      this->m_FixedImageSamples[sampleNumber].point, jacobian);
    for( unsigned int mu = 0; mu < this->m_NumberOfParameters; mu++ )
      {
      double innerProduct = 0.0;
      for( unsigned int dim = 0; dim < Superclass::FixedImageDimension; dim++ )
        {
        innerProduct += jacobian[dim][mu] * movingImageGradientValue[dim];
        }

      const double derivativeContribution = innerProduct * cubicBSplineDerivativeValue;

      if( this->m_UseExplicitPDFDerivatives )
        {
        *( derivPtr ) -= derivativeContribution;
        ++derivPtr;
        }
      else
        {
        this->m_ThreaderMetricDerivative[threadID][mu] += precomputedWeight * derivativeContribution;
        }
      }
    }
  else
    {
    const WeightsValueType *weights = NULL;
    const IndexValueType *  indices = NULL;

    BSplineTransformWeightsType *   weightsHelper = NULL;
    BSplineTransformIndexArrayType *indicesHelper = NULL;

    if( this->m_UseCachingOfBSplineWeights )
      {
      //
      // If the transform is of type BSplineTransform, we can obtain
      // a speed up by only processing the affected parameters.  Note that
      // these pointers are just pointing to pre-allocated rows of the caching
      // arrays. There is therefore, no need to free this memory.
      //
      weights = this->m_BSplineTransformWeightsArray[sampleNumber];
      indices = this->m_BSplineTransformIndicesArray[sampleNumber];
      }
    else
      {
      if( threadID > 0 )
        {
        weightsHelper = &( this->m_ThreaderBSplineTransformWeights[threadID - 1] );
        indicesHelper = &( this->m_ThreaderBSplineTransformIndices[threadID - 1] );
        }
      else
        {
        weightsHelper = &( this->m_BSplineTransformWeights );
        indicesHelper = &( this->m_BSplineTransformIndices );
        }

      /** Get Jacobian at a point. A very specialized function just for BSplines */
      this->m_BSplineTransform->ComputeJacobianFromBSplineWeightsWithRespectToPosition(
        this->m_FixedImageSamples[sampleNumber].point,
        *weightsHelper, *indicesHelper);
      }
    for( unsigned int dim = 0; dim < Superclass::FixedImageDimension; dim++ )
      {
      for( unsigned int mu = 0; mu < this->m_NumBSplineWeights; mu++ )
        {
        /* The array weights contains the Jacobian values in a 1-D array
         * (because for each parameter the Jacobian is non-zero in only 1 of the
         * possible dimensions) which is multiplied by the moving image
         * gradient. */
        double innerProduct;
        int    parameterIndex;
        if( this->m_UseCachingOfBSplineWeights )
          {
          innerProduct = movingImageGradientValue[dim] * weights[mu];
          parameterIndex = indices[mu] + this->m_BSplineParametersOffset[dim];
          }
        else
          {
          innerProduct = movingImageGradientValue[dim] * ( *weightsHelper )[mu];
          parameterIndex = ( *indicesHelper )[mu] + this->m_BSplineParametersOffset[dim];
          }

        const double derivativeContribution = innerProduct * cubicBSplineDerivativeValue;

        if( this->m_UseExplicitPDFDerivatives )
          {
          JointPDFValueType * const ptr = derivPtr + parameterIndex;
          *( ptr ) -= derivativeContribution;
          }
        else
          {
          this->m_ThreaderMetricDerivative[threadID][parameterIndex] += precomputedWeight * derivativeContribution;
          }
        } // end mu for loop
      }   // end dim for loop
    }     // end if-block transform is BSpline
}

} // end namespace itk

#endif
