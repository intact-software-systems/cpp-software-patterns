/*
 * SubFwd.h
 *
 *  Created on: 30. november 2012
 *      Author: KVik
 */

#ifndef DCPS_Sub_SubFwd_h_IsIncluded
#define DCPS_Sub_SubFwd_h_IsIncluded

#include"DCPS/IncludeExtLibs.h"

namespace DCPS { namespace Subscription {

// ------------------------------------
//	DataReader
// -----------------------------------
template <typename DATA>
class DataReader;

//template <typename DATA>
//using DataReaderPtr = std::shared_ptr< DataReader<DATA> >;

//template <typename DATA>
//std::shared_ptr< DataReader<DATA> >;

class AnyDataReaderHolderBase;

FORWARD_CLASS_TRAITS(AnyDataReaderHolderBase)

template <typename DATA>
class AnyDataReaderHolder;

// ------------------------------------
// DataReaderListener
// ------------------------------------
template <typename DATA>
class DataReaderListener;

// ------------------------------------
// Subscriber
// ------------------------------------
class Subscriber;

FORWARD_CLASS_TRAITS(Subscriber)

}} // namespace DCPS::Domain

#endif // DCPS_Sub_SubFwd_h_IsIncluded

