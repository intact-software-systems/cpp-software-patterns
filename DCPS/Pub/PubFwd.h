/*
 * PubFwd.h
 *
 *  Created on: 30. november 2012
 *      Author: KVik
 */

#ifndef DCPS_Pub_PubFwd_h_IsIncluded
#define DCPS_Pub_PubFwd_h_IsIncluded

#include"DCPS/IncludeExtLibs.h"

namespace DCPS { namespace Publication
{

// ------------------------------------
//	DataWriter
// -----------------------------------
template <typename DATA>
class DataWriter;

//template <typename DATA>
//using DataWriterPtr = std::shared_ptr< DataWriter<DATA> >;

//template <typename DATA>
//std::shared_ptr< DataWriter<DATA> >;

template <typename DATA>
class AnyDataWriterHolder;

class AnyDataWriterHolderBase;

FORWARD_CLASS_TRAITS(AnyDataWriterHolderBase)

// ------------------------------------
// DataWriterListener
// ------------------------------------
template <typename DATA>
class DataWriterListener;

// ------------------------------------
// Publisher
// ------------------------------------
class Publisher;

FORWARD_CLASS_TRAITS(Publisher)

}} // namespace DCPS::Domain

#endif // DCPS_Pub_PubFwd_h_IsIncluded

