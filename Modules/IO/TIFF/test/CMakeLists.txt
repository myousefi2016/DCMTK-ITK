itk_module_test()
set(ITKIOTIFFTests
itkTIFFImageIOTest.cxx
itkTIFFImageIOTest2.cxx
itkLargeTIFFImageWriteReadTest.cxx
)

CreateTestDriver(ITKIOTIFF  "${ITKIOTIFF-Test_LIBRARIES}" "${ITKIOTIFFTests}")

itk_add_test(NAME itkTIFFImageIOTest
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/cthead1.tif}
              ${ITK_TEST_OUTPUT_DIR}/cthead1.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/cthead1.tif} ${ITK_TEST_OUTPUT_DIR}/cthead1.tif)
itk_add_test(NAME itkTIFFImageIOTest1
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImage.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImage.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImage.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImage.tif)
itk_add_test(NAME itkTIFFImageIOTest2
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageCCITTFax3.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageCCITTFax3.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageCCITTFax3.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageCCITTFax3.tif)
itk_add_test(NAME itkTIFFImageIOTest3
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageCCITTFax4.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageCCITTFax4.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageCCITTFax4.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageCCITTFax4.tif)
itk_add_test(NAME itkTIFFImageIOTest4
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageGray.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageGray.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageGray.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageGray.tif)
itk_add_test(NAME itkTIFFImageIOTest5
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageHuffmanRLE.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageHuffmanRLE.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageHuffmanRLE.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageHuffmanRLE.tif)
itk_add_test(NAME itkTIFFImageIOTest6
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageJPEG.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageJPEG.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageJPEG.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageJPEG.tif)
itk_add_test(NAME itkTIFFImageIOTest7
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageLZW.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageLZW.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageLZW.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageLZW.tif)
itk_add_test(NAME itkTIFFImageIOTest8
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageNone.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageNone.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageNone.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageNone.tif)
itk_add_test(NAME itkTIFFImageIOTest9
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImagePackBits.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImagePackBits.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImagePackBits.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImagePackBits.tif)
itk_add_test(NAME itkTIFFImageIOTest10
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageZIP.tif}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageZIP.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageZIP.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageZIP.tif)
itk_add_test(NAME itkTIFFImageIOTest11
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/ShortTestImage.tiff}
              ${ITK_TEST_OUTPUT_DIR}/ShortTestImage.tiff
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/ShortTestImage.tiff} ${ITK_TEST_OUTPUT_DIR}/ShortTestImage.tiff 2 2)
itk_add_test(NAME itkTIFFImageIOTest12
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/48BitTestImage.tif}
              ${ITK_TEST_OUTPUT_DIR}/48BitTestImage.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/48BitTestImage.tif} ${ITK_TEST_OUTPUT_DIR}/48BitTestImage.tif 2 2)
itk_add_test(NAME itkTIFFImageIOTest13
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageCCITTFax3.mha}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageCCITTFax3.mha
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageCCITTFax3.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageCCITTFax3.mha)
itk_add_test(NAME itkTIFFImageIOTest14
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/RGBTestImageCCITTFax4.mha}
              ${ITK_TEST_OUTPUT_DIR}/RGBTestImageCCITTFax4.mha
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/RGBTestImageCCITTFax4.tif} ${ITK_TEST_OUTPUT_DIR}/RGBTestImageCCITTFax4.mha)
itk_add_test(NAME itkTIFFImageIOMultiPagesTest
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/ramp.tif}
              ${ITK_TEST_OUTPUT_DIR}/ramp.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/ramp.tif} ${ITK_TEST_OUTPUT_DIR}/ramp.tif 3)
itk_add_test(NAME itkTIFFImageIOMultiPagesTest2
      COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/rampShort.tif}
              ${ITK_TEST_OUTPUT_DIR}/rampShort.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/rampShort.tif} ${ITK_TEST_OUTPUT_DIR}/rampShort.tif 3 3)
itk_add_test(NAME itkTIFFImageIOMultResTest1
   COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/cthead1.tif}
              ${ITK_TEST_OUTPUT_DIR}/itkTIFFImageIOMultResTest1.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/cthead1-multires.tif} ${ITK_TEST_OUTPUT_DIR}/itkTIFFImageIOMultResTest1.tif)
itk_add_test(NAME itkTIFFImageIOMultResTest2
   COMMAND ITKIOTIFFTestDriver
    --compare DATA{${ITK_DATA_ROOT}/Baseline/IO/cthead1.tif}
              ${ITK_TEST_OUTPUT_DIR}/itkTIFFImageIOMultResTest2.tif
    itkTIFFImageIOTest DATA{${ITK_DATA_ROOT}/Input/cthead1-multires.tif} ${ITK_TEST_OUTPUT_DIR}/itkTIFFImageIOMultResTest2.tif 3)
itk_add_test(NAME itkTIFFImageIOSpacing
   COMMAND ITKIOTIFFTestDriver
    itkTIFFImageIOTest2 ${ITK_TEST_OUTPUT_DIR}/itkTIFFImageIOSpacing.tif)


if( "${ITK_COMPUTER_MEMORY_SIZE}" GREATER 5 )

  # Image of 1.67 Gigabytes (pixels size is 16bits)
  itk_add_test(NAME itkLargeTIFFImageWriteReadTest1
      COMMAND ITKIOTIFFTestDriver
    itkLargeTIFFImageWriteReadTest ${ITK_TEST_OUTPUT_DIR}/LargeImage01.tif  30000L )

  # Image of 2.98 Gigabytes (pixels size is 16bits)
  itk_add_test(NAME itkLargeTIFFImageWriteReadTest2
      COMMAND ITKIOTIFFTestDriver
    itkLargeTIFFImageWriteReadTest ${ITK_TEST_OUTPUT_DIR}/LargeImage02.tif  40000L )

  # Image of 4.9 Gigabytes (pixels size is 8-bits out and 16-bit in)
  itk_add_test(NAME itkLargeTIFFImageWriteReadTest3
      COMMAND ITKIOTIFFTestDriver
    itkLargeTIFFImageWriteReadTest ${ITK_TEST_OUTPUT_DIR}/LargeImage03.tif  50000L )

  # Due to the large memory requirements this tests must be run one by one
  set_tests_properties(itkLargeTIFFImageWriteReadTest1
                       itkLargeTIFFImageWriteReadTest2
                       itkLargeTIFFImageWriteReadTest3
                       PROPERTIES
                       RUN_SERIAL 1
                      )
endif()


if( "${ITK_COMPUTER_MEMORY_SIZE}" GREATER 12 )

if(ITK_BUG_2822_IS_FIXED)
  # Image of 9.12 Gigabytes (pixels size is 16bits)
  itk_add_test(NAME itkLargeTIFFImageWriteReadTest4
      COMMAND ITKIOTIFFTestDriver
    itkLargeTIFFImageWriteReadTest ${ITK_TEST_OUTPUT_DIR}/LargeImage04.tif  70000L )

  # Due to the large memory requirements this tests must be run one by one
  set_tests_properties(itkLargeTIFFImageWriteReadTest4
    PROPERTIES
    RUN_SERIAL 1
    )
endif()

endif()
