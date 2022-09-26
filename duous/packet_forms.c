#include "packet.h"

struct form forms[NUM_FORMS] = {
	{
		"Handshake",
		HANDSHAKING,
		0,
		4,
		{
			{
				DTVARINT,
				"Protocol Version"
			},
			{
				DTSTRING,
				"Server Address"
			},
			{
				DTUNSIGNED_SHORT,
				"Server Port"
			},
			{
				DTVARINT,
				"Next State"
			}
		}
	},
	{
		"Request",
		STATUS,
		0,
		0,
		{
			{ (data_type) NULL, NULL }
		}
	},
	{
		"Ping",
		STATUS,
		1,
		1,
		{
			{DTLONG, "Time"}
		}
	},
	{
		"Login Start",
		LOGIN,
		0,
		1,
		{
			{DTSTRING, "Name"}
		}
	},
	{
		"Keep Alive",
		PLAY,
		0,
		1,
		{
			{DTINT, "Keep Alive ID"}
		}
	},
	{
		"Chat Message",
		PLAY,
		1,
		1,
		{
			{DTSTRING, "Message"}
		}
	},
	{
		"Use Entity",
		PLAY,
		2,
		2,
		{
			{DTINT, "Target"},
			{DTBYTE, "Mouse"}
		}
	},
	{
		"Player",
		PLAY,
		3,
		1,
		{
			{DTBOOL, "On Ground"}
		}
	},
	{
		"Player Position",
		PLAY,
		4,
		5,
		{
			{DTDOUBLE, "X"},
			{DTDOUBLE, "FeetY"},
			{DTDOUBLE, "HeadY"},
			{DTDOUBLE, "Z"},
			{DTBOOL, "On Ground"}
		}
	},
	{
		"Player Look",
		PLAY,
		5,
		3,
		{
			{DTFLOAT, "Yaw"},
			{DTFLOAT, "Pitch"},
			{DTBOOL, "On Ground"}
		}
	},
	{
		"Player Position And Look",
		PLAY,
		6,
		7,
		{
			{DTDOUBLE, "X"},
			{DTDOUBLE, "FeetY"},
			{DTDOUBLE, "HeadY"},
			{DTDOUBLE, "Z"},
			{DTFLOAT, "Yaw"},
			{DTFLOAT, "Pitch"},
			{DTBOOL, "On Ground"}
		}
	},
	{
		"Player Digging",
		PLAY,
		7,
		5,
		{
			{DTBYTE, "Status"},
			{DTINT, "X"},
			{DTUNSIGNED_BYTE, "Y"},
			{DTINT, "Z"},
			{DTBYTE, "Face"}
		}
	},
	{
		"Held Item Change",
		PLAY,
		9,
		1,
		{
			{DTSHORT, "Slot"}
		}
	},
	{
		"Animation",
		PLAY,
		10,
		2,
		{
			{DTINT, "Entity ID"},
			{DTBYTE, "Animation"}
		}
	},
	{
		"Entity Action",
		PLAY,
		11,
		3,
		{
			{DTINT, "Entity ID"},
			{DTBYTE, "Action ID"},
			{DTINT, "Jump Boost"}
		}
	},
	{
		"Steer Vehicle",
		PLAY,
		12,
		4,
		{
			{DTFLOAT, "Sideways"},
			{DTFLOAT, "Forward"},
			{DTBOOL, "Jump"},
			{DTBOOL, "Unmount"}
		}
	},
	{
		"Close Window",
		PLAY,
		13,
		1,
		{
			{DTBYTE, "Window id"}
		}
	},
	{
		"Confirm Transaction",
		PLAY,
		15,
		3,
		{
			{DTBYTE, "Window ID"},
			{DTSHORT, "Action number"},
			{DTBOOL, "Accepted"}
		}
	},
	{
		"Enchant Item",
		PLAY,
		17,
		2,
		{
			{DTBYTE, "Window ID"},
			{DTBYTE, "Enchantment"}
		}
	},
	{
		"Update Sign",
		PLAY,
		18,
		7,
		{
			{DTINT, "X"},
			{DTSHORT, "Y"},
			{DTINT, "Z"},
			{DTSTRING, "Line 1"},
			{DTSTRING, "Line 2"},
			{DTSTRING, "Line 3"},
			{DTSTRING, "Line 4"}
		}
	},
	{
		"Player Abilities",
		PLAY,
		19,
		3,
		{
			{DTBYTE, "Flags"},
			{DTFLOAT, "Flying speed"},
			{DTFLOAT, "Walking speed"}
		}
	},
	{
		"Tab-Complete",
		PLAY,
		20,
		1,
		{
			{DTSTRING, "Text"}
		}
	},
	{
		"Client Settings",
		PLAY,
		21,
		6,
		{
			{DTSTRING, "Locale"},
			{DTBYTE, "View distance"},
			{DTBYTE, "Chat flags"},
			{DTBOOL, "Chat colours"},
			{DTBYTE, "Difficulty"},
			{DTBOOL, "Show Cape"}
		}
	},
	{
		"Client Status",
		PLAY,
		22,
		1,
		{
			{DTBYTE, "Action ID"}
		}
	},
};
