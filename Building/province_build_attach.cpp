// File: province_build_attach.cpp
// Commit: Update attachment construction to use typed estate and smallholder rules while keeping attachments optional relationships

#include "province.h"

int Province::buildAttached(BuildingType type, int parentId)
{
    const int newId = nextBuildingId_++;

    Building child;
    child.id = newId;
    child.type = type;
    child.parentId = parentId;
    buildings_.push_back(child);

    Building* parent = getBuildingById(parentId);
    if (parent)
    {
        parent->childIds.push_back(newId);
    }

    return newId;
}

AttachmentCounts Province::attachmentCountsForParent(int parentId) const
{
    AttachmentCounts counts;

    const Building* parent = getBuildingById(parentId);
    if (!parent)
    {
        return counts;
    }

    for (int childId : parent->childIds)
    {
        const Building* child = getBuildingById(childId);
        if (!child)
        {
            continue;
        }

        if (isSmallholderBuildingType(child->type))
        {
            ++counts.directSmallholders;
        }
        else if (isEstateBuildingType(child->type))
        {
            ++counts.estates;
        }
    }

    return counts;
}

bool Province::canBuildSmallholderAttachment(int parentId, std::string& reason) const
{
    const Building* parent = getBuildingById(parentId);
    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    const std::vector<BuildingType> allowedTypes = allowedSmallholderTypesForParent(parent->type);
    if (allowedTypes.empty())
    {
        reason = std::string(buildingTypeName(parent->type)) + " cannot host smallholders.";
        return false;
    }

    const AttachmentCounts counts = attachmentCountsForParent(parentId);
    const int capacity = directAttachmentSlotCapacity(parent->type);

    if (counts.directSmallholders + counts.estates >= capacity)
    {
        reason = std::string(buildingDisplayName(*parent)) + " has no free attachment slots.";
        return false;
    }

    reason =
        std::string("Attach ") +
        buildingTypeName(allowedTypes.front()) +
        " to " +
        buildingDisplayName(*parent) +
        ".";

    return true;
}

bool Province::buildSmallholderAttachment(int parentId, std::string& reason)
{
    if (!canBuildSmallholderAttachment(parentId, reason))
    {
        return false;
    }

    const Building* parent = getBuildingById(parentId);
    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    const std::vector<BuildingType> allowedTypes = allowedSmallholderTypesForParent(parent->type);
    if (allowedTypes.empty())
    {
        reason = std::string(buildingTypeName(parent->type)) + " cannot host smallholders.";
        return false;
    }

    const int newId = buildAttached(allowedTypes.front(), parentId);
    const Building* building = getBuildingById(newId);

    reason = std::string("Built ") + (building ? buildingDisplayName(*building) : "Smallholder") + ".";
    return true;
}

bool Province::canBuildEstateAttachment(int parentId, std::string& reason) const
{
    const Building* parent = getBuildingById(parentId);
    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    const std::vector<BuildingType> allowedTypes = allowedEstateTypesForParent(parent->type);
    if (allowedTypes.empty())
    {
        reason = std::string(buildingTypeName(parent->type)) + " cannot host estates.";
        return false;
    }

    const AttachmentCounts counts = attachmentCountsForParent(parentId);
    const int capacity = directAttachmentSlotCapacity(parent->type);

    if (counts.directSmallholders + counts.estates >= capacity)
    {
        reason = std::string(buildingDisplayName(*parent)) + " has no free attachment slots.";
        return false;
    }

    reason =
        std::string("Attach ") +
        buildingTypeName(allowedTypes.front()) +
        " to " +
        buildingDisplayName(*parent) +
        ".";

    return true;
}

bool Province::buildEstateAttachment(int parentId, std::string& reason)
{
    if (!canBuildEstateAttachment(parentId, reason))
    {
        return false;
    }

    const Building* parent = getBuildingById(parentId);
    if (!parent)
    {
        reason = "No valid parent selected.";
        return false;
    }

    const std::vector<BuildingType> allowedTypes = allowedEstateTypesForParent(parent->type);
    if (allowedTypes.empty())
    {
        reason = std::string(buildingTypeName(parent->type)) + " cannot host estates.";
        return false;
    }

    const int newId = buildAttached(allowedTypes.front(), parentId);
    const Building* building = getBuildingById(newId);

    reason = std::string("Built ") + (building ? buildingDisplayName(*building) : "Estate") + ".";
    return true;
}