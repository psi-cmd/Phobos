#pragma once

#include "Commands.h"

#include <BuildingTypeClass.h>
#include <MessageListClass.h>
#include <MapClass.h>
#include <ObjectClass.h>
#include <Utilities/GeneralUtils.h>
#include <Utilities/Debug.h>
#include <Ext/Techno/Body.h>
#include <Ext/TechnoType/Body.h>

enum IFALLSELECTED
{
	ALL_SELECTED = 0,
	NOT_ALL_SELECTED = 1,
};

// Select next idle harvester
class SamePassengerSelectionCommandClass : public CommandClass
{
public:
	// CommandClass
	virtual const char* GetName() const override;
	virtual const wchar_t* GetUIName() const override;
	virtual const wchar_t* GetUICategory() const override;
	virtual const wchar_t* GetUIDescription() const override;
	virtual void Execute(WWKey eInput) const override;

private:
	bool IsWithinScreen(TechnoClass* pTechno) const;
	void printSelectedUnitPassengerSet(
		std::set<std::pair<const char*, std::vector<const char*>>>& selectedUnitPassengerSet
	) const;
	IFALLSELECTED SelectUnitWithSamePassenger(
		std::set<std::pair<const char*, std::vector<const char*>>>& selectedUnitPassengerSet,
		bool fullScreen = false
	) const;
};
