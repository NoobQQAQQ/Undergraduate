#pragma once
#include"File.h"
using std::string;

class Stock
{
public:
	Stock(string FileName); // 从文件读取库存数据
	void SyncData(string FileName); // 同步内存中的库存到文件中
	void PrintStock(); // 管理员打印库存
	void UserPrintStock(); // 用户打印库存
	void AddItem(); // 增加商品
	void RemoveItem(); // 移除商品
	void ChangeItem(); // 修改商品数量
	ItemNode* SearchItem(string ItemName, string ItemBrand) const; // 查找库存中特定商品
	ItemNode* SearchItem(string ItemID) const; // 查找库存中特定商品
	void SearchItem(); // 搜索商品并展示
	void AdvanceSearch(); // 高级搜索
	void SearchNameBrand(); // 根据名称品牌搜索
	void SearchID(); // 根据ID搜索
	void SearchBrand(); // 根据品牌搜索
	bool IsEnough(ItemNode *Scp);  // 判断库存是否足够
	void Traverse(ItemNode *Scp); // 遍历结账
	~Stock();
private:
	ItemNode *Head; // 带附加头结点
	ItemNode *Tail;
	File f;
	int __ItemNum;
	void GetID(string &next); // 计算新商品ID
};