#pragma once
#include "CtrlComponent.h"

CtrlComponent::CtrlComponent(String bdId, String bdType) :boardId(bdId), boardType(bdType) {}


String CtrlComponent::getBoardId() { return this->boardId; }

void CtrlComponent::setBoardId(String str) { this->boardId = str; }

void CtrlComponent::setBoardType(String str) { this->boardType = str; }

String CtrlComponent::getBoardType() { return this->boardType; }

// CtrlComponent::~CtrlComponent() {}

int CtrlComponent::threadBlinker(struct pt* pt) {
	//LED控制线程
	PT_BEGIN(pt);
	while (true) {
		PT_WAIT_UNTIL(pt, this->needBlink);//持续监听needBlink变量
		this->changeLed(true);
		// this->debugPrint("in the class, set led as on");
		PT_WAIT_UNTIL(pt, !(this->needBlink));
		this->changeLed(false);
		// this->debugPrint("in the class, set led as off");
	}
	PT_END(pt);
}


void CtrlComponent::changeLed(boolean b) {
	digitalWrite(this->pinLed, b);
	return;
}

void CtrlComponent::changeLed() {
	this->needBlink = !needBlink;
	digitalWrite(this->pinLed, this->needBlink);
	return;
}


void CtrlComponent::setNeedBlink(boolean b) {
	this->needBlink = b;
}
boolean CtrlComponent::getNeedBlink() {
	return this->needBlink;
}

void CtrlComponent::setLedPin(unsigned long p) {
	pinMode(p, OUTPUT);
	this->pinLed = p;
}
unsigned long CtrlComponent::getLedPin() { return this->pinLed; }

JsonDocument* CtrlComponent::getInputBuffer() { return &(this->jsonInputBuffer); }
JsonDocument* CtrlComponent::getOutputBuffer() { return &(this->jsonOut); }
JsonDocument* CtrlComponent::getDataBuffer() { return &(this->jsonData); }