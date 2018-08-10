///
/// \file Test_Converter.cpp
/// \author Christian von Arnim
/// \date 09.08.2018
///

#include <gtest/gtest.h>

#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionArray.hpp>
#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionPrimitives.hpp>
#include <SeRoNet/OPCUA/Converter/CommObjArrayToValue.hpp>

TEST(Converer, ObjArrayToVariantArray) {
  std::vector<std::int32_t > testVal = {1, 2, 3};
  auto pSelfDesc = SeRoNet::CommunicationObjects::Description::SelfDescription(&testVal, "");
  auto pArraySelfDesc = std::dynamic_pointer_cast<SeRoNet::CommunicationObjects::Description::ElementArray>(pSelfDesc);
  ASSERT_EQ(pArraySelfDesc->size(), testVal.size());
  SeRoNet::OPCUA::Converter::CommObjArrayToValue commObj2Array(pArraySelfDesc.get());
  auto retVal = commObj2Array.Value();
  EXPECT_EQ(retVal.arrayLength, testVal.size());
  EXPECT_EQ(retVal.arrayDimensionsSize, 1);
  ASSERT_NE(retVal.arrayDimensions, nullptr);
  EXPECT_EQ(retVal.arrayDimensions[0], testVal.size());
  ASSERT_TRUE(UA_NodeId_equal(&(retVal.type->typeId), &(UA_TYPES[UA_TYPES_INT32].typeId)));
  auto *pData = reinterpret_cast<std::int32_t*> (retVal.data);
  decltype(testVal) retValVec(pData, pData + retVal.arrayLength);
  EXPECT_EQ(testVal, retValVec);
}
