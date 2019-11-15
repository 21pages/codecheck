#ifndef INTERFACE_H
#define INTERFACE_H

#include "project.h"
#include "treemodel.h"
#include "provider.h"

#define REGISTERS_INITIALIZA     \
{   \
OPENPROJECT_REGISTER;    \
TREEMODEL_REGISTER; \
Provider_Register;\
}


#endif // INTERFACE_H
