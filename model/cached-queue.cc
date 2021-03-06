#include "cached-queue.h"
#include <algorithm>
#include <functional>
#include "ns3/ipv4-route.h"
#include "ns3/socket.h"
#include "ns3/log.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("PolsrQueue");

namespace polsr
{
uint32_t
CachedQueue::GetSize ()
{
  return m_queue.size ();
}

bool
CachedQueue::Enqueue (QueueEntry & entry)
{
  for (std::vector<QueueEntry>::const_iterator i = m_queue.begin (); i
       != m_queue.end (); ++i)
    {
      if ((i->GetPacket ()->GetUid () == entry.GetPacket ()->GetUid ())
          && (i->GetIpv4Header ().GetDestination ()
              == entry.GetIpv4Header ().GetDestination ()))
        std::cout<<"already has this packet!"<<std::endl;
        return false;
    }
  m_queue.push_back (entry);
  return true;
}

void
CachedQueue::DropPacketWithDst (Ipv4Address dst)
{
  NS_LOG_FUNCTION (this << dst);
  Purge ();
  for (std::vector<QueueEntry>::iterator i = m_queue.begin (); i
       != m_queue.end (); ++i)
    {
      if (IsEqual (*i, dst))
        {
          Drop (*i, "DropPacketWithDst ");
        }
    }
  m_queue.erase (std::remove_if (m_queue.begin (), m_queue.end (),
                                 std::bind2nd (std::ptr_fun (CachedQueue::IsEqual), dst)), m_queue.end ());
}

bool
CachedQueue::Dequeue (Ipv4Address dst, QueueEntry & entry)
{
  Purge ();
  for (std::vector<QueueEntry>::iterator i = m_queue.begin (); i != m_queue.end (); ++i)
    {
      if (i->GetIpv4Header ().GetDestination () == dst)
        {
          entry = *i;
          m_queue.erase (i);
          return true;
        }
    }
  return false;
}

bool
CachedQueue::Find (Ipv4Address dst)
{
  for (std::vector<QueueEntry>::const_iterator i = m_queue.begin (); i
       != m_queue.end (); ++i)
    {
      if (i->GetIpv4Header ().GetDestination () == dst)
        return true;
    }
  return false;
}

struct IsExpired
{
  bool
  operator() (QueueEntry const & e) const
  {
    return (e.GetExpireTime () < Seconds (0));
  }
};

void
CachedQueue::Purge ()
{
  IsExpired pred;
  for (std::vector<QueueEntry>::iterator i = m_queue.begin (); i
       != m_queue.end (); ++i)
    {
      if (pred (*i))
        {
          Drop (*i, "Drop outdated packet ");
        }
    }
  m_queue.erase (std::remove_if (m_queue.begin (), m_queue.end (), pred),
                 m_queue.end ());
}

void
CachedQueue::Drop (QueueEntry en, std::string reason)
{
  NS_LOG_LOGIC (reason << en.GetPacket ()->GetUid () << " " << en.GetIpv4Header ().GetDestination ());
  en.GetErrorCallback () (en.GetPacket (), en.GetIpv4Header (),
                          Socket::ERROR_NOROUTETOHOST);
  return;
}

void
CachedQueue::Send()
{
	for (std::vector<QueueEntry>::iterator i = m_queue.begin (); i
       != m_queue.end ();)
  {
  	i->Send();
  	i = m_queue.erase(i);
  }
}

void
CachedQueue::SendOnce()
{
	for (std::vector<QueueEntry>::iterator i = m_queue.begin (); i
       != m_queue.end ();)
  {
  	i->Send();
  	m_queue.erase(i);
  	break;
  }
}

}
}
