#pragma once
#include"BaseNetworkCtrlComponent.h"

BaseNetworkCtrlComponent::BaseNetworkCtrlComponent(String bdId, String bdType) :BaseCtrlComponent(bdId,bdType){}

// BaseNetworkCtrlComponent::~BaseNetworkCtrlComponent() {}


JsonDocument* BaseNetworkCtrlComponent::getInputBuffer() { return &(this->jsonInputBuffer); }
JsonDocument* BaseNetworkCtrlComponent::getOutputBuffer() { return &(this->jsonOut); }
JsonDocument* BaseNetworkCtrlComponent::getDataBuffer() { return &(this->jsonData); }