#include "traci/Core.h"
#include "traci/Launcher.h"
#include "traci/LiteAPI.h"
#include "traci/API.h"
#include "traci/SubscriptionManager.h"
#include <inet/common/ModuleAccess.h>

Define_Module(traci::Core)

namespace
{
const omnetpp::simsignal_t initSignal = omnetpp::cComponent::registerSignal("traci.init");
const omnetpp::simsignal_t stepSignal = omnetpp::cComponent::registerSignal("traci.step");
const omnetpp::simsignal_t closeSignal = omnetpp::cComponent::registerSignal("traci.close");
}

namespace traci
{

Core::Core() : m_traci(new API()), m_lite(new LiteAPI(*m_traci)), m_subscriptions(nullptr)
{
}

Core::~Core()
{
    cancelAndDelete(m_connectEvent);
    cancelAndDelete(m_updateEvent);
}

void Core::initialize()
{
    m_connectEvent = new omnetpp::cMessage("connect TraCI");
    m_updateEvent = new omnetpp::cMessage("TraCI step");
    omnetpp::cModule* manager = getParentModule();
    m_launcher = inet::getModuleFromPar<Launcher>(par("launcherModule"), manager);
    m_stopping = par("selfStopping");
    scheduleAt(par("startTime"), m_connectEvent);
    m_subscriptions = inet::getModuleFromPar<SubscriptionManager>(par("subscriptionsModule"), manager, false);
}

void Core::finish()
{
    emit(closeSignal, omnetpp::simTime());
    if (!m_connectEvent->isScheduled()) {
        m_traci->close();
    }
}

void Core::handleMessage(omnetpp::cMessage* msg)
{
    if (msg == m_updateEvent) {
        m_traci->simulationStep();
        if (m_subscriptions) {
            m_subscriptions->step();
        }
        emit(stepSignal, omnetpp::simTime());

        if (!m_stopping || m_traci->simulation.getMinExpectedNumber() > 0) {
            scheduleAt(omnetpp::simTime() + m_updateInterval, m_updateEvent);
        }
    } else if (msg == m_connectEvent) {
        m_traci->connect(m_launcher->launch());
        checkVersion();
        syncTime();
        emit(initSignal, omnetpp::simTime());
        m_updateInterval = time_cast(m_traci->simulation.getDeltaT());
        scheduleAt(omnetpp::simTime() + m_updateInterval, m_updateEvent);
    }
}

void Core::checkVersion()
{
    int expected = par("version");
    if (expected == 0) {
        expected = constants::TRACI_VERSION;
        EV_INFO << "Defaulting expected TraCI API level to client API version " << expected << std::endl;
    }

    const auto actual = m_traci->getVersion();
    EV_INFO << "TraCI server is " << actual.second << " with API level " << actual.first << std::endl;

    if (expected < 0) {
        EV_DEBUG << "No specific TraCI server version requested, accepting connection..." << std::endl;
    } else if (expected != actual.first) {
        EV_FATAL << "Reported TraCI server version does not match expected version " << expected << std::endl;
        error("TraCI server version mismatch (expected: %i, actual: %i)", expected, actual.first);
    }
}

void Core::syncTime()
{
    const omnetpp::SimTime now = omnetpp::simTime();
    ASSERT(now.remainderForUnit(omnetpp::SIMTIME_MS).isZero());
    if (!now.isZero()) {
        m_traci->simulationStep(now.inUnit(omnetpp::SIMTIME_MS));
    }
}

LiteAPI& Core::getLiteAPI()
{
    return *m_lite;
}

} // namespace traci
