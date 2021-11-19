#include<bits/stdc++.h>
#include "Opcode_map.h"
#include "Pass1.h"
#include "Pass2.h"


using namespace std;

int main()
{
	input();
	addressing();
	generate_object_code();
	generate_machine_code();	
}
