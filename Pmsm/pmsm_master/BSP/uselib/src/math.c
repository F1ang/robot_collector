#include "math.h"

/* б�º���:Output_value��������Remote_value*/
void Ramp_(int *Output_value, int Pace, int Remote_value)
{
	if(Remote_value>*Output_value)
	{
		*Output_value += Pace;
		if(*Output_value>Remote_value)
		{
			*Output_value=Remote_value;
		}
	}
	else if(Remote_value<*Output_value)
	{
		*Output_value -= Pace;
		if(*Output_value<Remote_value)
		{
			*Output_value=Remote_value;
		}
	}
	else *Output_value=Remote_value;
}




