
/*
	Tobby Rw-ini Parse
	铁锈ini解析测试+代码整理
	Tobby新测试
	upd 10-14
	禁止商用
*/

/*
	Why in English, you ask? 
	Because the Dev code used in writing is ANSI code,
	the Chinese part is garbled,
	so now all changed to English.
*/

/*	
	更新内容:
	1.0 beta 22-8-28
	支持中文,彩色
	修复亿些bug
	理论上基本可用
	1.1 beta 22-8-28
	支持多行赋值
	1.2 beta 22-8-28
	修复多行赋值bug
	增加类型识别(为以后的更多功能做准备)
	1.3 beta 22-8-28
	支持参数配置
	1.4 beta 22-8-29
	增加两个可配置参数
	修复number和float判断不支持负数
	1.5 beta 22-10-14
	增加节后换行数量参数
*/

#include <bits/stdc++.h>
#include <windows.h>
#include <map>

using namespace std;

const string version = "v1.5 beta";

ifstream fin("test.ini");
ofstream fout("result.ini");

bool keep_comments;
bool keep_empty_line;


void Reset_Color(){
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE);
}

inline bool file_alive(const std::string& name) {
    ifstream f(name.c_str());
    return f.good();
}

string filename_extension_list[5001];
int filename_extension_list_cnt=0;

void add_filename_extension_list(){//文件扩展名列表(忽略大小写)
	filename_extension_list[++filename_extension_list_cnt]="txt";
	filename_extension_list[++filename_extension_list_cnt]="png";
	filename_extension_list[++filename_extension_list_cnt]="ini";
	filename_extension_list[++filename_extension_list_cnt]="jpg";
	filename_extension_list[++filename_extension_list_cnt]="template";
	filename_extension_list[++filename_extension_list_cnt]="ogg";
	filename_extension_list[++filename_extension_list_cnt]="mp3";
	filename_extension_list[++filename_extension_list_cnt]="md";
	//待补充
}

bool all_include(string a,string b){
	//printf("[Debug] A-I %s %s\n",a.c_str(),b.c_str());
	if(b.find(a)!=string::npos||a.find(b)!=string::npos){
		return true;
	}
	return false;//STL快速判断包含
}

int get_number(bool &rs,string s){//取出整数
	rs=false;
	int ans=0;
	int p=1;
	for(int i=0;i<s.size();i++){
		if(s[i]>='0'&&s[i]<='9'){
			ans*=10;
			ans+=s[i]-'0';
		}else{
			if(s[i]!=' '){
				if(s[i]=='-'&&ans==0){
					p=-1;
					continue;//负数
				}
				rs=false;
				return 114514;
			}
		}
	}
	rs=true;
	return p*ans;
}

double get_float(bool &rs,string s){//取出浮点数
	rs=false;
	double ans=0;
	int ws=0;//小数点位数
	bool xf=false;
	int p=1;
	for(int i=0;i<s.size();i++){
		//printf("# s[i]:%c s[i]-\'0\':%d xf:%d ans:%d\n",s[i],s[i]-'0',xf,ans);
		if(s[i]>='0'&&s[i]<='9'){
			if(xf==true) ws++;
			ans*=10;
			ans+=s[i]-'0';
			//printf("ws:%d\n",ws);
		}else{
			if(s[i]=='.'){
				xf=true;
			}else{
				if(s[i]!=' '){
					if(s[i]=='-'&&ans==0){
						p=-1;
						continue;//负数
					}
					rs=false;
					return 114514;
				}	
			}
		}
	}
	rs=true;
	ans/=pow(10,ws);//小数点
	return p*ans;
}

bool get_boolean(bool &rs,string s){
	rs=false;
	for(int i=0;i<s.size();i++){
		if(s[i]>='A'&&s[i]<='Z'){
			s[i]=s[i]-'A'+'a';//统一转小写
		}else{
			if(!(s[i]>='a'&&s[i]<='z')&&s[i]!=' '){
				rs=false;
				return false;
			}
		}
	}
	if(s.find("true")!=string::npos){
		rs=true;
		return true;
	}else if(s.find("false")!=string::npos){
		rs=true;
		return false;
	}else{
		rs=false;
		return false;
	}
}

string rem_sp(string s){//去除首尾空格
	int wz=-1;
	for(int i=0;i<s.size();i++){
		if(s[i]!=' '){
			wz=i;
			break;
		}
	}
	if(wz==-1) return "";//只有空格
	int kz=-1;
	for(int i=s.size();i>0;i--){
		if(s[i]!=' '){
			kz=i;
			break;
		}
	}
	if(kz==-1) return "";//只有空格
	//理论上不会出现kz=-1
	return s.substr(wz,kz);
}

