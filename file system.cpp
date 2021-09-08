/*This Program is compiled and executed in DEV C++ Environment And I use TDM-GCC 4.9.2 64-bit Release.
  Tools>>Compiler Options>>General tab :-
  		1) using -std=c++11 command when calling the compiler.
		2) using -static-libgcc command when calling the linker. */
#include<bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdio.h>
#include <windows.h>
using namespace std;

string path = "disk";
int freeBlockList[128];
//class to store i-node information
typedef class inode{
	public:
		int size;
		int blockPointer[8];
		int used;
		char name[8];
}inode; 
inode* in;
string SuperBlockInfo;
string fetch_files[17];
int no_of_files = 0;

//use to convert string to integer.
int string_To_Int(string s){
	// object from the class stringstream
	stringstream input(s);
	int x = 0;
    input >> x;
    return x;
}

//use to display superblock information
void Display_superBlock(){
	for(int i=0;i<128;i++){
		cout<<freeBlockList[i]<<" ";
	}
	cout<<endl;
	for(int i=0;i<16;i++){
		if(in[i].used == 0){
			continue;
		}
		cout<<"name : "<<in[i].name<<endl;
		cout<<"size : "<<in[i].size<<endl;
		cout<<"Block pointers : ";
		for(int m = 0;m<8;m++){
			cout<<in[i].blockPointer[m]<<" ";
		}
		cout<<endl;
		cout<<"user or not (1 - used, 0 - not used) : "<<in[i].used<<endl;
	}
}

//use to read information of superblock if already exist
void Read_info(){
	for(int i=0;i<128;i++){
		freeBlockList[i] = SuperBlockInfo[i] - '0';
	}
	for(int i=0;i<16;i++){
		int pos = i*48+128;
		string s1 = SuperBlockInfo.substr(pos,8);
		if(s1[0] == '@'){
			continue;
		}
		int v = 0;
		for(int l = 0;l<8;l++){
			if(s1[l] != '@'){
				in[i].name[v++] = s1[l];
			}
		}
		pos = pos+8;
		in[i].size = string_To_Int(SuperBlockInfo.substr(pos,4));
		for(int j=0;j<8;j++){
			pos = pos+4;
			in[i].blockPointer[j] = string_To_Int(SuperBlockInfo.substr(pos,4));
		}
		pos = pos+4;
		in[i].used = string_To_Int(SuperBlockInfo.substr(pos,4));
	}
}

//use to update the super block after every step 
void update_suprBlock(){
	//reading superblock to update its information
	ofstream file_write;	
	file_write.open(path+"/"+"superBlock.txt");
	if(file_write.is_open()){
		for(int i=0;i<128;i++){
			file_write<<freeBlockList[i];
		}
		for(int i=0;i<16;i++){
			file_write<<in[i].name;
			int x = 8-strlen(in[i].name);
			for(int j=0;j<x;j++){
				file_write<<'@';
			}
			file_write<<"000"<<in[i].size;
			for(int j = 0;j<8;j++){
				if(in[i].blockPointer[j]<=9){
					file_write<<"000"<<in[i].blockPointer[j];
				}else if(in[i].blockPointer[j]>9 && in[i].blockPointer[j]<=99){
					file_write<<"00"<<in[i].blockPointer[j];
				}else{
					file_write<<"0"<<in[i].blockPointer[j];
				}
			}
			file_write<<"000"<<in[i].used;
		}
	}else{
		cout<<"file is not open"<<endl;
	}
}

//use to print command line information
void Info(){
	cout<<"command information : "<<endl;
	cout<<"'sb' : to show superblock information"<<endl;
	cout<<"'create' : to create new file"<<endl;
	cout<<"'delete' : to delete a file"<<endl;
	cout<<"'read' : to read the specified block from this file"<<endl;
	cout<<"'write' : to  write the data in the buffer to the specified block in this file."<<endl;
	cout<<"'ls' : to list the names of all files in the file system and their sizes."<<endl;
	cout<<"'clear' : to clear the console"<<endl;
	cout<<"'exit' : to exit from the file system"<<endl;
}

