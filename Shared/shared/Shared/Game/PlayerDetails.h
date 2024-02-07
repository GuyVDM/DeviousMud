#pragma once
#include "EntityDetails.h"

typedef struct _PlayerDetails : public _EntityDetails 
{
	uint64_t    handle = -1;
	std::string name   = "";

}   PlayerDetails;