#ifndef INTERFACE_H
#define INTERFACE_H

#include "project.h"
#include "provider.h"
#include "filefolderinterface.h"

#define REGISTERS_INITIALIZA     \
{   \
OPENPROJECT_REGISTER;    \
Provider_Register;\
FileFolder_Register;\
}


//生成顺序:Manager,Project,Provider

#endif // INTERFACE_H
