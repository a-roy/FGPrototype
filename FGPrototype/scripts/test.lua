-- Action definitions
--   length:   Number of frames that an action executes.
--   commands: See CmdTranslator.h for details. Directions are defined using
--             "numpad notation", common for fighting game players to describe
--             inputs. 6 is forward, 4 is back, etc.
--   groups:   Can be used to define when a move is legal to use. Useful, for
--             example, when a "normal" move should be able to cancel
--             mid-animation into a "special" move.
actions = {}
n = 1
-- Neutral
actions[n] = {}
actions[n].length = 60
actions[n].name = "Neutral"
actions[n].commands = { "5" }
actions[n].groups = { "Neutral" }
actions[n].anim = "../FGPrototype/models/Neutral.dae"
n = n + 1
-- Forward
actions[n] = {}
actions[n].length = 60
actions[n].name = "Forward"
actions[n].commands = { "6" }
actions[n].groups = { "Neutral" }
actions[n].anim = "../FGPrototype/models/Neutral.dae"
n = n + 1
-- Backward
actions[n] = {}
actions[n].length = 60
actions[n].name = "Backward"
actions[n].commands = { "4" }
actions[n].groups = { "Neutral" }
actions[n].anim = "../FGPrototype/models/Neutral.dae"
n = n + 1
-- Jab
actions[n] = {}
actions[n].length = 14
actions[n].name = "5LP"
actions[n].commands = { "LP*" }
actions[n].groups = { "Neutral" }
actions[n].anim = "../FGPrototype/models/5LP.dae"
n = n + 1
-- Straight
actions[n] = {}
actions[n].length = 23
actions[n].name = "5MP"
actions[n].commands = { "MP*" }
actions[n].groups = { "Neutral" }
actions[n].anim = "../FGPrototype/models/5MP.dae"
n = n + 1
-- Hook
actions[n] = {}
actions[n].length = 33
actions[n].name = "236P"
actions[n].commands = { "236_<(5LP)(6LP)(9LP)>*" }
actions[n].groups = { "Neutral", "Special" }
actions[n].anim = "../FGPrototype/models/Hook.dae"
n = n + 1
-- Uppercut
actions[n] = {}
actions[n].length = 42
actions[n].name = "623P"
actions[n].commands = { "6_23_<(5MP)(3MP)>*" }
actions[n].groups = { "Neutral", "Special" }
actions[n].anim = "../FGPrototype/models/Uppercut.dae"
n = n + 1

-- Action micro-scripts
-- Functions:
--   time()    - get current time in frames
--   move(d)   - move a distance
--   cancel(i) - player can cancel to the specified group, -1 means none
-- Host program populates groups table with indices
function action_Neutral(entity)
    entity:cancel(groups["Neutral"])
end

function action_Forward(entity)
    entity:move(40)
    entity:cancel(groups["Neutral"])
end

function action_Backward(entity)
    entity:move(-30)
    entity:cancel(groups["Neutral"])
end

function action_5LP(entity)
	entity:cancel(-1)
end

function action_5MP(entity)
	if (entity:time(6, 9)) then
		entity:cancel(groups["Special"])
	else
		entity:cancel(-1)
	end
end

function action_236P(entity)
    entity:cancel(-1)
end

function action_623P(entity)
    entity:cancel(-1)
end
