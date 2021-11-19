#include<bits/stdc++.h>
using namespace std;

void generate_machine_code()
{
	string size = decToHex(hexToDec(arr[arr.size()-1].first) - hexToDec(starting_address));
	//計算目的程式的長度以位元組為單位，size=最後一個位置-第一個位置+1 
	ofile.open("objectcode.txt",ios::out|ios::trunc);
	
	cout << "H " << program_name << "\t" <<"00"<< starting_address <<" "<<"00"<< size << endl;
	ofile<< "H " << program_name << "\t" <<"00"<< starting_address <<" "<<"00"<< size << endl;
	int i = 0, save = 0, eternal_flag = 0;
	string firstExecutable;
	while(1)
	{
		int flag = 0, length = 0, codes = 0;
		i = save;//從第i個開始找可輸出的位址 
		while(i < arr.size()-1 && object_code[i] != "\t" && codes < 30)//目的碼最多有10組，每段有3個bytes 
		{
			length++;
			if(!flag && get_opcode(arr[i].second.second.first) != "-1")
			{
				cout <<"T ";
				ofile<<"T ";
				flag = 1;
				cout <<"00"<<arr[i].first << " ";
				ofile<<"00"<<arr[i].first << " ";
				if(!eternal_flag)
				{
					firstExecutable = arr[i].first;//設定第一個被執行的位址 
					eternal_flag = 1;
				}
				
			}
			if(object_code[i] != "\t")//object_code[i]不等於RESB,RESW時 
			{
				codes += (object_code[i].size() / 2);//計算目的碼長度，一段目的碼有3個bytes 
				if(codes > 30)
				{
					codes -= (object_code[i].size() / 2);//若目的碼超過10組跳出迴圈 
					break;
				}
			}
			i++;
		}
		if(!flag)//若起始位址找到 RESB,RESW時，使save +1，讓輸出可跳過 
		{
			save++;
			continue;
		}
		
		cout<<setw(2)<<setfill('0')<< decToHex(codes) << " ";//輸出目的碼長度 
		ofile<<setw(2)<<setfill('0')<< decToHex(codes) << " ";
		i = save;
		codes = 0;
		for(; codes < 30; i++)
		{
			if(i >= arr.size() - 1)
				break;
			if(object_code[i] != "\t")
			{
				codes += (object_code[i].size() / 2);
				if(codes > 30)
				{
					break;
				}
				cout << object_code[i] << " "; 
				ofile<< object_code[i] << " ";
			}
			else
			{
				i++;
				break;
			}
		}
		save = i;
		if(i >= arr.size()-1)
			break;
		cout << endl;
		ofile<<endl;
	}
	cout << endl << "E " <<"00"<< firstExecutable << endl;
	ofile<< endl << "E " <<"00"<< firstExecutable << endl;
ofile.close();
}
