#pragma once

class Instruction;
class VM
{
	
public:
	VM();
	~VM();
	void RunInstruction(Instruction inst);
};


class Instruction
{
public:
	Instruction();
	~Instruction();
};
