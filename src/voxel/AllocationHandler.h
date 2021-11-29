//
// Created by nic on 09/10/2021.
//

#pragma once

#ifndef CUBICAD_ALLOCATIONHANDLER_H_
#define CUBICAD_ALLOCATIONHANDLER_H_

#include <memory>
#include <utility>
#include <vector>

#include "VoxelUtils.h"

class Allocator;

class VirtualAllocation {
  protected:
    size_t          size                            {};
    //Allocator*      pAllocator                      {};

  public:
    [[nodiscard]]
    size_t              getSize() const             { return size; }
    virtual char*       getData(size_t address)     {};

    virtual void        resize(size_t newSize)      {};

    char* operator      +(size_t address)           { return getData(address); }
    explicit operator   size_t() const              { return size; }

    virtual             ~VirtualAllocation()        = default;
};

class StackAllocator;

class Allocator {
  public:
    virtual VirtualAllocation*  makeAllocation(size_t size) {};

    virtual                     ~Allocator()                = default;
};

class VirtualStackAllocation : public VirtualAllocation {
  private:
    struct MappedPage {    //!< represents a page that has been mapped to a user allocation
        std::shared_ptr<char[]>   pData;         //!< pointer to the allocation
    };

    StackAllocator*             pAllocator                  {};
    uint8_t                     pageSize                    {};
    std::vector<MappedPage>     mappedPages                 {};

  public:
    VirtualStackAllocation(StackAllocator* allocator,
                           size_t size_,
                           const std::vector<std::shared_ptr<char[]>>& pages,
                           uint8_t page_size);

    char*                       getData(size_t address)     override;

    void                        resize(size_t newSize)      override;
};

class StackAllocator : public Allocator {
  private:
    enum PageStatus : uint_fast8_t {           //!< possible use stati of an allocated page
        PAGE_STATUS_INVALID     = 0b00,
        PAGE_STATUS_FREE        = 0b01, //!< the page is free
        PAGE_STATUS_USED        = 0b10, //!< the page is (partially) used
    };

    struct PhysicalPage {    //!< represents a page that has been allocated
        std::shared_ptr<char[]> pData;  //!< pointer to the allocation
        PageStatus status;  //!< status of the allocation

        //PhysicalPage(const PhysicalPage&) = delete;
        PhysicalPage() { pData = nullptr; status = PAGE_STATUS_INVALID; }
        PhysicalPage(std::shared_ptr<char[]> data) { pData = std::move(data); status = PAGE_STATUS_FREE; }

        //PhysicalPage& operator=(const PhysicalPage&) = delete;
    };

    uint8_t               pageSize                    {};
    uint8_t               allocationChunkSize                    {};
    size_t lastFreePage{};
    std::vector<PhysicalPage>   physicalAllocations {};
    std::vector<VirtualStackAllocation>   virtualAllocations {};

    void createPages(size_t numberOfPages);

  public:
    explicit StackAllocator(uint8_t page_size, uint8_t allocation_chunks=1);

    VirtualStackAllocation* makeAllocation(size_t size) override;
    std::vector<std::shared_ptr<char[]>> findOrCreatePages(size_t numberOfPages);

    ~StackAllocator() override;
};

/*
class AllocationHandler {       //! class to handle allocations. Automatically reserves memory and organized allocations
                                //! based on the buddy allocation algorithm and an allocation stack
  private:
    
    struct PhysicalAllocation {                             //! represents physical memory allocated as a chunk
        void*                           pData;                  //!< pointer to the allocation
        size_v                          size;                   //!< size of the allocation in blocks
    };
    
    enum AllocationNodeStatus : uint_fast8_t {              //!< possible use stati of an allocation node
        ALLOCATION_NODE_STATUS_UNUSED       = 0b001,            //!< the allocation is completely unused
        ALLOCATION_NODE_STATUS_PARTIALLY    = 0b010,            //!< the allocation is partially used
        ALLOCATION_NODE_STATUS_USED         = 0b100,            //!< the allocation is completely used
    };
    
    struct AllocationNodeInfo {                             //! information about the allocation node
        void*                           pData;                  //!< this holds a pointer to memory if the node is memory bound
        size_v                          size;                   //!< holds the size of the allocation in blocks
        AllocationNodeStatus            status;                 //!< weather or not the memory is used
    };
    
    struct AllocationNode {                                 //! node that represents an allocation. Buddy allocation
                                                            //! algorithm
        AllocationNode*                 pLeftNode;              //!< pointer to the left buddy allocation
        AllocationNode*                 pRightNode;             //!< pointer to thr right buddy allocation
        
        AllocationNodeInfo              nodeInfo;               //!< holds info about the allocation
    };
    
    struct UserAllocationHolder {                           //! holds information about a user allocation
        std::vector<AllocationNode*>    pNodesV;                //!< vector of pointers to allocation nodes
        size_v                          totalSize;              //!< total size of the allocation
    };
    
    
    std::vector<UserAllocationHolder>   userAllocationsV        {};     //!< vector of user allocations
    std::vector<PhysicalAllocation>     physicalAllocationsV    {};     //!< vector of physical allocations
    
    AllocationNode                      allocationRootNode      {};     //!< root node for the allocation tree
    size_v                              allocationPageSize        ;     //!< size of the smallest physical allocation
                                                                        //!< that will be done


    void                appendPhysicalAllocation(size_v size);


    AllocationNode *    _createNode(size_v size);
    AllocationNode *    _createNode(AllocationNode* currentNode, size_v size);

  public:
    explicit            AllocationHandler(size_v page_size = 4194304);

    void *              makeAllocation(size_v size);
    void                freeAllocation(void* handle);
};*/

#endif //CUBICAD__ALLOCATIONHANDLER_H_