string get_file_type(bool &rs,string s){
	rs=false;
	for(int i=0;i<s.size();i++){
		if(s[i]>='A'&&s[i]<='Z'){
			s[i]=s[i]-'A'+'a';//统一转小写
		}
	}
	//printf("[Debug] file type s:%s \n",s.c_str());
	for(int i=1;i<=filename_extension_list_cnt;i++){
		if(all_include(s,filename_extension_list[i])){
			rs=true;
			return filename_extension_list[i];
		}
	}
	return "null";
}

struct Key{
	string name;//键名称
	string value;//键内容
	string type;//键类型
	/*
	normal - 普通
	comment - 注释
	MLA - 多行赋值
	empty - 空行
	*/
	string data_type;
	int data_num;
	double data_float;
	bool data_boolean;
	string data_file;
	/*
	数据类型
	*unknown - 未知
	*float - 浮点数
	*number - 整数
	string - 字符串
	*boolean - 布尔值
	enum - 特定值
	unit - 单位
	marker - 参考
	*file - 文件
	*/
	Key () {}
	Key (string a,string b){
		name=a,value=b;
	}
};

struct Section{
	string name;//节名称
	int key_cnt=0;//键数量
	Key keys[5001];//节内容(键)
	
};
Section secs[801];

string section_now_name;
int secp=1;
int line_cnt=0;
int section_cnt=0;
int key_cnt=0;
int empty_key_cnt=0;
int comment_key_cnt=0;
bool in_cp=false;//多行赋值

void code_sort(int sep,int kep){
	//secs[sep].keys[kep].name=rem_sp(secs[sep].keys[kep].name);
	//secs[sep].keys[kep].value=rem_sp(secs[sep].keys[kep].value);
	
	string code_name=secs[sep].keys[kep].name;
	string code_value=secs[sep].keys[kep].value;
	
	if(secs[sep].keys[kep].type!="normal"){
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
		printf("[Error] Code type error when sorting: %s\n",code_name.c_str());
		Reset_Color();
		return;
	}
	bool st=false;
	int num_g=get_number(st,code_value);
	if(st==true){
		secs[sep].keys[kep].data_type="number";
		secs[sep].keys[kep].data_num=num_g;
		return;
	}
	double float_g=get_float(st,code_value);
	if(st==true){
		secs[sep].keys[kep].data_type="float";
		secs[sep].keys[kep].data_float=float_g;
		return;
	}
	bool boolean_g=get_boolean(st,code_value);
	if(st==true){
		secs[sep].keys[kep].data_type="boolean";
		secs[sep].keys[kep].data_boolean=boolean_g;
		return;
	}
	string file_g=get_file_type(st,code_value);
	if(st==true){
		secs[sep].keys[kep].data_type="file";
		secs[sep].keys[kep].data_file=file_g;
		return;
	}
	//到这里还没return说明类型为unknown
	secs[sep].keys[kep].data_type="unknown";
	return;
}

