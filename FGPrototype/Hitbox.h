#pragma once

enum HitboxFlags
{
	Action = 0x01,
	Reaction = 0x02,
	Strike = 0x04,
	Throw = 0x08,
	AirThrow = 0x10,
	Projectile = 0x20,
	Collision = 0x40
};

struct Hitbox
{
	int X, Y, Time, Width, Height, Length;
	HitboxFlags Flags;
};

// stuff that is not explicitly in the hitbox data:
// guard point/armor
// counter (Reaction | Strike)
// reflectors (Reaction | Projectile)
// proximity (Action | Collision)