//use to create file with specified number of blocks
bool create(char n[8], int s){
	if(s == 0){
		cout<<"size shoud be greater than 0 and less the 9"<<endl;
		return false;
	}
	int freeB = 0; // finding free blocks
	for(int i=1;i<128;i++){
		if(freeBlockList[i] == 0){
			freeB++;
		}
	}
	if(freeB < s){
		cout<<"there are only "<<freeB<<" blocks are free"<<endl;
		return false;
	}
	int index = -1;
	for(int i=0;i<16;i++){
		if(in[i].used == 0){
			index = i;
			break;
		}
	}
	if(index == -1){
		cout<<"file not created because all i-nodes are used"<<endl;
		return false;
	}
	in[index].size = s;
	strcpy(in[index].name,n);
	int x = s;
	//assigning free blocks
	for(int i=0;i<128;i++){
		if(freeBlockList[i] == 0 && x>0){
			in[index].blockPointer[s-x] = i;
			freeBlockList[i] = 1;
			x--;
		}
	}
	in[index].used = 1;
	ofstream create_file;
	//creating file
	create_file.open(path+"/"+n+".txt");
	if(create_file.is_open()){
		for(int i = 0;i<s;i++){
			for(int j = 0;j<1024;j++){
				if(i%2 == 0){
					create_file<<'@';
				}else{
					create_file<<'#';
				}
			}
		}
		create_file.close();
	}else{
		cout<<"problem in creation of file"<<endl;
	}
	return true;
}

//use to delete the file
bool Delete(char *name){
	//checking whether the file exist or not
	bool flag = false;
	string s = "";
	for(int i = 0;i<strlen(name);i++){
		s = s+name[i];
	}
	s = s+".txt";
	for(int i=0;i<no_of_files+2;i++){
		if(s.compare(fetch_files[i]) == 0){
			flag = true;
			break;
		}
	}
	if(flag == false){
		cout<<"file not found"<<endl;
		return false;
	}else{
		string s2 = path+"/"+s;
		int n = s2.size();
		char z[n];
		strcpy(z, s2.c_str());
		int index = -1;
		for(int i= 0;i<16;i++){
			if(strcmp(name,in[i].name) == 0){
				index = i;
				break;
			}
		}
		//reset
		for(int i=0;i<in[index].size;i++){
			freeBlockList[in[index].blockPointer[i]] = 0;
			in[index].blockPointer[i] = 0;
		}
		for(int i=0;i<8;i++){
			in[index].name[i] = '\0';
		}
		in[index].size = 0;
		in[index].used = 0;
		remove(z);
	}
	return true;
}

//use to write some content in the given file in the specified block
bool write(char *name, int blockNum, char* buf){
	//checking whether the file exist or not
	bool flag = false;
	string s = "";
	for(int i = 0;i<strlen(name);i++){
		s = s+name[i];
	}
	s = s+".txt";
	for(int i=0;i<no_of_files+2;i++){
		if(s.compare(fetch_files[i]) == 0){
			flag = true;
			break;
		}
	}
	if(flag == false){
		cout<<"file not found"<<endl;
		return false;
	}else{
		int index = -1;
		for(int i= 0;i<16;i++){
			if(strcmp(name,in[i].name) == 0){
				index = i;
				break;
			}
		}
		if(in[index].size-1 < blockNum){
			cout<<"file has only "<<in[index].size<<" blocks so enter value between 0 to "<<in[index].size-1<<endl;
			return false;
		}
		//Read the old content
		ifstream Read_file;
		Read_file.open(path+"/"+s);
		string content[in[index].size];
		if(Read_file.is_open()){
			string str;
			getline(Read_file >> ws, str);
			for(int i=0;i<in[index].size;i++){
					content[i] = str.substr(i*1024, 1024);
			}
			Read_file.close();
		}else{
			cout<<"file not open"<<endl;
		}
		//write
		string ss;
  		cout << "enter text : ";
  		getline (cin>>ws,ss);
  		if(ss.size() > 1024){
  			cout<<"data should be less than 1024 characters"<<endl;
			return false;  	
		}
  		for(int i=0;i<ss.size();i++){
  			content[blockNum][i] = ss[i];	
		}
  		strcpy(buf, ss.c_str());
  		ofstream write_file;
  		write_file.open(path+"/"+s);
  		if(write_file.is_open()){
  			cout<<"file open successfully"<<endl;
			for(int i=0;i<in[index].size;i++){
				write_file<<content[i];
			}
			write_file.close();
		}else{
			cout<<"problem in opening the file"<<endl;
		}
	}
	return true;
}

