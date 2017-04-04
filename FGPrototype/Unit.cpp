#include "Unit.h"

const Action& Unit::GetNeutralAction()
{
	return MoveList[NeutralAction];
}

void Unit::AddMove(Action& move)
{
	if (MoveList.size() <= move.ID)
	{
		MoveList.resize(move.ID + 1);
	}
	MoveList[move.ID] = move;
}