void parse(string s){
//	cout<<"[DEBUG] s: ["<<s<<"]\n";
	line_cnt++;
	if(s[0]=='['&&s[s.size()-1]==']'){
		//节
		string sec_name=s.substr(1,s.size()-2);
		//判重
		bool rpt=false;
		for(int i=1;i<=section_cnt;i++){
			if(secs[i].name==sec_name){
				//节已经存在
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
				printf("[Waring] Reapet section: %s\n",sec_name.c_str());
				Reset_Color();
				secp=i;
				rpt=true;
				break;
			}
		}
		if(!rpt){
			printf("[Log] Found new section: %s\n",sec_name.c_str());
			section_cnt++;
			secs[section_cnt].name=sec_name;
			secp=section_cnt;
		}
		section_now_name=sec_name;
	}else{
		//代码
		key_cnt++;
		string key_name;
		string key_value;
		int fs_c=-1;
		int cmf=-1;
		bool fs=false;
		bool emp_l=true;
		bool is_comment=false;
		int dcnt=0;
		for(int i=0;i<s.size();i++){
  			//printf("[DEBUG] i:%d fs_c:%d fs:%d emp_l:%d s[i]:%c name:%s value:%s\n",i,fs_c,fs,emp_l,s[i],key_name,key_value);
			if(s[i]=='\"'){
				if(s[i-1]==s[i]){
					dcnt++;
				}else{
					dcnt=0;
				}
			}
			if(s[i]!=' '){
				emp_l=false;//空行
			}
			if(s[i]=='#'&&fs_c==-1){
				is_comment=true;
				cmf=i;
				break;
			}
			if(s[i]==':'&&fs_c==-1){
				fs_c=i;
				continue;
			}
			if(fs_c==-1&&s[i]!=' '){
				key_name+=s[i];
			}else{
				if(!fs&&s[i]!=' '){
					fs=true;
				}
				if(fs){//&&s[i]!=':'
					key_value+=s[i];
				}//消除ini冒号前后的空格
			}
		}
		if(dcnt==2&&fs_c==-1&&in_cp){//多行赋值结尾
			printf("[Log] Multi-line assignment ended\n");
			in_cp=false;
			secs[secp].key_cnt++;
			secs[secp].keys[secs[secp].key_cnt]=Key("null",s);
			secs[secp].keys[secs[secp].key_cnt].type="MLA";
			goto pdend;
		}
		if(in_cp&&fs_c==-1){//多行赋值中间
			secs[secp].key_cnt++;
			secs[secp].keys[secs[secp].key_cnt]=Key("null",s);
			secs[secp].keys[secs[secp].key_cnt].type="MLA";
			goto pdend;
		}
		if(!is_comment){
			if(fs_c==-1){
				if(emp_l){
					printf("[Log] Find empty line: Line %d\n",line_cnt);
					secs[secp].key_cnt++;
					secs[secp].keys[secs[secp].key_cnt]=Key("null","empty_line");
					secs[secp].keys[secs[secp].key_cnt].type="empty";
					empty_key_cnt++;
				}else{
					SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
					printf("[Error] No colon found in: %s\n",s.c_str());
					Reset_Color();
				}
			}else{
				bool cfj=false;
				for(int i=0;i<=secs[secp].key_cnt;i++){
					if(secs[secp].keys[i].name==key_name){
						SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
						printf("[Waring] Reapet Key: %s\n",key_name.c_str());
						Reset_Color();
						secs[secp].keys[i].value=key_value;
						cfj=true;
						break;
					}
				}
				if(!cfj){
					if(key_value=="\"\"\""){//"\'\'\'"
						if(in_cp==false){
							in_cp=true;//多行赋值
							secs[secp].key_cnt++;
							secs[secp].keys[secs[secp].key_cnt]=Key(key_name,key_value);
							secs[secp].keys[secs[secp].key_cnt].type="normal";
							printf("[Log] Find Multi-line assignment\n");
							
						}else{
							SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
							printf("[Error] Already in Multi-line assignment\n");
							Reset_Color();
						}
					}else{
						key_name=rem_sp(key_name);
						key_value=rem_sp(key_value);
						cout<<"[Log] Found Key - name: "<<key_name<<" value: "<<key_value;//<<"\n"
						secs[secp].key_cnt++;
						secs[secp].keys[secs[secp].key_cnt]=Key(key_name,key_value);//装入结构体方便调用
						secs[secp].keys[secs[secp].key_cnt].type="normal";
						code_sort(secp,secs[secp].key_cnt);
						cout<<" type:"<<secs[secp].keys[secs[secp].key_cnt].data_type<<" \n";
					}
				}
			}
		}else{
			printf("[Log] Find Comment line: %s\n",s.c_str());
			comment_key_cnt++;
			secs[secp].key_cnt++;
			secs[secp].keys[secs[secp].key_cnt]=Key("#",s.substr(cmf+1,s.size()));
			secs[secp].keys[secs[secp].key_cnt].type="comment";
		}
		pdend:int pd_end;
	}
}

int space_before_colon;
int space_after_colon;
int enter_after_section;

void sort_input(){
	fout<<"# This file is automatically collated by Tobby RW-INI Parse\n";
	for(int k=1;k<=section_cnt;k++){
		//输出节名称
		string n_sec=secs[k].name;
		fout<<"["<<n_sec<<"]"<<endl;
		//输出键
		for(int j=1;j<=secs[k].key_cnt;j++){
			if(secs[k].keys[j].type=="comment"){
				if(!keep_comments) continue;
				fout<<secs[k].keys[j].name<<" "<<secs[k].keys[j].value<<endl;//注释行
			}else if(secs[k].keys[j].type=="normal"){
				fout<<secs[k].keys[j].name;
				for(int u=1;u<=space_before_colon;u++) fout<<" ";
				fout<<":";
				for(int u=1;u<=space_after_colon;u++) fout<<" ";
				fout<<secs[k].keys[j].value<<endl;
			}else if(secs[k].keys[j].type=="MLA"){
				fout<<secs[k].keys[j].value<<endl;
			}else if(secs[k].keys[j].type=="empty"){
				if(!keep_empty_line) continue;
				fout<<"\n";
			}else{
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
				printf("[Error] Unknow key type: %s\n",secs[k].keys[j].type.c_str());
				//version question (char*<-string)
				Reset_Color();				
			}
		}
		for(int u=1;u<=enter_after_section;u++) if(k!=section_cnt) fout<<"\n";//节后换行
	}
	//fout<<"# Tobby RW-INI Parse collates documents that are prohibited for commercial use\n";
}

