/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#ifndef TCP_OLDSCHOOL_H
#define TCP_OLDSCHOOL_H

#include "tcp-socket-base.h"

namespace ns3 {

/**
 * \ingroup socket
 * \ingroup tcp
 *
 * \brief An implementation of a stream socket using TCP.
 *
 * This class contains the OldSchool implementation of TCP.
 * In summary, we have fast retransmit.
 */
class TcpOldSchool : public TcpSocketBase
{
public:
  /**
   * \brief Get the type ID.
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);
  /**
   * Create an unbound tcp socket.
   */
  TcpOldSchool (void);
  /**
   * \brief Copy constructor
   * \param sock the object to copy
   */
  TcpOldSchool (const TcpOldSchool& sock);
  virtual ~TcpOldSchool (void);

protected:
  virtual Ptr<TcpSocketBase> Fork (void); // Call CopyObject<TcpOldSchool> to clone me
  virtual void NewAck (SequenceNumber32 const& seq); // Inc cwnd and call NewAck() of parent
  virtual void DupAck (const TcpHeader& t, uint32_t count);  // Treat 3 dupack as timeout
  virtual void Retransmit (void); // Retransmit time out

protected:
  uint32_t               m_retxThresh;   //!< Fast Retransmit threshold
};

} // namespace ns3

#endif /* TCP_OLDSCHOOL_H */
