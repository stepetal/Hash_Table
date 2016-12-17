#include "stdafx.h"
#include <iostream>
#include <vector>
#include <list>
#include <windows.h>
#include <atlstr.h>
#include <fstream>

using namespace std;

class HashTable
{
	list<string> key_value;//means file name
	int index;//this is value of hash function
protected:
	void AddToList(string value){ key_value.push_front(value); }
public:
	HashTable(){ index = 0; }
	int GetIndex(){ return index; };
	void SetIndex(int in){ index = in; }
	list<string> GetKeyValue(){ return key_value; }
	list<string>::iterator GetBegIterator(){ return key_value.begin(); }
	list<string>::iterator GetEndIterator(){ return key_value.end(); }
	int GetCollisionNumb(){ return key_value.size(); }
	void AddToHashTable(int ind,string value);
};

void HashTable::AddToHashTable(int ind,string value)
{
	SetIndex(ind);
	AddToList(value);
}

//Hash Function 1 byte
char FormHash(string key)
{
	char hash_value = 0;
	for (int i = 0; i < key.size(); i++){
		hash_value = (char)((int)hash_value ^ (int)key[i]);
	}
	return hash_value;
}

//Forward through folders recursively
//to the list_of_names names of directories and files are pushed back
bool RecursiveCall(wstring path, wstring mask, vector<wstring> &list_of_names)
{
	WIN32_FIND_DATAW wfd;
	vector<char> file_name;
	wstring path_with_mask;
	wstring path_without_mask;
	path_with_mask = path + L"\\" + mask;
	path_without_mask = path + L"\\";
	HANDLE FindFile = FindFirstFileW(path_with_mask.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == FindFile){
		return false;
	}
	do
		{
			if (wcscmp(wfd.cFileName, L".") != 0 && wcscmp(wfd.cFileName, L"..") != 0){
				if (wfd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY){
					path_without_mask += wfd.cFileName;
					list_of_names.push_back(wfd.cFileName);
					RecursiveCall(path_without_mask, L"*", list_of_names);
				}
				else{
					list_of_names.push_back(wfd.cFileName);
				}
			}
		} while (FindNextFileW(FindFile, &wfd)!=0);
		FindClose(FindFile);
	return true;
}

//without recursive traversing of files
bool MakeListOfFiles(vector<string> &list_of_names)
{
	WIN32_FIND_DATAW wfd;
	vector<char> file_name;
	string str_file_name;
	HANDLE const hFind = FindFirstFileW(L"C:\\Windows\\System32\\*", &wfd);
	//HANDLE const hFind = FindFirstFileW(L"D:\\*", &wfd);
	setlocale(LC_ALL, "");
	CString str;
	if (INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			wcout  << &wfd.cFileName[0]  << endl;
			str = &wfd.cFileName[0];
			for (int i = 0; i < str.GetLength(); i++){
				str_file_name.push_back(str[i]);
			}
			list_of_names.push_back(str_file_name);
			str_file_name.clear();		
		} while (NULL != FindNextFileW(hFind, &wfd));

		FindClose(hFind);
	}
	
	return true;
}

//forming of table with alpha values
void FormAlphaTable(HashTable &hash_tab,float &alpha_total)
{
	string my_str;
	float alpha = 0.0;
	list<string>::iterator beg_it,end_it;
	beg_it = hash_tab.GetBegIterator();
	end_it = hash_tab.GetEndIterator();
	ofstream o_file,alpha_file;
	o_file.open("hash_table.txt", ofstream::out | ofstream::app);
	alpha_file.open("alpha_table.txt", ofstream::out | ofstream::app);
	if (o_file){
		if (hash_tab.GetCollisionNumb() == 0){
			o_file << hash_tab.GetIndex() << " " << 0;
			o_file << "\n";
		}
		else{
			o_file << hash_tab.GetIndex();
			o_file << "  ";
			for (beg_it; beg_it != end_it; beg_it++){
				my_str=*beg_it;
				for (int i = 0; i < my_str.size(); i++){
					o_file << my_str[i];
				}
				my_str.clear();
				o_file << " ";
			}
			o_file << "\n";
			
			alpha = (float)hash_tab.GetCollisionNumb() / (float)256;
			alpha_total += alpha;
			alpha_file << hash_tab.GetIndex() << " " << alpha;
			alpha_file << "\n";

		}
		o_file.close();
		alpha_file.close();
	}

}


int _tmain(int argc, _TCHAR* argv[])
{
	HashTable hash_table[256];
	vector<wstring> wlist_of_names;
	vector<string> list_of_names;
	wstring wstr=L"";
	string str = "";
	float alpha_total = 0.0;
	float alpha_aver = 0.0;
	char ch;
  //MakeListOfFiles(list_of_names);
	//RecursiveCall(L"D:\\NewFolder", L"*",list_of_names);
	RecursiveCall(L"C:\\Windows\\System32", L"*", wlist_of_names);
	for (int i = 0; i < wlist_of_names.size(); i++){
		wstr = wlist_of_names[i];
		for (int j = 0; j < wstr.size(); j++){
			str += wstr[j];
		}
		list_of_names.push_back(str);
		str.clear();
		wstr.clear();
	}
	ofstream o_file1("hash_table.txt"), o_file2("alpha_table.txt");
	o_file1.close();
	o_file2.close();
	for (int i = 0; i < list_of_names.size(); i++){
		ch = FormHash(list_of_names[i]);
		hash_table[(int)ch].AddToHashTable(ch,list_of_names[i]);
	}

	for (int i = 0; i < 256; i++){
		if (hash_table[i].GetCollisionNumb() == 0){
			hash_table[i].SetIndex(i);
		}
	}
	for (int i = 0; i < 256; i++){
		FormAlphaTable(hash_table[i],alpha_total);
	}
	alpha_aver = alpha_total / 256;
	cout << "Avrerage load factor for whole hash table: " << alpha_aver << endl;
	return 0;
}

