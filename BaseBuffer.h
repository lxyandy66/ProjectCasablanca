#pragma once

class BaseBuffer {
	//这个类针对所有需要缓存的共同属性buffer，例如boardId、rqId、日后可能的agent类型等
	//在此处统一处理
protected:
	String boardId;
	// String cmdType;//cmdType不缓存，只考虑解析时处理
	String boardType;//设备类型
	long rqId;//保存是针对第几次的coordinator信号的处理
public:
	BaseBuffer();
	BaseBuffer(String id, String bdType, long rq);
	String getBoardId();
	String getBoardType();
	int getReqId();
};