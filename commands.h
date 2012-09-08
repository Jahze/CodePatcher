#ifndef COMMANDS_H_
#define COMMANDS_H_

#include "extension.h"

class ListCommand : public ConCommand {
public:
	ListCommand();
};

class PatchCommand : public ConCommand {
public:
	PatchCommand();
};

class UnpatchCommand : public ConCommand {
public:
	UnpatchCommand();
};

#endif
