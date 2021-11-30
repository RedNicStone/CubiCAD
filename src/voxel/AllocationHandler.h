//
// Created by nic on 09/10/2021.
//

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
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
    size_t size{};
    //Allocator*      pAllocator                      {};

  public:
    /** Get the size of the allocation
     * @return The size in bytes
     */
    [[nodiscard]]
    size_t getSize() const { return size; }

    /** Get the pointer to the physical memory at virtual address (may be nonlinear)
     * @param address The virtual address
     * @return Pointer to the physical memory
     */
    virtual char *getData(size_t address) {};

    /** Resize the allocation to the new size specified
     * @param newSize Size which allocation should grow to
     */
    virtual void resize(size_t newSize) {};

    /** Shadow operator of getData
     * @param address The virtual address
     * @return Pointer to the physical memory
     */
    char *operator+(size_t address) { return getData(address); }

    /** Shadow operator of getSize
     * @return The size in bytes
     */
    explicit operator size_t() const { return size; }

    virtual             ~VirtualAllocation() = default;
};

class StackAllocator;

class Allocator {
  public:
    /** Create a virtual allocation with a given size
     * @param size Initial size of the allocation
     * @return The virtual allocation
     */
    virtual VirtualAllocation *makeAllocation(size_t size) {};

    virtual                     ~Allocator() = default;
};

class VirtualStackAllocation : public VirtualAllocation {
  private:
    struct MappedPage { //!< represents a page that has been mapped to a virtual allocation
        std::shared_ptr<char[]> pData; //!< pointer to the physical memory
    };

    StackAllocator *pAllocator{}; //!< Parent allocator
    uint8_t pageSize{}; //!< Size of one page
    std::vector<MappedPage> mappedPages{}; //!< Vector of MappedPage storing all virtual memory pages

  public:
    /** Constructor for VirtualStackAllocation
     * Should NOT be called by the user
     * @param allocator Parent allocator
     * @param size_ Initial site
     * @param pages Initial pages
     * @param page_size Size of one page
     */
    VirtualStackAllocation(StackAllocator *allocator,
                           size_t size_,
                           const std::vector<std::shared_ptr<char[]>> &pages,
                           uint8_t page_size);

    char *getData(size_t address) override;

    void resize(size_t newSize) override;
};

class StackAllocator : public Allocator {
  private:
    enum PageStatus : uint_fast8_t { //!< possible use statuses of an allocated page
        PAGE_STATUS_INVALID = 0b00, PAGE_STATUS_FREE = 0b01, //!< the page is free
        PAGE_STATUS_USED = 0b10, //!< the page is (partially) used
    };

    struct PhysicalPage { //!< represents a page that has been allocated
        std::shared_ptr<char[]> pData; //!< pointer to the allocation
        PageStatus status; //!< status of the allocation

        /** Default constructor where pData is null and the status is invalid
         */
        PhysicalPage() {
            pData = nullptr;
            status = PAGE_STATUS_INVALID;
        }

        /** Constructor where pData is given by data and the status is free
         * @param data Physical address of the data stored in pData
         */
        explicit PhysicalPage(std::shared_ptr<char[]> data) {
            pData = std::move(data);
            status = PAGE_STATUS_FREE;
        }
    };

    uint8_t pageSize{}; //!< Size of one page
    uint8_t allocationChunkSize{}; //!< Size of one chunk
    size_t lastFreePage{}; //!< Index of last free page
    std::vector<PhysicalPage> physicalAllocations{}; //!< Vector of all allocated pages
    std::vector<VirtualStackAllocation> virtualAllocations{}; //!< Vector of all virtual pages

    /** Private function to allocate new pages
     * @param numberOfPages How many should be allocated
     */
    void createPages(size_t numberOfPages);

  public:
    /** Constructor for StackAllocator
     * @param page_size Size of pages in 2^x
     * @param allocation_chunks How many pages should be allocated at once in 2^x;
     * eg bytes = 2^(page_size*allocation_chunks)
     */
    explicit StackAllocator(uint8_t page_size, uint8_t allocation_chunks = 1);

    /** Creates an virtual allocation with a give size
     * @param size Size in bytes
     * @return Pointer to allocation
     */
    VirtualStackAllocation* makeAllocation(size_t size) override;

    /** Finds or, if required, creates pages
     * @param numberOfPages How many pages should be created
     * @return Vector of pointers to pages
     */
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