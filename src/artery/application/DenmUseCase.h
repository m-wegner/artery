/*
 * Artery V2X Simulation Framework
 * Copyright 2016-2017 Raphael Riebl
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#ifndef ARTERY_DENM_USECASE_H_9BZSURIQ
#define ARTERY_DENM_USECASE_H_9BZSURIQ

#include "artery/application/DenmObject.h"
#include <omnetpp/simtime.h>
#include <vanetza/asn1/denm.hpp>
#include <vanetza/btp/data_request.hpp>

namespace artery
{
namespace denm
{
class UseCase
{
public:
    UseCase();

    /**
     * Evaluate triggering conditions
     * \note invoked for each middleware time stemp (trigger)
     * \return true if use case DENM shall be sent
     */
    bool check();

    /**
     * Fill given DEN message with use case specific data
     * \note General DENM data elements are set by DenmService afterwards!
     * \param denm empty DENM wrapper
     */
    virtual void message(vanetza::asn1::Denm&) = 0;

    /**
     * Fill given BTP data request with use case specific data
     * \note General request data element are set by Denmservice afterwards!
     * \param request empty BTP data request
     */
    virtual void dissemination(vanetza::btp::DataRequestB&) = 0;

    virtual ~UseCase() = default;

protected:
    void setDetectionBlockingTime(omnetpp::SimTime);
    bool isDetectionBlocked();

    virtual void update();
    virtual bool checkPreconditions() = 0;
    virtual bool checkConditions() = 0;

private:
    boost::optional<omnetpp::SimTime> mDetectionBlockingSince;
    omnetpp::SimTime mDetectionBlockingTime;
};

} // namespace denm
} // namespace artery

#endif /* ARTERY_DENM_USECASE_H_9BZSURIQ */
