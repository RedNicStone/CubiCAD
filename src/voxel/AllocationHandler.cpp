//
// Created by nic on 09/10/2021.
//

#include "AllocationHandler.h"


VirtualStackAllocation::VirtualStackAllocation(StackAllocator *allocator,
                                               size_t size_,
                                               const std::vector<std::shared_ptr<char[]>> &pages,
                                               uint8_t page_size) {
    pAllocator = allocator;
    size = size_;
    pageSize = page_size;

    for (const std::shared_ptr<char[]>& page: pages)
        mappedPages.push_back({page});
}

char *VirtualStackAllocation::getData(size_t address) {
    return static_cast<char *>((mappedPages.data() + (address >> pageSize))->pData.get())
        + (address & ((1 << pageSize) - 1));
}

void VirtualStackAllocation::resize(size_t newSize) {
    size_t page_count = (newSize + (1 << pageSize) - 1) >> pageSize;
    if (page_count > mappedPages.size()) {
        size_t new_page_count = page_count - mappedPages.size();
        std::vector<std::shared_ptr<char[]>> pages = pAllocator->findOrCreatePages(new_page_count);
        for (const std::shared_ptr<char[]> &page: pages) {
            mappedPages.push_back({page});
        }
    } else if (page_count < mappedPages.size()) {
        size_t discarded_page_count = mappedPages.size() - page_count;
        std::vector<std::shared_ptr<char[]>> discarded_pages(discarded_page_count);
        for (size_t i = 0; i < discarded_page_count; i++) {
            discarded_pages[i] = mappedPages.back().pData;
            mappedPages.pop_back();
        }
    }
}

void StackAllocator::createPages(size_t numberOfPages) {
    size_t chunk_count = (numberOfPages + (1 << allocationChunkSize) - 1) >> allocationChunkSize;
    size_t current_page_count = physicalAllocations.size();
    physicalAllocations.reserve(current_page_count + numberOfPages);
    for (size_t x = 0; x < chunk_count; x++) {
        std::shared_ptr<char[]>
            data = std::unique_ptr<char[]>(new char[((size_t) 1 << pageSize) << allocationChunkSize]);
        for (size_t y = 0; y < 1 << allocationChunkSize; y++) {
            physicalAllocations.emplace_back(std::shared_ptr<char[]>{data, data.get() + (y * 1 << pageSize)});
        }
    }
}

std::vector<std::shared_ptr<char[]>> StackAllocator::findOrCreatePages(size_t numberOfPages) {
    if (numberOfPages <= 0)
        return {};

    std::vector<std::shared_ptr<char[]>> pages(numberOfPages);
    size_t current_page;
    size_t pages_created = 0;
    for (current_page = lastFreePage; current_page < physicalAllocations.size(); current_page++) {
        if (physicalAllocations[current_page].status == PAGE_STATUS_FREE) {
            pages[pages_created] = physicalAllocations[current_page].pData;
            pages_created++;
        }
        if (numberOfPages <= pages_created) {
            lastFreePage = current_page + 1;
            return pages;
        }
    }

    size_t pageIndex = physicalAllocations.size();
    createPages(numberOfPages - pages_created);
    for (; pages_created < numberOfPages; pages_created++) {
        physicalAllocations[pageIndex].status = PAGE_STATUS_USED;
        pages[pages_created] = physicalAllocations[pageIndex].pData;
        pageIndex++;
    }

    lastFreePage = pageIndex + 1;
    return pages;
}

StackAllocator::StackAllocator(uint8_t page_size, uint8_t allocation_chunk_size) {
    pageSize = page_size;
    allocationChunkSize = allocation_chunk_size;
}

VirtualStackAllocation *StackAllocator::makeAllocation(size_t size) {
    size_t page_count = (size + (1 << pageSize) - 1) >> pageSize;
    std::vector<std::shared_ptr<char[]>> pages = findOrCreatePages(page_count);
    VirtualStackAllocation allocation = VirtualStackAllocation(this, size, pages, pageSize);
    virtualAllocations.push_back(allocation);
    return &virtualAllocations.back();
}

StackAllocator::~StackAllocator() {
    /*for (size_t i = physicalAllocations.size(); 0 < i; i -= 1 << allocationChunkSize) {
        //delete[] physicalAllocations[i - (1 << allocationChunkSize)].pData.get();
        for (size_t a = 0; a < 1 << allocationChunkSize; a++)
            physicalAllocations.erase(physicalAllocations.end());
    }
    for (size_t i = 0; i < physicalAllocations.size(); i++) {
        physicalAllocations.erase(physicalAllocations.begin());
    }*/
    physicalAllocations.clear();
    //std::vector<PhysicalPage>(0).swap(physicalAllocations);
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
