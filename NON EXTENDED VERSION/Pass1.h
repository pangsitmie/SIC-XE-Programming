#include <bits/stdc++.h>
using namespace std;

string program_name, starting_address;
vector<pair<string, pair<string, pair<string, string>>>> arr;
vector<string> object_code;
map<string, string> labels;
fstream ifile, ofile;

int hexToDec(string str)
{
    int y;
    stringstream stream;
    stream << str;
    stream >> hex >> y;
    return y;
}

string decToHex(int num)
{
    stringstream stream;
    stream << hex << num;
    return stream.str();
}

string add(string str, string adder, int flag)
{
    if (flag) //hex and hex
    {
        int num1 = hexToDec(str);
        int num2 = hexToDec(adder);
        int sum = num1 + num2;

        return decToHex(sum);
    }
    else //hex and dec
    {
        int num1 = hexToDec(str);
        int num2 = atoi(adder.c_str());
        int sum = num1 + num2;

        return decToHex(sum);
    }
}

void input()
{
    string str, temp;
    int index = 0;
    ifile.open("Input.txt", ios::in);

    while (!ifile.eof())
    {
        str.clear();
        arr.push_back(make_pair("\t", make_pair("\t", make_pair("\t", "\t"))));
        getline(ifile, str);
        int flag = 0, found_opcode = 0;

        if (str[0] == (char)46)
        {
            arr.resize(arr.size() - 1);
            continue;
        }

        for (int i = 0; i < str.size(); i++)
        {
            while (i < str.size() && str[i] != ' ' && str[i] != '\t' && str[i] != '\n')
            {
                temp.push_back(str[i++]);
                flag = 1;
            }

            if (flag)
            {
                if (get_opcode(temp) == "-1" && (temp != "RESB" && temp != "RESW" && temp != "BYTE" && temp != "WORD" &&
                                                 temp != "START" && temp != "END"))
                {
                    if (!found_opcode)
                        arr[index].second.first = temp;
                    else
                        arr[index].second.second.second = temp;
                }
                else
                {
                    arr[index].second.second.first = temp;
                    found_opcode = 1;
                }
                temp.clear();
                flag = 0;
            }
        }
        program_name = arr[0].second.first;
        starting_address = arr[0].second.second.second;
        index++;
    }
    ifile.close();
}

void addressing()
{
    arr[0].first = starting_address;
    arr[1].first = starting_address;

    if (arr[0].second.first.size() > 0)             //IF FOUND LABEL FOR THE FIRST TIME
        labels[arr[0].second.first] = arr[0].first; //MARK THAT LABEL WITH THE STARTING ADDRESS

    for (int i = 2; i < arr.size(); i++)
    {
        string mnemonic = arr[i - 1].second.second.first;
        string lastAddress = arr[i - 1].first;
        if (mnemonic != "BYTE" && mnemonic != "RESB" && mnemonic != "RESW")
            arr[i].first = add(lastAddress, "3", 0); //add address by 3, dec and hex
        else
        {
            if (mnemonic == "BYTE") //IF MNEMONIC ==  BYTE
            {
                int bytes;
                string label2 = arr[i - 1].second.second.second; //how much do you want to reserve
                char ch = label2[0];
                if (ch == 'C')
                    bytes = (label2.size() - 3);
                else
                {
                    if ((label2.size() - 3) % 2 == 0) //even
                        bytes = (label2.size() - 3) / 2;
                    else //odd
                        bytes = ((label2.size() - 3) / 2) + 1;
                }
                arr[i].first = add(lastAddress, to_string(bytes), 1);
            }
            else if (mnemonic == "RESB") //IF MNEMONIC ==  RESB
            {
                int reserve = atoi(arr[i - 1].second.second.second.c_str());
                string hexaReserve = decToHex(reserve);
                arr[i].first = add(lastAddress, hexaReserve, 1);
            }
            else //IF MNEMONIC == RESERVE WORD
            {
                int reserve = 3 * atoi(arr[i - 1].second.second.second.c_str());
                string hexaReserve = decToHex(reserve);
                arr[i].first = add(lastAddress, hexaReserve, 1);
            }
        }
        if (arr[i].second.first.size() > 0)             //IF FOUND INSTRUCTIONS
            labels[arr[i].second.first] = arr[i].first; //MARK THE LABEL WITH THAT CURRENT ADDRESS
    }
    ifile.open("Input.txt", ios::in);
    ofile.open("loc.txt", ios::out | ios::trunc);
    string str;
    int j = 0;
    while (!ifile.eof())
    {
        str.clear();
        getline(ifile, str);
        if (str[0] != '.') //IF THE CURRENT STRING IS NOT "."
        {
            if (arr[j].second.first != "\t")
            {
                ofile << arr[j].first << "\t" << arr[j].second.first << "\t" << arr[j].second.second.first << "\t" << arr[j].second.second.second << endl;
                j++;
            }
            else
            {
                ofile << arr[j].first << "\t" << arr[j].second.first << arr[j].second.second.first << "\t" << arr[j].second.second.second << endl;
                j++;
            }
        }
        else // IF CURRENT STRING IS "."
        {
            ofile << arr[j].first << "\t" << str << endl;
        }
    }
    ofile.close();
    ifile.close();
}

