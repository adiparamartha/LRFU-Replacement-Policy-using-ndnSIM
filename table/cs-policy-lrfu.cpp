/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/**
 * Copyright (c) 2014-2017,  Regents of the University of California,
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
 * 
 * Created by : Made Adi Paramartha Putra
 * Bandung Institute of Technology
 * May 2019
 */

#include "cs-policy-lrfu.hpp"
#include "cs.hpp"
#include "core/logger.hpp"
#include "math.h"

NFD_LOG_INIT("LrfuPolicy");

namespace nfd {
namespace cs {
namespace lrfu {

const std::string PriorityLrfuPolicy::POLICY_NAME = "lrfu";
NFD_REGISTER_CS_POLICY(PriorityLrfuPolicy);

PriorityLrfuPolicy::PriorityLrfuPolicy()
  : Policy(POLICY_NAME)
{
}

PriorityLrfuPolicy::~PriorityLrfuPolicy()
{
  for (auto entryInfoMapPair : m_entryInfoMap) {
    delete entryInfoMapPair.second;
  }
}

//Not Yet
void
PriorityLrfuPolicy::doBeforeErase(iterator i)
{
  NFD_LOG_INFO("Before Erase Function" );

  this->detachQueue(i);
}

void
PriorityLrfuPolicy::doAfterRefresh(iterator i)
{
  NFD_LOG_INFO("After Refresh Function" );

  EntryInfo* entryInfo = m_entryInfoMap[i];

  if(entryInfo->queueType == linkedlist){
    NFD_LOG_INFO("Linked List Location");
    this->updateCRF(i);
    this->restoreHeapStructure(true);
    this->moveToHeapList(i);
  }

  else if (entryInfo->queueType == heaplist){
    NFD_LOG_INFO("Already in HeapList");
    this->updateCRF(i);
    this->restoreHeapStructure(false);
  }
  
}


void
PriorityLrfuPolicy::doAfterInsert(iterator i)
{
  NFD_LOG_INFO("After Insert Function" );

  this->attachQueue(i);
  this->evictEntries();
}

void
PriorityLrfuPolicy::doBeforeUse(iterator i)
{
  NFD_LOG_INFO("Before Use Function" );

  EntryInfo* entryInfo = m_entryInfoMap[i];

  if(entryInfo->queueType == linkedlist){
    NFD_LOG_INFO("Linked List Location");
    this->updateCRF(i);
    this->restoreHeapStructure(true);
    this->moveToHeapList(i);
  }

  else if (entryInfo->queueType == heaplist){
    NFD_LOG_INFO("Already in HeapList");
    this->updateCRF(i);
    this->restoreHeapStructure(false);
  }

}

void
PriorityLrfuPolicy::evictEntries()
{
  BOOST_ASSERT(this->getCs() != nullptr);

  while (this->getCs()->size() > this->getLimit()) {
    this->evictOne();
  }
}

void
PriorityLrfuPolicy::evictOne()
{
  BOOST_ASSERT(!m_queues[heaplist].empty()||
               !m_queues[linkedlist].empty());

   iterator i;
    if(!m_queues[linkedlist].empty()){
       i = m_queues[linkedlist].front();
     }

   this->detachQueue(i);
   this->emitSignal(beforeEvict, i);
}

void
PriorityLrfuPolicy::attachQueue(iterator i)
{
  BOOST_ASSERT(m_entryInfoMap.find(i) == m_entryInfoMap.end());

  ndn::time::steady_clock::TimePoint init_now = ::ndn::time::steady_clock::now();
  ndn::time::milliseconds init_milliseconds = ::ndn::time::duration_cast<::ndn::time::milliseconds>(init_now.time_since_epoch());
  double init_time_ms = init_milliseconds.count();
  double init_currentTime = init_time_ms/1000;

  EntryInfo* entryInfo = new EntryInfo();
    entryInfo->crf = 1.0;
    entryInfo->lastReferencedTime=init_currentTime;

    if(this->getCs()->size() == this->getLimit()+1){
      NFD_LOG_INFO("** New Interest **");
      this->restoreHeapStructure(true);
      entryInfo->queueType = heaplist;
    }
    else if (this->getCs()->size() > 7) {
      entryInfo->queueType = linkedlist;
      NFD_LOG_INFO("Type : LinkedList");
    }
    else{
      entryInfo->queueType = heaplist;
      NFD_LOG_INFO("Type : HeapList");
    }

  Queue& queue = m_queues[entryInfo->queueType];
  entryInfo->queueIt = queue.insert(queue.end(), i);
  m_entryInfoMap[i] = entryInfo;

  for(auto it = m_queues[heaplist].begin(); it != m_queues[heaplist].end(); ++it)
     {
      if(m_entryInfoMap[*it]->queueType == heaplist){
      NFD_LOG_INFO("EntryInfo HeapList " << m_entryInfoMap[*it]);
     }
  }
 
  for(auto it = m_queues[linkedlist].begin(); it != m_queues[linkedlist].end(); ++it)
  {
    if(m_entryInfoMap[*it]->queueType == linkedlist){
    NFD_LOG_INFO("EntryInfo LinkedList " << m_entryInfoMap[*it]);
    }
  }          


  NFD_LOG_DEBUG("Size: "<< this->getCs()->size() <<"CRF : "<< m_entryInfoMap[i]->crf << ", entryInfo: "<< m_entryInfoMap[i]);

}

void
PriorityLrfuPolicy::detachQueue(iterator i)
{
  BOOST_ASSERT(m_entryInfoMap.find(i) != m_entryInfoMap.end());

  EntryInfo* entryInfo = m_entryInfoMap[i];

  m_queues[entryInfo->queueType].erase(entryInfo->queueIt);

  NFD_LOG_DEBUG("Erased " << m_entryInfoMap[i]);
  m_entryInfoMap.erase(i);
  delete entryInfo;

}

void
PriorityLrfuPolicy::updateCRF(iterator i)
{
  BOOST_ASSERT(m_entryInfoMap.find(i) != m_entryInfoMap.end());

  EntryInfo* entryInfo = m_entryInfoMap[i];

  double initTime = entryInfo->lastReferencedTime;
  double lastCrf = entryInfo->crf;

  ndn::time::steady_clock::TimePoint now = ::ndn::time::steady_clock::now();
  ndn::time::milliseconds milliseconds = ::ndn::time::duration_cast<::ndn::time::milliseconds>(now.time_since_epoch());
  double time_ms = milliseconds.count();
  double currentTime = time_ms/1000;

  double newReferencedTime = currentTime - initTime;

  entryInfo->crf = 1.0 + pow((1.0/2.0),(0.1 * newReferencedTime)) * lastCrf;
  entryInfo->lastReferencedTime = currentTime;

  NFD_LOG_DEBUG("Update CRF : "<< m_entryInfoMap[i]->crf << " New Referenced: " <<m_entryInfoMap[i]->lastReferencedTime);
  NFD_LOG_DEBUG("currentTime: " <<currentTime << " CRF: " << lastCrf << " lastReferencedTime: " <<initTime);

}

void
PriorityLrfuPolicy::restoreHeapStructure(bool status)
{
  BOOST_ASSERT(!m_queues[heaplist].empty());

  iterator lowestCrfPointer;
  double tempCrf = 9999.0;
  double getCrf;
  int list = 1;

  for(auto it = m_queues[heaplist].begin(); it != m_queues[heaplist].end(); ++it)
  {
      if(m_entryInfoMap[*it]->queueType == heaplist){
        NFD_LOG_INFO( list << ". HeapList CRF Value: " << m_entryInfoMap[*it]->crf);

        getCrf = m_entryInfoMap[*it]->crf;
          if ( tempCrf > getCrf)
          {
            tempCrf = getCrf;
            lowestCrfPointer = *it;
          }
          else if (tempCrf == getCrf){
          	//do nothing
          }
      }
      list ++;
  }
  NFD_LOG_INFO("-- Lowest CRF: " << tempCrf);
  NFD_LOG_INFO("-- Lowest Iterator: " << m_entryInfoMap[lowestCrfPointer]);

  if (status == true){
    this->moveToLinkedList(lowestCrfPointer);
  }

}

void
PriorityLrfuPolicy::moveToLinkedList(iterator i)
{
    EntryInfo* entryInfo = m_entryInfoMap[i];
    m_queues[heaplist].erase(entryInfo->queueIt);

    entryInfo->queueType = linkedlist;
    Queue&queue = m_queues[linkedlist];
    entryInfo->queueIt = queue.insert(queue.end(),i);
    m_entryInfoMap[i] = entryInfo;

    if(m_entryInfoMap[i]->queueType==linkedlist){
      NFD_LOG_INFO("Move to LinkedList (1) ; Crf = " << m_entryInfoMap[i]->crf);
  }
}

void
PriorityLrfuPolicy::moveToHeapList(iterator i)
{
    EntryInfo* entryInfo = m_entryInfoMap[i];
    m_queues[linkedlist].erase(entryInfo->queueIt);

    entryInfo->queueType = heaplist;
    Queue&queue = m_queues[heaplist];
    entryInfo->queueIt = queue.insert(queue.end(),i);
    m_entryInfoMap[i] = entryInfo;

    NFD_LOG_INFO("Move To HeapList (0) ; CRF = " <<m_entryInfoMap[i]->crf);

}

} // namespace lrfu
} // namespace cs
} // namespace nfd
