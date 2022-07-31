#include <omnetpp.h>
#include <mutex>
#include <vector>
#include "NodeAlloc.hh"

class IBRingAllreduceApp : public omnetpp::cSimpleModule
{
private:
    static constexpr unsigned msgLen_B_ = 648 * 64 * 1024;
    static constexpr unsigned msgMtuLen_B_ = 2048;
    static std::mutex finishCountMutex_;
    static int finishCount_;

    NodeAlloc nodeAllocVec_;
    unsigned rank_;
    unsigned counter_;
    unsigned recv_counter_;
    unsigned num_workers_;
    bool is_sending_ = false;
    std::vector<int> data_;

    virtual ~IBRingAllreduceApp() {}
    omnetpp::cMessage *getMsg(unsigned &msgIdx);
    void trySendNext();

protected:
    virtual void initialize() override;
    virtual void handleMessage(omnetpp::cMessage *msg) override;
    virtual void finish() override {}
};