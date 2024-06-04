#pragma once

namespace MyChemistry {
	
	// world weather status
	enum WORLD_WEATHER_STATUS {
		NONE = 0,
		HOT = 1,
		COLD = 2,
		REGIONTHUNDER = 3,
		REGIONRAIN = 4,
		REGIONSNOW = 5
	};

	// chemical reaction
	/*
	* 
	* 
	*/
	enum CHEMICAL_REACTION_STATUS {
		NONE = 0,
		// fire
		BURNING = 1,
		MELT = 2,
		// ice
		FREEZE = 3,
		// electric
		ELECTRICCONDUCTION = 4,
	};


	// fire
	enum FIRE_STATUS {
		NONE = 0,
		ONFIRE = 1,
	};

	enum HYDRO_STATUS {
		NONE = 0,
		ONWET = 1,
	};

	enum ICE_STATUS {
		NONE = 0,
		ONICE = 1,
	};

	enum THUNDER_STATUS {
		NONE = 0,
		ONTHUNDER = 1,
	};
}