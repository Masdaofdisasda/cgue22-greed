#include "ItemCollection.h"

void item_collection::collect(Physics::PhysicsObject* object)
{
	collectedItems.push_back(object);
	game_properties* item_properties = &object->modelGraphics->game_properties;
	item_properties->is_active = false;
	total_monetary_value_ += item_properties->collectableItemProperties.worth;
	total_weight_ += item_properties->collectableItemProperties.weight;
}

float item_collection::get_total_monetary_value() const
{
	return total_monetary_value_;
}

float item_collection::get_total_weight() const
{
	return total_weight_;
}

std::vector<item_collection::item_info> item_collection::get_list_of_items() const
{
	std::vector<item_info> itemList;
	for (const auto collected_item : collectedItems)
	{
		item_info item;
		item.name = collected_item->modelGraphics->game_properties.display_name;
		item.price = std::to_string(collected_item->modelGraphics->game_properties.collectableItemProperties.worth);
		itemList.push_back(item);
	}
	return itemList;
}

size_t item_collection::size() const
{
	return collectedItems.size();
}
