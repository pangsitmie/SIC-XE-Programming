#include <bits/stdc++.h>
using namespace std;

string program_name, starting_address;
vector<pair<string,pair<string,pair<string,string>>>> arr;
map<string,string> labels;
vector<string> object_code;
fstream ifile, ofile;



int hexToDec(string str)
{
    int y;
    stringstream stream;
    stream<<str;
    stream>>hex>>y;
    return y;
}

string decToHex(int num)
{
    stringstream stream;
    stream<<hex<<num;
    return stream.str();
}

void input()
{
    string str, temp;
    int index =0;
    ifile.open("Input.txt",ios::in);

    while(!ifile.eof())
    {
        str.clear();
        arr.push_back(make_pair("\t",make_pair("\t",make_pair("\t","\t"))));
        getline(ifile, str);
        int flag=0, found_opcode=0;

        if(str[0] == (char)46)
        {
            arr.resize(arr.size()-1);
            continue;
        }

        for(int i=0;i<str.size();i++)
        {
            while(i<str.size() && str[i] != ' '&& str[i] != '\t'&& str[i] != '\n')
            {
                temp.push_back(str[i++]);
                flag=1;
            }

            if(flag)
            {
                if(get_opcode(temp) == "-1" && (temp != "BYTE" && temp != "WORD" && temp != "RESB" && temp != "RESW" &&
                temp != "START" &&temp != "END"))
                {
                    if(!found_opcode)
                    {
                        arr[index].second.first = temp;
                    }
                    else
                        arr[index].second.second.second = temp;
                }
                else
                {
                    arr[index].second.second.first = temp;
                    found_opcode=1;
                }
                temp.clear();
                flag=0;
            }
        }
        program_name = arr[index].second.first;
        starting_address = arr[index].second.second.second;
        index++;
    }
    ifile.close();
}


string add(string str, string adder, int flag)
{
	//Adding hex and hex
	if(flag)
	{
		int num1 = hexToDec(str);
		int num2 = hexToDec(adder);
		int sum = num1 + num2;
		return decToHex(sum);
	}
	//Adding decimal and hex
	else
	{
		int num1 = hexToDec(str);
		int num2 = atoi(adder.c_str());
		int sum = num1 + num2;

		return decToHex(sum);
	}
}

void addressing()
{
    arr[0].first = starting_address;
    arr[1].first = starting_address;

    if(arr[0].second.first.size() > 0)//IF FOUND LABEL 
        labels[arr[0].second.first] = arr[0].first;//MARK THAT LABEL WITH THE STARTING ADDRESS
    
    for(int i=2;i<arr.size();i++)
    {
        string mnemonic = arr[i-1].second.second.first;
        string lastAddress = arr[i-1].first;
        if(mnemonic !="BYTE" && mnemonic !="RESB" && mnemonic !="RESW")
        {
            arr[i].first = add(lastAddress,"3",0);//add address by 3
        }
        else
        {
            if(mnemonic == "BYTE")//IF MNEMONIC ==  BYTE
            {
                int bytes;
                string label2 = arr[i-1].second.second.second;
                char ch = label2[0];
                if(ch == 'C')
                {
                    bytes = (label2.size()-3);
                }
                else
                {
                    if((label2.size() - 3) % 2 == 0)
					{
						bytes = (label2.size() -3) / 2;
					}
					else
					{
						bytes = ((label2.size() - 3) / 2) + 1;
					}
                }
                arr[i].first = add(lastAddress,to_string(bytes),1);
            }
            else if(mnemonic == "RESB")//IF MNEMONIC ==  RESB
            {
                int reserve = atoi(arr[i-1].second.second.second.c_str());
                string hexaReserve = decToHex(reserve);
                arr[i].first = add(lastAddress,hexaReserve,1);
            }
            else//IF MNEMONIC == RESERVE WORD
            {
                int reserve = 3 * atoi(arr[i-1].second.second.second.c_str());
                string hexaReserve = decToHex(reserve);
                arr[i].first = add(lastAddress,hexaReserve,1);
            }
        }
        if(arr[i].second.first.size()>0)//IF FOUND INSTRUCTIONS
            labels[arr[i].second.second.first] = arr[i].first;//MARK THE LABEL WITH THAT CURRENT ADDRESS
    }
    ofile.open("loc.txt",ios::out|ios::trunc);
    ifile.open("Input.txt", ios::in);
    string str;
    int j =0;
    while(!ifile.eof())
    {
        string s;
        getline(ifile,str);
        if(str[0]!='.')//IF THE CURRENT STRING IS NOT "."
        {
            if(arr[j].second.first != "\t")
            {
                ofile<<arr[j].first<<"\t"<<arr[j].second.first<<"\t"<<arr[j].second.second.first<<"\t"<<arr[j].second.second.second<<endl;
			    j++;	
            }
            else
            {
                ofile<<arr[j].first<<"\t"<<arr[j].second.first<<arr[j].second.second.first<<"\t"<<arr[j].second.second.second<<endl;
			    j++;
            }
        }
        else // IF CURRENT STRING IS "."
        {
            ofile<<arr[j].first<<"\t"<<str<<endl;
        }
    }
    ofile.close();
    ifile.close();
}