void in_it(){
	Reset_Color();
	system("chcp 65001");//防乱码
	add_filename_extension_list();//补充扩展名列表
	printf("Tobby rw ini parse %s\n",version.c_str());
	//printf("Why in English, you ask? Because the Dev code used in writing is ANSI code, the Chinese part is garbled, so now all changed to English.\n");
	printf("Input File:test.ini  Output File:result.ini  Setting File:set.txt\n");
	if(file_alive("set.txt")){
		ifstream ctr;
		ctr.open("set.txt");
		//1.保留空行
		//2.保留注释
		//3.冒号前空格数量
		//4.冒号后空格数量
		//5.节之间换行数量
		string ct1,ct2;
		ctr>>ct1;
		if(ct1=="1"){
			printf("[Log] Read Setting : keep_empty_line = true\n");
			keep_empty_line=true;
		}else{
			keep_empty_line=false;
			if(ct1!="0"){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
				printf("[Error] setting file wrong : keep_empty_line\n");
				Reset_Color();
			}else{
				printf("[Log] Read Setting : keep_empty_line = false\n");
			}
		}
		ctr>>ct2;
		if(ct2=="1"){
			printf("[Log] Read Setting : keep_comments = true\n");
			keep_comments=true;
		}else{
			keep_comments=false;
			if(ct2!="0"){
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
				printf("[Error] setting file wrong : keep_comments\n");
				Reset_Color();
			}else{
				printf("[Log] Read Setting : keep_comments = false\n");
			}
		}
		string ct3,ct4;
		ctr>>ct3;
		bool gt1=false;
		int gt1_i=get_number(gt1,ct3);
		if(gt1){
			space_before_colon=gt1_i;
			printf("[Log] Read Setting : space_before_colon = %d\n",gt1_i);
		}else{
			space_before_colon=0;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
			printf("[Error] setting file wrong : space_before_colon\n");
			Reset_Color();
		}
		ctr>>ct4;
		bool gt2=false;
		int gt2_i=get_number(gt2,ct4);
		if(gt2){
			space_after_colon=gt2_i;
			printf("[Log] Read Setting : space_after_colon = %d\n",gt2_i);
		}else{
			space_after_colon=1;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
			printf("[Error] setting file wrong : space_after_colon\n");
			Reset_Color();
		}
		string ct5;
		ctr>>ct5;
		bool gt3=false;
		int gt3_i=get_number(gt3,ct5);
		if(gt3){
			enter_after_section=gt3_i;
			printf("[Log] Read Setting : enter_after_section = %d\n",gt3_i);
		}else{
			enter_after_section=0;
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED);
			printf("[Error] setting file wrong : enter_after_section\n");
			Reset_Color();
		}
		ctr.close();
	}else{
		keep_comments=false;
		keep_empty_line=false;
		space_before_colon=0;
		space_after_colon=1;
		ofstream ctw;
		ctw.open("set.txt");
		ctw<<"0\n1\n0\n1\n1\n";
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY|FOREGROUND_RED|FOREGROUND_GREEN);
		printf("[Waring] Missing Setting file: set.txt , Default Settings are being used.\n");
		Reset_Color();
		ctw.close();
	}
}

int main() {
	in_it();
	
	
	system("PAUSE");
	bool gt_f;
	//printf("[Debug] 114514:%d 114.154:%.3lf \n",get_number(gt_f,"114514"),get_float(gt_f,"114.514"));
	
	clock_t start=clock();
	char now[800001];
	while(fin.getline(now,20001)){
//		printf("[DEBUG] Get %s\n",now);
		parse(now);
	}
	clock_t end=clock();
	double endtime=(double)(end-start)/CLOCKS_PER_SEC;
	printf("Parsing is complete.Takes %.2lfms.\n",endtime*1000);
	printf("Statistics:\nSections:%d\nKeys:%d (Empty Key:%d Comment Key:%d)\n",section_cnt,key_cnt,empty_key_cnt,comment_key_cnt);
	sort_input();
	printf("Sorted.\n");
	fin.close();
	fout.close();
	printf("By Tobby.\n");
	
	system("PAUSE");
	return 0;
}
