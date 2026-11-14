// Keep GridArray as the first and only project header in this translation unit.
// Aggregate test headers can otherwise hide a missing concrete-type dependency.
#include <util/containers/GridArray.h>

int main()
{
   Util::GridArray<int> array;
   return array.isAllocated() ? 1 : 0;
}
