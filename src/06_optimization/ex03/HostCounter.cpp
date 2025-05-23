#include "HostCounter.h"

#include <algorithm> // for std::find

HostCounter::HostCounter()
{
}

bool HostCounter::isNewHost(std::string hostname)
{
    return myHosts.find(hostname) == myHosts.end();
}

void HostCounter::notifyHost(std::string hostname)
{
    myHosts.insert(hostname); 
}

int HostCounter::getNbOfHosts()
{
    return myHosts.size();
}
