itk_module_test()

set(ITKLevelSetsv4Tests
itkLevelSetDenseImageBaseTest.cxx
itkWhitakerSparseLevelSetImageTest.cxx
itkShiSparseLevelSetImageTest.cxx
itkMalcolmSparseLevelSetImageTest.cxx
# binary image to sparse level set adaptors
itkBinaryImageToWhitakerSparseLevelSetAdaptorTest.cxx
itkBinaryImageToMalcolmSparseLevelSetAdaptorTest.cxx
itkBinaryImageToShiSparseLevelSetAdaptorTest.cxx
itkLevelSetEquationChanAndVeseExternalTermTest.cxx
itkLevelSetEquationChanAndVeseInternalTermTest.cxx
itkLevelSetEquationCurvatureTermTest.cxx
itkLevelSetEquationLaplacianTermTest.cxx
itkLevelSetEquationPropagationTermTest.cxx
itkLevelSetEquationRegionTermBaseTest.cxx
itkLevelSetEquationTermBaseTest.cxx
itkLevelSetEquationTermContainerBaseTest.cxx
# domain partition classes
itkLevelSetDomainPartitionBaseTest.cxx
itkLevelSetDomainMapImageFilterTest.cxx
# level set container
itkDenseLevelSetContainerTest.cxx
itkSparseLevelSetContainerTest.cxx
# single level set
itkSingleLevelSetDenseImage2DTest.cxx
itkSingleLevelSetWhitakerImage2DTest.cxx
itkSingleLevelSetMalcolmImage2DTest.cxx
itkSingleLevelSetShiImage2DTest.cxx
itkSingleLevelSetWhitakerImage2DWithCurvatureTest.cxx
itkSingleLevelSetWhitakerImage2DWithLaplacianTest.cxx
itkSingleLevelSetWhitakerImage2DWithPropagationTest.cxx
# two level set
itkTwoLevelSetDenseImage2DTest.cxx
itkTwoLevelSetWhitakerImage2DTest.cxx
itkTwoLevelSetMalcolmImage2DTest.cxx
itkTwoLevelSetShiImage2DTest.cxx
# multi level set
itkMultiLevelSetDenseImageTest.cxx
itkMultiLevelSetChanAndVeseInternalTermTest.cxx
itkMultiLevelSetEvolutionTest.cxx
# stopping criterion
itkLevelSetEvolutionNumberOfIterationsStoppingCriterionTest.cxx
)

CreateTestDriver(ITKLevelSetsv4  "${ITKLevelSetsv4-Test_LIBRARIES}" "${ITKLevelSetsv4Tests}")

itk_add_test(NAME itkLevelSetsv4DenseImageBaseTest
      COMMAND ITKLevelSetsv4TestDriver itkLevelSetDenseImageBaseTest)
itk_add_test(NAME itkWhitakerSparseLevelSetsv4BaseTest
      COMMAND ITKLevelSetsv4TestDriver itkWhitakerSparseLevelSetImageTest)
itk_add_test(NAME itkShiSparseLevelSetsv4BaseTest
      COMMAND ITKLevelSetsv4TestDriver itkShiSparseLevelSetImageTest)
itk_add_test(NAME itkMalcolmSparseLevelSetsv4BaseTest
      COMMAND ITKLevelSetsv4TestDriver itkMalcolmSparseLevelSetImageTest)
# binary image to sparse level set adaptors
itk_add_test(NAME itkBinaryImageToWhitakerSparseLevelSetsv4AdaptorTest
      COMMAND ITKLevelSetsv4TestDriver
      itkBinaryImageToWhitakerSparseLevelSetAdaptorTest
      DATA{${ITK_DATA_ROOT}/Input/circle.png}
      ${ITK_TEST_OUTPUT_DIR}/output_binary_whitaker_adaptor.mha
      ${ITK_TEST_OUTPUT_DIR}/status_binary_whitaker_adaptor.mha
      )
itk_add_test(NAME itkBinaryImageToMalcolmSparseLevelSetsv4AdaptorTest
      COMMAND ITKLevelSetsv4TestDriver
      itkBinaryImageToMalcolmSparseLevelSetAdaptorTest
      DATA{${ITK_DATA_ROOT}/Input/circle.png}
      ${ITK_TEST_OUTPUT_DIR}/output_binary_malcolm_adaptor.mha
      )
itk_add_test(NAME itkBinaryImageToShiSparseLevelSetsv4AdaptorTest
      COMMAND ITKLevelSetsv4TestDriver
      itkBinaryImageToShiSparseLevelSetAdaptorTest
      DATA{${ITK_DATA_ROOT}/Input/circle.png}
      ${ITK_TEST_OUTPUT_DIR}/output_binary_shi_adaptor.mha
      )
# domain partition classes
itk_add_test(NAME itkLevelSetsv4DomainPartitionBaseTest
      COMMAND ITKLevelSetsv4TestDriver itkLevelSetDomainPartitionBaseTest)
itk_add_test(NAME itkLevelSetsv4DomainMapImageFilterTest
      COMMAND ITKLevelSetsv4TestDriver itkLevelSetDomainMapImageFilterTest)
# level set container
itk_add_test(NAME itkLevelSetsv4SparseLevelSetContainerTest
      COMMAND ITKLevelSetsv4TestDriver itkSparseLevelSetContainerTest)
