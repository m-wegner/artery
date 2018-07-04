#ifndef VANETRX_H_KTVCYSUX
#define VANETRX_H_KTVCYSUX

#include "inet/linklayer/ieee80211/mac/Rx.h"
#include <boost/circular_buffer.hpp>

class VanetRx : public inet::ieee80211::Rx
{
public:
    VanetRx();
    ~VanetRx();

    static const omnetpp::simsignal_t ChannelLoadSignal;

protected:
    void initialize(int stage) override;
    void handleMessage(omnetpp::cMessage*) override;
    void recomputeMediumFree() override;
    virtual void reportChannelLoad();

private:
    omnetpp::simtime_t channelReportInterval;
    omnetpp::cMessage* channelReportTrigger;
    omnetpp::simtime_t channelLoadLastUpdate;
    boost::circular_buffer<bool> channelLoadSamples;
    double channelBusyRatio;
};

#endif /* VANETRX_H_KTVCYSUX */

