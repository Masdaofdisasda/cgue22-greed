#include "ItemCollection.h"

void ItemCollection::collect(Physics::PhysicsObject* object)
{
	collectedItems.push_back(object);
	GameProperties* itemProperties = &object->modelGraphics->gameProperties;
	itemProperties->isActive = false;
}

float ItemCollection::getTotalMonetaryValue()
{
	return totalMonetaryValue;
}

float ItemCollection::getTotalWeight()
{
	return totalWeight;
}

std::vector<ItemCollection::ItemInfo> ItemCollection::getListOfItems()
{
	std::vector<ItemInfo> itemList;
	for (int i = 0; i < collectedItems.size(); i++) {
		ItemInfo item;
		item.name = collectedItems[i]->modelGraphics->gameProperties.displayName;
		item.price = std::to_string(collectedItems[i]->modelGraphics->gameProperties.collectableItemProperties.worth);
		itemList.push_back(item);
	}
	return itemList;
}
