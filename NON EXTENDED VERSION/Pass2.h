#include<bits/stdc++.h>
using namespace std;

void generate_machine_code()
{
	string size = decToHex(hexToDec(arr[arr.size()-1].first) - hexToDec(starting_address));
	ofile.open("objectcode.txt",ios::out|ios::trunc);
	
	cout << "H " << program_name << "\t" <<"00"<< starting_address <<" "<<"00"<< size << endl;
	ofile<< "H " << program_name << "\t" <<"00"<< starting_address <<" "<<"00"<< size << endl;
	int i = 0, save = 0, eternal_flag = 0;
	string firstExecutable;
	while(1)
	{
		int flag = 0, length = 0, codes = 0;
		i = save;
		while(i < arr.size()-1 && object_code[i] != "\t" && codes < 30)
		{
			length++;
			if(!flag && get_opcode(arr[i].second.second.first) != "-1")
			{
				flag = 1;
				cout <<"T "<<"00"<<arr[i].first << " ";
				ofile <<"T "<<"00"<<arr[i].first << " ";
				if(!eternal_flag)
				{
					firstExecutable = arr[i].first;
					eternal_flag = 1;
				}
				
			}
			if(object_code[i] != "\t")
			{
				codes += (object_code[i].size() / 2);
				if(codes > 30)
				{
					codes -= (object_code[i].size() / 2);
					break;
				}
			}
			i++;
		}
		if(!flag)
		{
			save++;
			continue;
		}
		
		cout<<setw(2)<<setfill('0')<< decToHex(codes) << " ";
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
