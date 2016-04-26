if (m_node) { std::clog << Simulator::Now ().GetSeconds () << " [node " << m_node->GetId () << "] "; }

#include "tcp-oldschool.h"
#include "ns3/log.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/abort.h"
#include "ns3/node.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("TcpOldSchool");

NS_OBJECT_ENSURE_REGISTERED (TcpOldSchool);

TypeId
TcpOldSchool::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TcpOldSchool")
    .SetParent<TcpSocketBase> ()
    .SetGroupName ("Internet")
    .AddConstructor<TcpOldSchool> ()
    .AddAttribute ("ReTxThreshold", "Threshold for fast retransmit",
                    UintegerValue (3),
                    MakeUintegerAccessor (&TcpOldSchool::m_retxThresh),
                    MakeUintegerChecker<uint32_t> ())
  ;
  return tid;
}

TcpOldSchool::TcpOldSchool (void) : m_retxThresh (3)
{
  NS_LOG_FUNCTION (this);
}

TcpOldSchool::TcpOldSchool (const TcpOldSchool& sock)
  : TcpSocketBase (sock),
    m_retxThresh (sock.m_retxThresh)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC ("Invoked the copy constructor");
}

TcpOldSchool::~TcpOldSchool (void)
{
}

Ptr<TcpSocketBase>
TcpOldSchool::Fork (void)
{
  return CopyObject<TcpOldSchool> (this);
}

/* New ACK (up to seqnum seq) received. */
void
TcpOldSchool::NewAck (SequenceNumber32 const& seq)
{
  NS_LOG_FUNCTION (this << seq);
  NS_LOG_LOGIC ("TcpOldSchool received ACK for seq " << seq <<
                " cwnd " << m_cWnd <<
                " ssthresh " << m_ssThresh);
  TcpSocketBase::NewAck (seq);           // Complete newAck processing
}

/* DupAck */
void
TcpOldSchool::DupAck (const TcpHeader& t, uint32_t count)
{
  NS_LOG_FUNCTION (this << "t " << count);
  if (count == m_retxThresh)
    { // triple duplicate ack triggers fast retransmit (RFC2001, sec.3)
      NS_LOG_INFO ("Triple Dup Ack: old ssthresh " << m_ssThresh << " cwnd " << m_cWnd);
      m_nextTxSequence = m_txBuffer->HeadSequence (); // Restart from highest Ack
      NS_LOG_INFO ("Triple Dup Ack: new ssthresh " << m_ssThresh << " cwnd " << m_cWnd);
      NS_LOG_LOGIC ("Triple Dup Ack: retransmit missing segment at " << Simulator::Now ().GetSeconds ());
      DoRetransmit ();
    }
}

/* Retransmit timeout */
void TcpOldSchool::Retransmit (void)
{
  NS_LOG_FUNCTION (this);
  NS_LOG_LOGIC (this << " ReTxTimeout Expired at time " << Simulator::Now ().GetSeconds ());
  // If erroneous timeout in closed/timed-wait state, just return
  if (m_state == CLOSED || m_state == TIME_WAIT) return;
  // If all data are received (non-closing socket and nothing to send), just return
  if (m_state <= ESTABLISHED && m_txBuffer->HeadSequence () >= m_highTxMark) return;
  m_nextTxSequence = m_txBuffer->HeadSequence (); // Restart from highest Ack
  DoRetransmit ();                          // Retransmit the packet
}

} // namespace ns3
