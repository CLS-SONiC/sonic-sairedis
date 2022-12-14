#include "MACsecEgressFilter.h"

#include <swss/logger.h>

#include <unistd.h>
#include <string.h>

using namespace saivs;

MACsecEgressFilter::MACsecEgressFilter(
        _In_ const std::string &macsecInterfaceName):
    MACsecFilter(macsecInterfaceName)
{
    SWSS_LOG_ENTER();

    // empty intentionally
}

TrafficFilter::FilterStatus MACsecEgressFilter::forward(
        _In_ const void *buffer,
        _In_ size_t length)
{
    SWSS_LOG_ENTER();

    if (write(m_macsecfd, buffer, length) < 0)
    {
        if (errno != ENETDOWN && errno != EIO)
        {
            SWSS_LOG_WARN(
                    "failed to write to macsec device %s fd %d, errno(%d): %s",
                    m_macsecInterfaceName.c_str(),
                    m_macsecfd,
                    errno,
                    strerror(errno));
        }

        if (errno == EBADF)
        {
            // If the MACsec device was deleted by outside,
            // this action should not terminate the Main tap thread.
            // So just report a warning.
            SWSS_LOG_WARN(
                    "ending thread for macsec device %s fd %d",
                    m_macsecInterfaceName.c_str(),
                    m_macsecfd);
        }
    }

    return TrafficFilter::TERMINATE;
}
