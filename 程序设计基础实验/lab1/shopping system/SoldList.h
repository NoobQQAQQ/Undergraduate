#pragma once
#include"File.h"

class SoldList
{
public:
	SoldList(string FileName);
	~SoldList();
	void SyncData(string FileName);
	void AddSoldItem(ItemNode *cur, string username);
	void PrintSoldList(); // 打印售货清单
	void Search2ChangeName(string __username, string NewName);
	void PrintRecords();
	void PrintMyRecord(string __username);
private:
	SoldItemNode *Head;
	SoldItemNode *Tail;
	File f;
};