//use to read the content of specified block of the file given by the user
bool read(char *name, int blockNum, char* buf){
	//checking whether the file exist or not
	bool flag = false;
	string s = "";
	for(int i = 0;i<strlen(name);i++){
		s = s+name[i];
	}
	s = s+".txt";
	for(int i=0;i<no_of_files+2;i++){
		if(s.compare(fetch_files[i]) == 0){
			flag = true;
			break;
		}
	}
	if(flag == false){
		cout<<"file not found"<<endl;
		return false;
	}else{
		int index = -1;
		for(int i= 0;i<16;i++){
			if(strcmp(name,in[i].name) == 0){
				index = i;
				break;
			}
		}
		if(in[index].size-1 < blockNum){
			cout<<"file has only "<<in[index].size<<" blocks so enter value between 0 to "<<in[index].size-1<<endl;
			return false;
		}
		//Read the content
		ifstream Read_file;
		Read_file.open(path+"/"+s);
		string content[in[index].size];
		if(Read_file.is_open()){
			string str;
			getline(Read_file >> ws, str);
			for(int i=0;i<in[index].size;i++){
					content[i] = str.substr(i*1024, 1024);
			}
			Read_file.close();
		}else{
			cout<<"file not open"<<endl;
		}
		//read
		if(content[blockNum][0] == '@' || content[blockNum][0] == '#'){
			cout<<"block is empty"<<endl;
			return false;
		}
		int z = 0;
		char cmp;
		if(blockNum%2 == 0){
			cmp = '@';
		}else{
			cmp = '#';
		}
		int i;
		for(i=0;i<1024;i++){
			if(content[blockNum][i] == cmp){
				break;
			}
		}
		string fin = content[blockNum].substr(0,i);
		strcpy(buf,fin.c_str());
	}
	return true;
}

//use to fetch all the file names only
void fetch_all_files(){
	DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;

    char buf[512];
    mydir = opendir("disk");
    int i = 0;
    while((myfile = readdir(mydir)) != NULL)
    {
        sprintf(buf, "%s/%s", "disk", myfile->d_name);
        stat(buf, &mystat);
        if((strcmp(myfile->d_name, ".") == 0) || (strcmp(myfile->d_name, "..") == 0)){
        	continue;
		}
        fetch_files[i] = myfile->d_name;
        i++;
    }
    no_of_files = i-2;
    closedir(mydir);
}

// use to print all the files with size
int ls(){
	DIR *mydir;
    struct dirent *myfile;
    struct stat mystat;
	int count = 0;
    char buf[512];
    mydir = opendir("disk");
    while((myfile = readdir(mydir)) != NULL)
    {
        sprintf(buf, "%s/%s", "disk", myfile->d_name);
        stat(buf, &mystat);
        if((strcmp(myfile->d_name, ".") == 0) || (strcmp(myfile->d_name, "..") == 0) || (strcmp(myfile->d_name, "superBlock.txt") == 0)){
        	continue;
		}
		count++;
        printf("filename : %s", myfile->d_name);
        printf(", file size : %zu bytes\n",mystat.st_size);
    }
    closedir(mydir);
    return count;
}