itk_add_test(NAME itkLevelSetsv4DenseLevelSetContainerTest
      COMMAND ITKLevelSetsv4TestDriver itkDenseLevelSetContainerTest)
# single level set
itk_add_test(NAME itkSingleLevelSetsv4DenseImage2DTest
      COMMAND ITKLevelSetsv4TestDriver
      --compare DATA{Baseline/solution_whiteSpot_output_dense_single.mha}
                ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_dense_single.mha
      itkSingleLevelSetDenseImage2DTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png} 25 25 25
      ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_dense_single.mha
)

itk_add_test(NAME itkSingleLevelSetsv4WhitakerImage2DTest
      COMMAND ITKLevelSetsv4TestDriver
      --compare DATA{Baseline/solution_whiteSpot_output_sparse_single.mha}
                ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_sparse_single.mha
      itkSingleLevelSetWhitakerImage2DTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png}
      50
      ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_sparse_single.mha
)
itk_add_test(NAME itkSingleLevelSetsv4MalcolmImage2DTest
      COMMAND ITKLevelSetsv4TestDriver
      --compare DATA{Baseline/solution_whiteSpot_output_malcolm_single.mha}
                ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_malcolm_single.mha
      itkSingleLevelSetMalcolmImage2DTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png}
      15
      ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_malcolm_single.mha
 )
itk_add_test(NAME itkSingleLevelSetsv4ShiImage2DTest
      COMMAND ITKLevelSetsv4TestDriver
      --compare DATA{Baseline/solution_whiteSpot_output_shi_single.mha}
                ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_shi_single.mha
      itkSingleLevelSetShiImage2DTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png}
      15
      ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_shi_single.mha
)
itk_add_test(NAME itkSingleLevelSetsv4WhitakerImage2DWithCurvatureTest
      COMMAND ITKLevelSetsv4TestDriver
      itkSingleLevelSetWhitakerImage2DWithCurvatureTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png}
)
itk_add_test(NAME itkSingleLevelSetsv4WhitakerImage2DWithLaplacianTest
      COMMAND ITKLevelSetsv4TestDriver
      itkSingleLevelSetWhitakerImage2DWithLaplacianTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png}
)
itk_add_test(NAME itkSingleLevelSetsv4WhitakerImage2DWithPropagationTest
      COMMAND ITKLevelSetsv4TestDriver
      itkSingleLevelSetWhitakerImage2DWithPropagationTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png}
)

itk_add_test(NAME itkLevelSetsv4EquationCurvatureTermTest
      COMMAND ITKLevelSetsv4TestDriver itkLevelSetEquationCurvatureTermTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png} )

itk_add_test(NAME itkLevelSetsv4EquationPropagationTermTest
      COMMAND ITKLevelSetsv4TestDriver itkLevelSetEquationPropagationTermTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png} )

itk_add_test(NAME itkLevelSetsv4EquationLaplacianTermTest
      COMMAND ITKLevelSetsv4TestDriver itkLevelSetEquationLaplacianTermTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png} )

# two level sets
itk_add_test(NAME itkTwoLevelSetsv4DenseImage2DTest
      COMMAND ITKLevelSetsv4TestDriver
      itkTwoLevelSetDenseImage2DTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png} 25 25 10
      ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_dense_two.mha
)

itk_add_test(NAME itkTwoLevelSetsv4WhitakerImage2DTest
      COMMAND ITKLevelSetsv4TestDriver
      --compare DATA{Baseline/solution_whiteSpot_output_sparse_two.mha}
                ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_sparse_two.mha
      itkTwoLevelSetWhitakerImage2DTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png}
      1
      ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_sparse_two.mha
)

itk_add_test(NAME itkTwoLevelSetsv4MalcolmImage2DTest
      COMMAND ITKLevelSetsv4TestDriver
      --compare DATA{Baseline/solution_whiteSpot_output_malcolm_two.mha}
                ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_malcolm_two.mha
      itkTwoLevelSetMalcolmImage2DTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png}
      1
      ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_malcolm_two.mha
)

itk_add_test(NAME itkTwoLevelSetsv4ShiImage2DTest
      COMMAND ITKLevelSetsv4TestDriver
      --compare DATA{Baseline/solution_whiteSpot_output_shi_two.mha}
                ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_shi_two.mha
      itkTwoLevelSetShiImage2DTest
      DATA{${ITK_DATA_ROOT}/Input/whiteSpot.png}
      1
      ${ITK_TEST_OUTPUT_DIR}/whiteSpot_output_shi_two.mha
)

itk_add_test(NAME itkLevelSetsv4EvolutionNumberOfIterationsStoppingCriterionTest
      COMMAND ITKLevelSetsv4TestDriver itkLevelSetEvolutionNumberOfIterationsStoppingCriterionTest)
itk_add_test(NAME itkMultiLevelSetsv4DenseImageTest
      COMMAND ITKLevelSetsv4TestDriver itkMultiLevelSetDenseImageTest)
itk_add_test(NAME itkMultiLevelSetsv4ChanAndVeseInternalTermTest
      COMMAND ITKLevelSetsv4TestDriver itkMultiLevelSetChanAndVeseInternalTermTest)
itk_add_test(NAME itkMultiLevelSetsv4SetEvolutionTest
      COMMAND ITKLevelSetsv4TestDriver itkMultiLevelSetEvolutionTest)
