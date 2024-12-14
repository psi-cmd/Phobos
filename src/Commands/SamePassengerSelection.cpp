#include "SamePassengerSelection.h"
#include <TacticalClass.h>


const char* SamePassengerSelectionCommandClass::GetName() const
{
	return "Same passenger selection";
}

const wchar_t* SamePassengerSelectionCommandClass::GetUIName() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_TRANSPORT_WITH_SAME_PASSENGER", L"Transport with same passenger");
}

const wchar_t* SamePassengerSelectionCommandClass::GetUICategory() const
{
	return CATEGORY_SELECTION;
}

const wchar_t* SamePassengerSelectionCommandClass::GetUIDescription() const
{
	return GeneralUtils::LoadStringUnlessMissing("TXT_SAME_PASSENGER_SELECTION_DESC", L"Select the next transport that has the same passenger.");
}

void SamePassengerSelectionCommandClass::printSelectedUnitPassengerSet(
	std::set<std::pair<const char*, std::vector<const char*>>>& selectedUnitPassengerSet
	) const
{
	for (auto& pair : selectedUnitPassengerSet)
	{
		for (auto& passenger : pair.second)
		{
			Debug::Log("Unit: %s, Passenger: %s\n", pair.first, passenger);
		}
	}
}

bool SamePassengerSelectionCommandClass::IsWithinScreen(TechnoClass* pTechno) const
{
	auto isVisible = TacticalClass::Instance->CoordsToClient(pTechno->GetCoords()).second;
	return isVisible;
}

	IFALLSELECTED SamePassengerSelectionCommandClass::SelectUnitWithSamePassenger(
	std::set<std::pair<const char*, std::vector<const char*>>>& selectedUnitPassengerSet,
	bool fullScreen
	) const
{
	auto ifAllSelected = ALL_SELECTED;
	auto pFirstObject = MapClass::Instance->NextObject(nullptr);
	auto pNextObject = pFirstObject;
	do
	{
		auto pTechno = abstract_cast<TechnoClass*>(pNextObject);
		if (pTechno)
		{
			auto pType = pTechno->GetTechnoType();
			std::vector<const char*> passengerIDs;
			if (pType->Passengers > 0)
			{
				auto pPassenger = pTechno->Passengers.GetFirstPassenger();
				while (pPassenger)
				{
					passengerIDs.push_back(pPassenger->get_ID());
					pPassenger = static_cast<FootClass*>(pPassenger->NextObject);
				}
				std::sort(passengerIDs.begin(), passengerIDs.end());
			}
			if (selectedUnitPassengerSet.find(std::make_pair(pTechno->get_ID(), passengerIDs)) != selectedUnitPassengerSet.end())
			{
				// get if the unit is within the current screen
				if (this->IsWithinScreen(pTechno) || fullScreen)
				{
					if (!pNextObject->IsSelected)
					{
						pNextObject->Select();
						ifAllSelected = NOT_ALL_SELECTED;
					}
				}
			}
		}
		pNextObject = MapClass::Instance->NextObject(pNextObject);
	} while (pNextObject != pFirstObject);
	return ifAllSelected;
}

void SamePassengerSelectionCommandClass::Execute(WWKey eInput) const
{
	// compare get_ID() of vehicle and passengers

	MapClass::Instance->SetTogglePowerMode(0);
	MapClass::Instance->SetWaypointMode(0, false);
	MapClass::Instance->SetRepairMode(0);
	MapClass::Instance->SetSellMode(0);

	std::set<std::pair<const char*, std::vector<const char*>>> selectedUnitPassengerSet;
	for (int i = 0; i < ObjectClass::CurrentObjects->Count; i++)
	{
		auto pUnit = abstract_cast<TechnoClass*>(ObjectClass::CurrentObjects->GetItem(i));
		if (pUnit)
		{
			auto pType = pUnit->GetTechnoType();
			std::vector<const char*> passengerIDs;
			if (pType->Passengers > 0)
			{
				auto pPassenger = pUnit->Passengers.GetFirstPassenger();
				while (pPassenger)
				{
					passengerIDs.push_back(pPassenger->get_ID());
					pPassenger = static_cast<FootClass*>(pPassenger->NextObject);
				}
				std::sort(passengerIDs.begin(), passengerIDs.end());
			}
			selectedUnitPassengerSet.insert(std::make_pair(pUnit->get_ID(), passengerIDs));
		}
	}

	this->printSelectedUnitPassengerSet(selectedUnitPassengerSet);
	// smart T selection for one screen
	auto allSelected = this->SelectUnitWithSamePassenger(selectedUnitPassengerSet);
    if (allSelected == IFALLSELECTED::ALL_SELECTED)
	{
		// smart T selection for the whole map
		this->SelectUnitWithSamePassenger(selectedUnitPassengerSet, true);
	}
}
