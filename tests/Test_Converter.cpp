///
/// \file Test_Converter.cpp
/// \author Christian von Arnim
/// \date 09.08.2018
///

#include <gtest/gtest.h>

#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionArray.hpp>
#include <SeRoNet/CommunicationObjects/Description/SelfDescriptionPrimitives.hpp>
#include <SeRoNet/OPCUA/Converter/CommObjArrayToValue.hpp>
#include <SeRoNet/OPCUA/Converter/UaVariantToCommObjArray.hpp>
#include <SeRoNet/OPCUA/Converter/CommObjectToUaVariantArray.hpp>
#include <SeRoNet/OPCUA/Converter/UaVariantArrayToCommObject.hpp>

#include <SeRoNet/DefaultCommObjects/ParameterRequest.hpp>
#include <SeRoNet/DefaultCommObjects/Description/ParameterRequestDescription.hpp>
#include <SeRoNet/DefaultCommObjects/Description/ParameterRequestIdlDescription.hpp>

#include <SeRoNet/State/CommObjs.hpp>
#include <SeRoNet/State/CommObjsSelfDescription.hpp>

TEST(Converter, ObjArrayToVariant) {
  std::vector<std::int32_t> testVal = {1, 2, 3};
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
  auto *pData = reinterpret_cast<std::int32_t *> (retVal.data);
  decltype(testVal) retValVec(pData, pData + retVal.arrayLength);
  EXPECT_EQ(testVal, retValVec);
}

TEST(Converter, VariantToObjArray) {
  std::vector<std::int32_t> inputVal = {1, 2, 3};
  auto pSelfDescInput = SeRoNet::CommunicationObjects::Description::SelfDescription(&inputVal, "");
  auto pArraySelfDescInput =
      std::dynamic_pointer_cast<SeRoNet::CommunicationObjects::Description::ElementArray>(pSelfDescInput);
  SeRoNet::OPCUA::Converter::CommObjArrayToValue commObj2Array(pArraySelfDescInput.get());
  auto retVal = commObj2Array.Value();

  open62541::UA_Variant retValuAsVariant(&retVal, false);
  decltype(inputVal) deserializedValue;
  auto pArraySelfDescDeserialized =
      std::dynamic_pointer_cast<SeRoNet::CommunicationObjects::Description::ElementArray>(
          SeRoNet::CommunicationObjects::Description::SelfDescription(&deserializedValue, "")
      );
  SeRoNet::OPCUA::Converter::VariantToCommObjArray vToCommArray(pArraySelfDescDeserialized.get(), retValuAsVariant);

  EXPECT_EQ(deserializedValue, inputVal);
}

TEST(Converter, ParameterRequest) {
  SeRoNet::DefaultCommObjects::CommParameterRequest inputVal;
  inputVal.setString("abc", "value");

  auto pSelfDescInput = SeRoNet::CommunicationObjects::Description::SelfDescription(&inputVal, "");
  SeRoNet::OPCUA::Converter::CommObjectToUaVariantArray commObj2Array(pSelfDescInput.get());
  auto retVal = commObj2Array.getValue();

  decltype(inputVal) deserializedValue;
  auto pSelfDescDeserialized = SeRoNet::CommunicationObjects::Description::SelfDescription(&deserializedValue, "");
  SeRoNet::OPCUA::Converter::UaVariantArrayToCommObject arr2ComObj(retVal, pSelfDescDeserialized.get());

  EXPECT_EQ(deserializedValue.getString("abc"), inputVal.getString("abc"));

}