//this is the main function
int main(){
	  // Creating a directory if not exist
    if (mkdir("disk") == -1)
        cerr<<strerror(errno) << endl;
    else
        cout << "Directory created"<<endl;
	in = new inode[16]; //defining i-node
	bool flag = false;
	//read super block
	ifstream file_read;
	A : file_read.open(path+"/"+"superBlock.txt");
	if(file_read.is_open()){
		cout<<"superBlock.txt found"<<endl;
		file_read>>SuperBlockInfo;
		Read_info();
		flag = true;
		file_read.close();
	}else{
		cout<<"superBlock.txt not found"<<endl;
	}
	//goes here if superblock is not there
	if(flag == false){
		ofstream file_write;	
		file_write.open(path+"/"+"superBlock.txt");
		if(file_write.is_open()){
			cout<<"superBlock is created"<<endl;
			file_write<<1;
			for(int i=0;i<127;i++){
				file_write<<0;
			}
			for(int i=0;i<16;i++){
				for(int j = 0;j<8;j++){
					file_write<<'@';
				}
				for(int j = 0;j<40;j++){
					file_write<<0;
				}
			}
			file_write.close();
			goto A;
		}else{
			cout<<"file is not open"<<endl;
		}
	}
	//rin untill you write exit in the console
	while(1){
		string choice;
		cout<<"file_Management_System(Hint : type 'info' to get information)$ ";
		cin>>choice;
		if(choice == "info" || choice == "Info"){
			Info();
		}else if(choice == "sb"){
			Display_superBlock();
		}else if(choice == "create"){ //creation of file
			fetch_all_files();
			C : char name[8];
			int size;
			cout<<"enter the name of the file : ";
			cin>>name;
			//checking whether the file already exist or not
			bool flag = false;
			string sn = "";
			for(int i = 0;i<strlen(name);i++){
				sn = sn+name[i];
			}
			sn = sn+".txt";
			for(int i=0;i<no_of_files+2;i++){
				if(sn.compare(fetch_files[i]) == 0){
					flag = true;
					break;
				}
			}	
			if(flag == true){
				cout<<"file with name "<<sn<<" already exist"<<endl;
				continue;
			}	
			if(strlen(name)>8){
				cout<<"error : length of the name should be greater than 0 and less than 9"<<endl;
				goto C;
			}
			B : cout<<"enter size of the file : ";
			cin>>size;
			if(size > 8){
				cout<<"error : size should be greater than 0 and less than 9"<<endl;
				goto B;
			}                                                                                                  
			bool isCreated = create(name, size);
			if(isCreated){
				cout<<"file created successfully"<<endl;
				update_suprBlock();
			}else{
				cout<<"file not created"<<endl;
			}
		}else if(choice == "delete"){  // deletion of file
			fetch_all_files();
			char* name = new char[8];
			cout<<"enter the name of the file : ";
			cin>>name;
			bool isDelete = Delete(name);
			if(isDelete){
				cout<<"file Deleted successfully"<<endl;
				update_suprBlock();
			}
		}else if(choice == "read"){ //reading content of file
			fetch_all_files();
			char* name = new char[8];
			char* buffer = new char[1024];
			int blockNo;
			cout<<"enter the name of the file : ";
			cin>>name;
			cout<<"enter the block of the file : ";
			cin>>blockNo;
			bool ans = read(name, blockNo, buffer);
			if(ans){
				cout<<"content of the block is : "<<buffer<<endl; 
				cout<<"content readed successfully"<<endl;
			}
		}else if(choice == "write"){ //writing the content to file
			fetch_all_files();
			char* name = new char[8];
			char* buffer = new char[1024];
			int blockNo;
			cout<<"enter the name of the file : ";
			cin>>name;
			cout<<"enter the block of the file : ";
			cin>>blockNo;
			bool ans = write(name, blockNo, buffer);
			if(ans){
				cout<<"content written successfully"<<endl;
			}
		}else if(choice == "ls"){ //getting file information in the disk
			int ans = ls();
			if(ans == 0){
				cout<<"there is no file in the disk"<<endl;
			}
		}else if(choice == "clear"){ //clearing the console
			system("CLS");
		}else if(choice == "exit"){ //exiting 
			update_suprBlock();
			exit(0);
		}else{
			cout<<"command not found"<<endl;
		}
	}
	return 0;
}