void generate_object_code()
{
    string label_address, mnemonic, operand, objectCode = "" ;
    for (int i = 0; i < arr.size(); i++)
    {
        int flag = 0;
        objectCode.clear();
        mnemonic = arr[i].second.second.first;
        if (mnemonic == "RESW" || mnemonic == "RESB" || mnemonic == "END" || mnemonic == "START")
        {
            object_code.push_back("\t");
            continue;
        }
        operand.clear();

        for (int j = 0; j < arr[i].second.second.second.size(); j++)
        {
            if (arr[i].second.second.second[j] == ',')
            {
                flag = 1;
                break;
            }
            operand += arr[i].second.second.second[j];
        }
        //if mnemonic == byte
        if (mnemonic == "BYTE")
        {
            if (operand[0] == 'C')
            {
                for (int k = 2; k < operand.size() - 1; k++)
                {
                    int ascii = operand[k];
                    objectCode += (decToHex(ascii));
                }
            }
            else
            {
                for (int k = 2; k < operand.size() - 1; k++)
                    objectCode += (operand[k]);
            }
            object_code.push_back(objectCode);
            continue;
        }
        //if mnemonic == word
        if (mnemonic == "WORD")
        {
            objectCode += decToHex(atoi(operand.c_str()));
            if (objectCode.size() < 6)
            {
                string zero;
                for (int k = 0; k < 6 - objectCode.size(); k++)
                    zero += "0";
                objectCode.insert(0, zero);
            }
            object_code.push_back(objectCode);
            continue;
        }

        objectCode += get_opcode(mnemonic);

        if (operand == "\t")
        {
            objectCode += "0000";
            object_code.push_back(objectCode);
            continue;
        }

        label_address = labels[operand];
        if (label_address[0] > '7')
        {
            if (label_address[0] >= 'A')
                label_address[0] -= 15;
            else
                label_address[0] -= 8;
        }

        objectCode += label_address;

        if (flag)
            objectCode = add(objectCode, "8000", 1);

        if (objectCode.size() < 6)
        {
            string zero;
            for (int k = 0; k < 6 - objectCode.size(); k++)
                zero += "0";
            objectCode.insert(0, zero);
        }
        object_code.push_back(objectCode);
    }
    //output
    ifile.open("Input.txt", ios::in);
    ofile.open("output.txt", ios::out | ios::trunc);
    string str;
    int j = 0;

    while (!ifile.eof())
    {
        str.clear();
        getline(ifile, str);
        if (str[0] != '.')
        {
            if (arr[j].second.first != "\t")
            {
                ofile << arr[j].first << "\t" << arr[j].second.first << "\t" << arr[j].second.second.first << "\t" << arr[j].second.second.second << "\t" << object_code[j] << endl;
                j++;
            }
            else if (arr[j].second.first == "\t" && arr[j].second.second.second == "\t")
            {
                ofile << arr[j].first << "\t" << arr[j].second.first << arr[j].second.second.first << "\t" << arr[j].second.second.second << object_code[j] << endl;
                j++;
            }
            else //second.first =="\t"
            {
                ofile << arr[j].first << "\t" << arr[j].second.first << arr[j].second.second.first << "\t" << arr[j].second.second.second << "\t" << object_code[j] << endl;
                j++;
            }
        }
        else
            ofile << "\t" << str << endl;
    }
    ifile.close();
    ofile.close();
}