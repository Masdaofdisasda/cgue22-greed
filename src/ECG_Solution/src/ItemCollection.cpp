#include "ItemCollection.h"

void ItemCollection::collect(Physics::PhysicsObject* object)
{
	collectedItems.push_back(object);
	game_properties* itemProperties = &object->modelGraphics->game_properties;
	itemProperties->is_active = false;
	totalMonetaryValue += itemProperties->collectableItemProperties.worth;
	totalWeight += itemProperties->collectableItemProperties.weight;
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
		item.name = collectedItems[i]->modelGraphics->game_properties.display_name;
		item.price = std::to_string(collectedItems[i]->modelGraphics->game_properties.collectableItemProperties.worth);
		itemList.push_back(item);
	}
	return itemList;
}

int ItemCollection::size()
{
	return collectedItems.size();
}
