#pragma once

#include "IRuntimeModule.hpp"
#include "SceneNodeGeometry.hpp"

namespace My {
	class ChemistryManager : __implements IRuntimeModule {
	public:
		~ChemistryManager() override;
	public:
		int Initialize() override;
		void Finalize() override;
		void Tick() override;

		void CreateChemistryBody(SceneGeometryNode& node, SceneObjectGeometry& Geometry);
		void DeleteChemistryBode(SceneGeometryNode& node);

		int CreateChemistryBodies();
		void ClearChemistryBodies();
	protected:


	};

	using TChemitryManager = ChemistryManager;
}