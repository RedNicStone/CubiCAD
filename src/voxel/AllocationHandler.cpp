//
// Created by nic on 09/10/2021.
//

#include "AllocationHandler.h"

VirtualStackAllocation::VirtualStackAllocation(StackAllocator *allocator,
                                               size_t size_,
                                               const std::vector<char *>& pages,
                                               uint8_t page_size) {
    pAllocator = allocator;
    size = size_;
    pageSize = page_size;

    for (void* page : pages)
        mappedPages.push_back({page});
}

char *VirtualStackAllocation::getData(size_t address) {
    return static_cast<char*>((mappedPages.data() + (address >> pageSize))->pData) + (address & ((1 << pageSize) - 1));
}

void VirtualStackAllocation::resize(size_t newSize) {
    VirtualAllocation::resize(newSize);
}

void StackAllocator::createPages(size_t numberOfPages) {
    for (size_t i = 0; i < numberOfPages; i++) {
        char* data = new char[1 << pageSize];
        physicalAllocations.push_back({data, PAGE_STATUS_USED});
    }
}

std::vector<char *> StackAllocator::findOrCreatePages(size_t numberOfPages) {
    if (numberOfPages <= 0)
        return {};

    std::vector<char *> pages{};
    size_t currentPage{};
    for (currentPage = lastFreePage; currentPage < physicalAllocations.size(); currentPage++) {
        if (physicalAllocations[currentPage].status == PAGE_STATUS_FREE) {
            pages.push_back(physicalAllocations[currentPage].pData);
            numberOfPages--;
        }
        if (numberOfPages <= 0)
            break;
    }

    if (numberOfPages > 0) {
        size_t pageIndex = physicalAllocations.size();
        createPages(numberOfPages);
        for (; pageIndex < physicalAllocations.size(); pageIndex++) {
            pages.push_back(physicalAllocations[pageIndex].pData);
        }
        lastFreePage = pages.size() - 1;
    } else {
        lastFreePage = currentPage;
    }

    return pages;
}

StackAllocator::StackAllocator(uint8_t page_size) {
    pageSize = page_size;
}

VirtualStackAllocation* StackAllocator::makeAllocation(size_t size) {
    std::vector<char *> pages = findOrCreatePages(size >> pageSize);
    VirtualStackAllocation allocation = VirtualStackAllocation(this, size, pages, pageSize);
    virtualAllocations.push_back(allocation);
    return &virtualAllocations.back();
}

StackAllocator::~StackAllocator() {
    for (PhysicalPage page : physicalAllocations) {
        delete page.pData;
    }
}




/*
AllocationHandler::AllocationHandler(size_v page_size) {
    allocationPageSize = page_size;
}

void AllocationHandler::appendPhysicalAllocation(size_v size) {
    PhysicalAllocation allocation{};
    allocation.size = size;
    allocation.pData = new char[size * allocationBlockSize];        // performs the actual allocation
    //allocation.pData = new char[size * allocationBlockSize]();    // makes sure the memory is present but is slower

    AllocationNode* memoryNode = _createNode(size);
}

AllocationHandler::AllocationNode *AllocationHandler::_createNode(size_v size) {
    if (allocationRootNode.nodeInfo.size < size) {
        auto* temp = new AllocationNode(allocationRootNode);
        allocationRootNode = {};
        allocationRootNode.pLeftNode = temp;
        allocationRootNode.nodeInfo.size = temp->nodeInfo.size * 2;

        return _createNode(size);
    } else {
        AllocationNode* res = _createNode(&allocationRootNode, size);

        if (res == nullptr) {
            auto* temp = new AllocationNode(allocationRootNode);
            allocationRootNode = {};
            allocationRootNode.pLeftNode = temp;
            allocationRootNode.nodeInfo.size = temp->nodeInfo.size * 2;

            return _createNode(size);
        }

        return res;
    }
}

AllocationHandler::AllocationNode *AllocationHandler::_createNode(AllocationHandler::AllocationNode *currentNode,
                                                                  size_v size) {
    if (currentNode->nodeInfo.size == size) {
        if (currentNode->nodeInfo.pData == nullptr) {
            return currentNode;
        } else {
            return nullptr;
        }
    } else {
        if (currentNode->pLeftNode == nullptr) {
            currentNode->pLeftNode = new AllocationNode();
        }
        AllocationNode* res = _createNode(currentNode->pLeftNode, size);
        if (res == nullptr) {
            if (currentNode->pRightNode == nullptr) {
                currentNode->pRightNode = new AllocationNode();
            }
            return _createNode(currentNode->pRightNode, size);
        } else {
            return res;
        }
    }
}

void *AllocationHandler::makeAllocation(size_v size) {
    return new char[size];
}

void AllocationHandler::freeAllocation(void *handle) {
}*/
