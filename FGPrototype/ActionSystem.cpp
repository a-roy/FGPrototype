#include "ActionSystem.h"
#include "CmdTranslator.h"
#include "Entity.h"
#include "LuaEntity.h"

void ActionSystem::Advance(EntityList& entities)
{
	for (int i = 0; i < num_entities; i++)
	{
		if (entities.enabled[i][IAction] && entities.enabled[i][IInput])
		{
			int character = entities.action[i].Character;
			int cancelList = entities.action[i].CancelList;
			Unit *c = Characters[character];
			if (cancelList >= 0)
			{
				std::vector<int> indices = c->Groups[cancelList];
				for (int j = indices.size() - 1; j >= 0; j--)
				{
					// Don't cancel into same move
					if (indices[j] == entities.action[i].Current)
						continue;
					std::vector<Command> commands = c->MoveList[indices[j]].Commands;
					if (entities.input[i].Inputs.CheckCommandInput(commands))
					{
						//std::cout << "Action: " << indices[j] << std::endl;
						entities.action[i].Current = indices[j];
						entities.action[i].Time = 0;
						break;
					}
				}
			}
			Action *current = &c->MoveList[entities.action[i].Current];
			if (entities.action[i].Time >= current->Length)
			{
				entities.action[i].Current = current->RecoverTo;
				entities.action[i].Time = 0;
			}
			current = &c->MoveList[entities.action[i].Current];

			// Call Lua action script
			std::string fn = "action_";
			fn += current->Name;
			Entity e(&entities, i);
			//lua_pushstring(L, fn.c_str());
			//lua_gettable (L, LUA_GLOBALSINDEX);
			lua_getglobal(L, fn.c_str());
			luaU_pushEntity(L, &e);
			if (lua_pcall(L, 1, 0, 0))
				std::cerr << lua_tostring(L, -1) << std::endl;

			entities.action[i].Time += 1;
		}
	}
}