TEST(Converter, Parameter_NameValue) {
  SeRoNet::DefaultCommObjects::CommParameterIDL::NameValue inputVal;
  inputVal.key = "abc";
  inputVal.values = {"a1", "b2", "c3"};

  auto pSelfDescInput = SeRoNet::CommunicationObjects::Description::SelfDescription(&inputVal, "");
  SeRoNet::OPCUA::Converter::CommObjectToUaVariantArray commObj2Array(pSelfDescInput.get());
  auto retVal = commObj2Array.getValue();

  decltype(inputVal) deserializedValue;
  auto pSelfDescDeserialized = SeRoNet::CommunicationObjects::Description::SelfDescription(&deserializedValue, "");
  SeRoNet::OPCUA::Converter::UaVariantArrayToCommObject arr2ComObj(retVal, pSelfDescDeserialized.get());

  EXPECT_EQ(deserializedValue.key, inputVal.key);
  EXPECT_EQ(deserializedValue.values, inputVal.values);
}

TEST(Converter, Parameter_CommParameterRequest) {
  SeRoNet::DefaultCommObjects::CommParameterIDL::CommParameterRequest inputVal;
  inputVal.items = {
      {"abc", {"a1", "b2", "c3"}}/*,
      {"abcd", {"a11", "b22", "c33"}}*/
  };

  auto pSelfDescInput = SeRoNet::CommunicationObjects::Description::SelfDescription(&inputVal, "");
  SeRoNet::OPCUA::Converter::CommObjectToUaVariantArray commObj2Array(pSelfDescInput.get());
  auto retVal = commObj2Array.getValue();
  ASSERT_EQ(1, retVal.VariantsSize);
  ASSERT_EQ(1, retVal.Variants->arrayLength);

  decltype(inputVal) deserializedValue;
  auto pSelfDescDeserialized = SeRoNet::CommunicationObjects::Description::SelfDescription(&deserializedValue, "");
  SeRoNet::OPCUA::Converter::UaVariantArrayToCommObject arr2ComObj(retVal, pSelfDescDeserialized.get());

  EXPECT_EQ(deserializedValue, inputVal);
}

TEST(Converter, CommStateRequest) {
  SeRoNet::State::CommStateRequest inputVal;
  inputVal.state_name = "StateName";
  inputVal.command = 22;

  auto pSelfDescInput = SeRoNet::CommunicationObjects::Description::SelfDescription(&inputVal, "");
  SeRoNet::OPCUA::Converter::CommObjectToUaVariantArray commObj2Array(pSelfDescInput.get());
  auto retVal = commObj2Array.getValue();

  decltype(inputVal) deserializedValue;
  auto pSelfDescDeserialized = SeRoNet::CommunicationObjects::Description::SelfDescription(&deserializedValue, "");
  SeRoNet::OPCUA::Converter::UaVariantArrayToCommObject arr2ComObj(retVal, pSelfDescDeserialized.get());

  EXPECT_EQ(deserializedValue, inputVal);
}

TEST(Converter, VectorOfComplexObj) {
  std::vector<SeRoNet::State::CommStateRequest> inputVal;
  {
    SeRoNet::State::CommStateRequest el;
    el.state_name = "StateName";
    el.command = 22;
    inputVal.push_back(el);
    el.state_name = "State3";
    el.command = 333;
    inputVal.push_back(el);
  }

  auto pSelfDescInput = SeRoNet::CommunicationObjects::Description::SelfDescription(&inputVal, "");
  auto pArraySelfDescInput =
      std::dynamic_pointer_cast<SeRoNet::CommunicationObjects::Description::ElementArray>(pSelfDescInput);
  SeRoNet::OPCUA::Converter::CommObjArrayToValue commObj2Array(pArraySelfDescInput.get());
  auto retVal = commObj2Array.Value();

  ASSERT_EQ(retVal.arrayLength, inputVal.size());

  open62541::UA_Variant retValuAsVariant(&retVal, false);
  decltype(inputVal) deserializedValue;
  auto pArraySelfDescDeserialized =
      std::dynamic_pointer_cast<SeRoNet::CommunicationObjects::Description::ElementArray>(
          SeRoNet::CommunicationObjects::Description::SelfDescription(&deserializedValue, "")
      );
  SeRoNet::OPCUA::Converter::VariantToCommObjArray vToCommArray(pArraySelfDescDeserialized.get(), retValuAsVariant);

  EXPECT_EQ(deserializedValue, inputVal);
}
