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

#include "cs-policy-lru.hpp"
#include "cs.hpp"

namespace nfd {
namespace cs {
namespace lru {

const std::string LruPolicy::POLICY_NAME = "lru";
NFD_REGISTER_CS_POLICY(LruPolicy);

LruPolicy::LruPolicy()
  : Policy(POLICY_NAME)
{
}

void
LruPolicy::doBeforeErase(iterator i)
{
  m_queue.get<1>().erase(i);
}

void
LruPolicy::evictEntries()                             //Mengeluarkan / Menghapus Interest terlama
{
  BOOST_ASSERT(this->getCs() != nullptr);             //cek apakah CS pointer tidak kosong
  while (this->getCs()->size() > this->getLimit()) {  //saat ukuran CS melebihi (>) limit
    BOOST_ASSERT(!m_queue.empty());                   //Cek apakah m_queue tidak kosong, kalo isi lanjut
    iterator i = m_queue.front();                     //Menunjuk yang paling depan dan dimasukkan ke iterator i
    m_queue.pop_front();                              //menghapus interest paling depan (begin) / paling baru
    this->emitSignal(beforeEvict, i);                 //Menghapus data "it" di Tabel
  }
}

void
LruPolicy::doAfterInsert(iterator i)    //kondisi saat interest sudah ada didalam CS
{
  this->insertToQueue(i, true);       //
  this->evictEntries();               //menghapus entry interest paling depan
}
void
LruPolicy::doBeforeUse(iterator i)
{
  this->insertToQueue(i, false);    //Relokasi Interest
}

void
LruPolicy::doAfterRefresh(iterator i)
{
  this->insertToQueue(i, false);    //Relokasi Interest
}

void
LruPolicy::insertToQueue(iterator i, bool isNewEntry)
{
  Queue::iterator it;
  bool isNew = false;                                 //Deklarasi isNew = False
  // push_back only if iterator i does not exist
  std::tie(it, isNew) = m_queue.push_back(i);         // Menambahkan entry interest ke paling belakang

  // NFD_LOG_INFO("PushBack Entries Success");

  //untuk lookup interest // isNewEntry == FALSE
  BOOST_ASSERT(isNew == isNewEntry);
  if (!isNewEntry) {
    m_queue.relocate(m_queue.end(), it);            // Memindahkan entry pada "it" ke paling belakang
    // NFD_LOG_INFO("Relocate Entries Success");
  }                                                 // Memindahkan yang direferensi saja.
}

} // namespace lru
} // namespace cs
} // namespace nfd
