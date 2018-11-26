#ifndef BaseLib_Policy_History_h_IsIncluded
#define BaseLib_Policy_History_h_IsIncluded

#include"BaseLib/Templates/BaseTypes.h"
#include"BaseLib/Policy/Lifespan.h"

#include"BaseLib/Export.h"

namespace BaseLib { namespace Policy
{

namespace HistoryKind
{
    enum Type
    {
        KEEP_LAST,
        KEEP_ALL
    };
}

class DLL_STATE History
        : public Templates::ComparableImmutableType<int>
{
private:
   History(HistoryKind::Type kind, int depth, Lifespan lifespan)
       : Templates::ComparableImmutableType<int>(depth)
       , kind_(kind)
       , lifespan_(lifespan)
   { }

public:
   History()
       : Templates::ComparableImmutableType<int>(1)
       , kind_(HistoryKind::KEEP_LAST)
       , lifespan_(Lifespan::Infinite())
   { }

   HistoryKind::Type Kind() const
   {
       return kind_;
   }

   Lifespan Lifetime() const
   {
       return lifespan_;
   }

   int Depth() const
   {
       return this->Clone();
   }

   bool IsKeepAll() const
   {
       return kind_ == HistoryKind::KEEP_ALL;
   }

   bool IsKeepLast() const
   {
       return kind_ == HistoryKind::KEEP_LAST;
   }

public:
   static History KeepAll(Lifespan lifespan = Lifespan::Infinite())
   {
       return History(HistoryKind::KEEP_ALL, std::numeric_limits<int>::max(), lifespan);
   }

   static History KeepLast(int depth, Lifespan lifespan = Lifespan::Infinite())
   {
       return History(HistoryKind::KEEP_LAST, depth, lifespan);
   }

private:
   HistoryKind::Type    kind_;
   Lifespan             lifespan_;
};

}}

#endif
