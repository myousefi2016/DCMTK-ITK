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
#include "itkIPLFileNameList.h"

#include <algorithm>
namespace itk
{
struct IPLFileSortInfo_ascend_compare:
  public std:: greater< IPLFileSortInfo * > {
private:
  int qsort_IPLFileSortInfo_ascend_compar(IPLFileSortInfo *item1, IPLFileSortInfo *item2)
  {
    const int ImageNoDiff = item1->GetImageNumber() -  item2->GetImageNumber();

    if ( ImageNoDiff < 0 )
      {
      return true;
      }
    if ( ImageNoDiff > 0 )
      {
      return false;
      }
    const int echoNumDiff = item1->GetEchoNumber() - item2->GetEchoNumber();
    if ( echoNumDiff < 0 )
      {
      return true;
      }
    else if ( echoNumDiff > 0 )
      {
      return false;
      }
    const float sliceGap = item1->GetSliceLocation() - item2->GetSliceLocation();
    if ( sliceGap < 0.0 )
      {
      return true;
      }
    if ( sliceGap > 0.0 )
      {
      return false;
      }
    return ( item1->GetImageFileName() < item2->GetImageFileName() );
  }

public:
  bool operator()(IPLFileSortInfo *item1, IPLFileSortInfo *item2)
  {
    return qsort_IPLFileSortInfo_ascend_compar(item1, item2);
  }
};

struct IPLFileSortInfo_descend_compare:
  public std:: greater< IPLFileSortInfo * > {
private:
  int qsort_IPLFileSortInfo_descend_compar(IPLFileSortInfo *item1,  IPLFileSortInfo *item2)
  {
    const int ImageNoDiff = item1->GetImageNumber() -  item2->GetImageNumber();

    if ( ImageNoDiff < 0 )
      {
      return false;
      }
    if ( ImageNoDiff > 0 )
      {
      return true;
      }
    const int echoNumDiff = item1->GetEchoNumber() - item2->GetEchoNumber();
    if ( echoNumDiff < 0 )
      {
      return false;
      }
    if ( echoNumDiff > 0 )
      {
      return true;
      }
    const float sliceGap = item1->GetSliceLocation() - item2->GetSliceLocation();
    if ( sliceGap < 0.0 )
      {
      return false;
      }
    if ( sliceGap > 0.0 )
      {
      return true;
      }
    return ( item1->GetImageFileName()  >= item2->GetImageFileName() );
  }

public:
  bool operator()(IPLFileSortInfo *item1, IPLFileSortInfo *item2)
  {
    return qsort_IPLFileSortInfo_descend_compar(item1, item2);
  }
};

struct IPLFileSortInfo_ascendbyname_compare:
  public std:: greater< IPLFileSortInfo * > {
public:
  bool operator()(IPLFileSortInfo *item1, IPLFileSortInfo *item2)
  {
    return ( item1->GetImageFileName() < item2->GetImageFileName() );
  }
};

struct IPLFileSortInfo_descendbyname_compare:
  public std:: greater< IPLFileSortInfo * > {
public:
  bool operator()(IPLFileSortInfo *item1, IPLFileSortInfo *item2)
  {
    return ( item1->GetImageFileName()  >= item2->GetImageFileName() );
  }
};

void
IPLFileNameList::sortImageListAscend()
{
#if 0
  IPLFileSortInfo_ascend_compare comp;
  //  qsort (fnList->Info, fnList->numImageInfoStructs, sizeof
  // (IPLFileSortInfo),
  //         qsort_IPLFileSortInfo_ascend_compar);
  m_List.sort< IPLFileSortInfo_ascend_compare >(comp);
#else
  std::sort( m_List.begin(), m_List.end(), IPLFileSortInfo_ascend_compare() );
#endif
  return;
}

void
IPLFileNameList::sortImageListDescend()
{
  //  qsort (fnList->Info, fnList->numImageInfoStructs, sizeof
  // (IPLFileSortInfo),
  //     qsort_IPLFileSortInfo_descend_compar);
#if 0
  IPLFileSortInfo_descend_compare comp;
  m_List.sort< IPLFileSortInfo_descend_compare >(comp);
#else
  std::sort( m_List.begin(), m_List.end(), IPLFileSortInfo_descend_compare() );
#endif
  return;
}

void
IPLFileNameList::sortImageList()
{
  if ( m_SortOrder == SortByNameAscend )
    {
    std::sort( m_List.begin(), m_List.end(), IPLFileSortInfo_ascendbyname_compare() );
    }
  else if ( m_SortOrder == SortByNameDescend )
    {
    std::sort( m_List.begin(), m_List.end(), IPLFileSortInfo_descendbyname_compare() );
    }
  else if ( m_SortOrder == SortGlobalDescend )
    {
    std::sort( m_List.begin(), m_List.end(), IPLFileSortInfo_descend_compare() );
    }
  else if ( m_SortOrder == SortGlobalAscend )
    {
    std::sort( m_List.begin(), m_List.end(), IPLFileSortInfo_ascend_compare() );
    }
}
}