void generate_object_code()
{
	string objectCode = "", mnemonic, operand, label_address;
	for(int i = 0; i < arr.size() ; i++)
	{
		int flag = 0;
		objectCode.clear();
		mnemonic = arr[i].second.second.first;
		if(mnemonic == "RESW" || mnemonic == "RESB" || mnemonic == "END"||mnemonic == "START")
		{
			object_code.push_back("\t");
			continue;
		}
		operand.clear();
		for(int l = 0; l < arr[i].second.second.second.size(); l++)
		{
			if(arr[i].second.second.second[l] == ',')//�P�_�O�_���򩳬۹�w�} 
			{
				flag = 1;
				break;
			}
			operand += arr[i].second.second.second[l];
		}
		if(mnemonic == "BYTE")
		{
			if(operand[0] == 'C')//�Y�Ĥ@�Ӧr����C�A�ݱN�r�ꤺ�e�নascii�X16�i�� 
			{
				for(int i = 2; i < operand.size()-1; i++)
				{
					int ascii = operand[i];
					objectCode += (decToHex(ascii));
				}
			}
			else
			{
				for(int i = 2; i < operand.size()-1; i++)
				{
					objectCode += (operand[i]);
				}
			}
			
			object_code.push_back(objectCode);
			continue;
		}
		if(mnemonic == "WORD")
		{
			objectCode += decToHex(atoi(operand.c_str()));
			if(objectCode.size() < 6)
			{
				string zero;
				for(int i = 0; i < 6 - objectCode.size(); i++)
				{
					zero += "0";
				}
				objectCode.insert(0, zero);
			}
			object_code.push_back(objectCode);
			continue;
		}
		
		objectCode += get_opcode(mnemonic);//�����e�������p���S���J��ɡA�bobjectCode�[�J�U�нX��opcode 
		if(operand == "\t")//�S����ģ���ԭz�[0000 
		{
			objectCode += "0000";
			object_code.push_back(objectCode);
			continue;
		}
		
		label_address = labels[operand];//�N���e�p��n��label������m�s�Jlabel_address
		
		if(label_address[0] > '7')//�ڤ]�n�_��ԣ�n�g�o�� 
		{
			if(label_address[0] >= 'A')
				label_address[0] -= 15;
			else
				label_address[0] -= 8;
		}
		
		objectCode += label_address;
		
		
		if(flag)//�Y���򩳩w�}�A�hx=1,�[8000(�Q�i��) 
			objectCode = add(objectCode, "8000", 1);

		if(objectCode.size() < 6)
		{
			string zero;
			for(int i = 0; i < 6 - objectCode.size(); i++)
			{
				zero += "0";
			}
			objectCode.insert(0, zero);
		}
		object_code.push_back(objectCode);
		
		
	}
	ofile.open("output.txt",ios::out|ios::trunc);
	ifile.open("Input.txt", ios::in);
	string ste;
	int j=0;
	while(!ifile.eof())
	{
		string s;
		getline(ifile,ste);
		
		if(ste[0]!='.')
		{
				if(arr[j].second.first=="\t"&&arr[j].second.second.second=="\t")
				{
					ofile<<arr[j].first<<"\t"<<arr[j].second.first<<arr[j].second.second.first<<"\t"<<arr[j].second.second.second<<object_code[j]<<endl;
					j++;
				}
				else if(arr[j].second.first!="\t")
				{
					ofile<<arr[j].first<<"\t"<<arr[j].second.first<<"\t"<<arr[j].second.second.first<<"\t"<<arr[j].second.second.second<<"\t"<<object_code[j]<<endl;
					j++;
				}
				else
				{
				ofile<<arr[j].first<<"\t"<<arr[j].second.first<<arr[j].second.second.first<<"\t"<<arr[j].second.second.second<<"\t"<<object_code[j]<<endl;
				j++;	
				}
		}
		else
		{
			ofile<<"\t"<<ste<<endl;
		}
	}
	ofile.close();
	ifile.close();
}
