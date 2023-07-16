#pragma once
#include<vector>
#include"File.h"
using namespace std;

class Table
{
public:
	Table();
	~Table();
	bool isEqual(const vector<string> &Record1, const vector<string> &Record2) const; // ������¼
	bool isEqual(const vector<string> &AttNameVec, const vector<string> &Record1, const vector<string> &Record2) const; // ��¼�еĲ�����
	void UniRecordCopy(vector< vector<string> > &RecordCopy) const ; // ������¼ȥ��
	void UniRecordCopy(const vector<string> &AttNameVec, vector< vector<string> > &RecordCopy) const; // ���ݲ�����ȥ��
	bool isGreat(const vector<int> &AttIndexVec, vector<string> &Record1, vector<string> &Record2) const; // R1 > R2 return 1
	void OpenFile(const string &FileName); // ���ļ���ȡ����Ϣ
	void CreateFile(const string &FileName, const vector<string> &AttNameVec); // �����±�
	void CloseFile(); // �ر��ļ���ͬ����Ϣ
	void ShowAttribute() const; // ��ӡ����Ϣ
	void GetTableName(const string &TableName); // �õ�����
	void ShowTable() const; // չʾ���ű�, �Զ����ID��
	void RegetMaxLen(); // �õ������ֵ
	int FindColIndex(const string &ColName) const; // ��������ΪColName�����±�
	void InsertRecord(const vector<string> &Record); // ���¼�����һ��
	void InsertRecord(const vector<string> &AttNameVec, const vector<string> &ValueNameVec); // ����һ����¼, ֻ����ָ����
	// ɾ��ColName = RecordValue�ļ�¼, ����������ɾ��ȫ��
	void DeleteRecord(const string &ColName = "", const string &RecordValue = ""); 
	// ����SelectedCol = SelectedValue�ļ�¼, �������������������
	void UpdateRecord(const vector<string> &AttNameVec, const vector<string> &NewValueVec, const string &SelectedCol = "", const string &SelectedValue = "");
	// ȥ��չʾ������, ������ѡ
	void DisSelectedShow(const string &SelectedCol = "", const string &SelectedValue = "") const;
	// ��ȥ��չʾ������, ������ѡ
	void NoDisSelectedShow(const string &SelectedCol = "", const string &SelectedValue = "") const;
	// ȥ��չʾָ����, ������ѡ
	void DisSelectedShow(const vector<string> &AttNameVec, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// ��ȥ��չʾָ����, ������ѡ
	void NoDisSelectedShow(const vector<string> &AttNameVec, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// ��ָ��������, ��ѡ������
	void SelectedSort(const vector<string> &AttNameVec, const string &mode) const;
	// ȥ�ر���������, ������ѡ
	void DisSelectedToFile(const string &FileName, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// ��ȥ�ر���������, ������ѡ
	void NoDisSelectedToFile(const string &FileName, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// ȥ�ر���ָ����, ������ѡ
	void DisSelectedToFile(const string &FileName, const vector<string> &AttNameVec, const string &SelectedCol = "", const string &SelectedValue = "") const;
	// ��ȥ�ر���ָ����, ������ѡ
	void NoDisSelectedToFile(const string &FileName, const vector<string> &AttNameVec, const string &SelectedCol = "", const string &SelectedValue = "") const;
	void UpdateRecordLess(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void UpdateRecordGreat(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void UpdateRecordNoEqual(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void UpdateRecordNoLess(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void UpdateRecordNoGreat(const vector<string> &AttNameVec, const vector<string> &ValueNameVec, const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordLess(const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordGreat(const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordNoEqual(const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordNoLess(const string &SelectedCol, const string &SelectedValue);
	void DeleteRecordNoGreat(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowLess(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowGreat(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoEqual(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoLess(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoGreat(const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowLess(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowGreat(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedNoEqual(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoLess(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedShowNoGreat(const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileLess(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileGreat(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoEqual(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoLess(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoGreat(const string &SFileName, const vector<string> &AttNameVec, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileLess(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileGreat(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoEqual(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoLess(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void NoDisSelectedToFileNoGreat(const string &SFileName, const string &SelectedCol, const string &SelectedValue);
	void ShowMax(const vector<string> &AttNameVec) const;
	void ShowMin(const vector<string> &AttNameVec) const;
	void ShowAvg(const vector<string> &AttNameVec) const;
private:
	int __ColNum; // ����
	vector<string> ColNameVec; // ����ÿ�е�����
	vector<int> MaxLenVec; // ����ÿһ�е����ֵ
	int __RecordNum; // ��¼����
	vector< vector<string> > RecordVec; // �����¼
	string __TableName; // ����
	string __FileName; // �ļ���
	File f; // �����ļ�IO
};