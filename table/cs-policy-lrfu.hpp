/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2016,  Regents of the University of California,
 *                           Arizona Board of Regents,
 *                           Colorado State University,
 *                           University Pierre & Marie Curie, Sorbonne University,
 *                           Washington University in St. Louis,
 *                           Beijing Institute of Technology,
 *                           The University of Memphis.
 *
 * This file is part of NFD (Named Data Networking Forwarding Daemon).
 * See AUTHORS.md for complete list of NFD authors and contributors.
 *
 * NFD is free software: you can redistribute it and/or modify it under the terms
 * of the GNU General Public License as published by the Free Software Foundation,
 * either version 3 of the License, or (at your option) any later version.
 *
 * NFD is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * NFD, e.g., in COPYING.md file.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NFD_DAEMON_TABLE_CS_POLICY_LRFU_HPP
#define NFD_DAEMON_TABLE_CS_POLICY_LRFU_HPP

#include "cs-policy.hpp"
#include "core/scheduler.hpp"


namespace nfd {
namespace cs {
namespace lrfu {

typedef std::list<iterator> Queue;
typedef Queue::iterator QueueIt;

enum QueueType{
  heaplist,
  linkedlist,
  lrfu
};

struct EntryInfo
{
  QueueType queueType;
  double crf;
  double lastReferencedTime;
  QueueIt queueIt;
  scheduler::EventId moveListEventId;
};

struct EntryItComparator
{

  bool
  operator()(const iterator& a, const iterator& b) const
  {
    return *a < *b;
  }

};

typedef std::map<iterator, EntryInfo*, EntryItComparator> EntryInfoMapLrfu;

/** \brief Priority LRFU cs replacement policy
 *
 * The entries that get removed first are unsolicited Data packets,
 * which are the Data packets that got cached opportunistically without preceding
 * forwarding of the corresponding Interest packet.
 * Next, the Data packets with expired freshness are removed.
 * Last, the Data packets are removed from the Content Store on a pure LRFU basis.
 */
class PriorityLrfuPolicy : public Policy
{
public:
  PriorityLrfuPolicy();

  virtual
  ~PriorityLrfuPolicy();

public:
  static const std::string POLICY_NAME;

private:
  virtual void
  doAfterInsert(iterator i) override;

  virtual void
  doAfterRefresh(iterator i) override;

  virtual void
  doBeforeErase(iterator i) override;

  virtual void
  doBeforeUse(iterator i) override;

  virtual void
  evictEntries() override;

private:
  /** \brief evicts one entry
   *  \pre CS is not empty
   */
  void
  evictOne();

  /** \brief attaches the entry to an appropriate queue
   *  \pre the entry is not in any queue
   */
  void
  attachQueue(iterator i);

  /** \brief detaches the entry from its current queue
   *  \post the entry is not in any queue
   */
  void
  detachQueue(iterator i);

  /** \brief moves an entry from LRFU queue to STALE queue
   */
  void
  updateCRF(iterator i);

  void
  restoreHeapStructure(bool status);

  void
  moveToLinkedList(iterator i);

  void
  moveToHeapList(iterator i);


private:
  // Queue m_queues[QUEUE_MAX];
  Queue m_queues[lrfu];
  EntryInfoMapLrfu m_entryInfoMap;
};

} // namespace priority_lrfu

using lrfu::PriorityLrfuPolicy;

} // namespace cs
} // namespace nfd

#endif // NFD_DAEMON_TABLE_CS_POLICY_LRFU_HPP
